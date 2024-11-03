#include "ble_manager.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "nvs_manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "BLE_MANAGER";

static const ble_uuid128_t gatt_svr_svc_uuid = BLE_UUID128_INIT(SERVICE_UUID);
static const ble_uuid128_t gatt_svr_chr_historical_sensor_data_uuid = BLE_UUID128_INIT(HISTORICAL_DATA_SERVICE_UUID);
static const ble_uuid128_t gatt_svr_chr_sensor_control_uuid = BLE_UUID128_INIT(SENSOR_CONTROL_UUID);
static const ble_uuid128_t gatt_svr_chr_sensor_metadata_uuid = BLE_UUID128_INIT(SENSOR_METADATA_UUID);

static uint8_t ble_addr_type;
static bool ble_connected = false;

// Define buffers for characteristic data
static uint8_t historical_data[512];  // Buffer for manifest data
static uint8_t sensor_control[1];
static uint8_t sensor_metadata[128];
static uint16_t historical_data_attr_handle = 0;
static uint16_t sensor_metadata_attr_handle = 0;

extern volatile bool door_unlock;  // Defined in main.c to control door lock

// GATT server callbacks
static int gatt_svr_chr_access_historical_data(uint16_t conn_handle, uint16_t attr_handle,
                                               struct ble_gatt_access_ctxt *ctxt, void *arg) {
    int rc;
    switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR: {
            // Get events from NVS
            event_record_t events[MAX_EVENTS];
            uint32_t count;
            esp_err_t ret = nvs_manager_get_all_events(events, &count);
            if (ret == ESP_OK) {
                // Calculate total size needed
                size_t manifest_size = sizeof(uint32_t) + (count * sizeof(event_record_t));
                if (manifest_size <= sizeof(historical_data)) {
                    // Pack the data
                    memcpy(historical_data, &count, sizeof(uint32_t));
                    memcpy(historical_data + sizeof(uint32_t), events, count * sizeof(event_record_t));
                    
                    rc = os_mbuf_append(ctxt->om, historical_data, manifest_size);
                    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
                }
            }
            return BLE_ATT_ERR_UNLIKELY;
        }
        default:
            return BLE_ATT_ERR_UNLIKELY;
    }
}

static int gatt_svr_chr_access_sensor_control(uint16_t conn_handle, uint16_t attr_handle,
                                              struct ble_gatt_access_ctxt *ctxt, void *arg) {
    int rc;
    switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_WRITE_CHR:
            rc = ble_hs_mbuf_to_flat(ctxt->om, sensor_control, sizeof(sensor_control), NULL);
            if (rc == 0) {
                if (sensor_control[0] == 0x01) {  // Unlock command
                    door_unlock = true;
                    ESP_LOGI(TAG, "Door unlock command received");
                }
            }
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        default:
            return BLE_ATT_ERR_UNLIKELY;
    }
}

static int gatt_svr_chr_access_sensor_metadata(uint16_t conn_handle, uint16_t attr_handle,
                                               struct ble_gatt_access_ctxt *ctxt, void *arg) {
    int rc;
    switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR: {
            trip_info_t trip_info;
            if (nvs_manager_get_trip_info(&trip_info) == ESP_OK) {
                rc = os_mbuf_append(ctxt->om, &trip_info, sizeof(trip_info_t));
                return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
            }
            return BLE_ATT_ERR_UNLIKELY;
        }
        default:
            return BLE_ATT_ERR_UNLIKELY;
    }
}

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) {
            {
                .uuid = &gatt_svr_chr_historical_sensor_data_uuid.u,
                .access_cb = gatt_svr_chr_access_historical_data,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                .val_handle = &historical_data_attr_handle,
            },
            {
                .uuid = &gatt_svr_chr_sensor_control_uuid.u,
                .access_cb = gatt_svr_chr_access_sensor_control,
                .flags = BLE_GATT_CHR_F_WRITE,
            },
            {
                .uuid = &gatt_svr_chr_sensor_metadata_uuid.u,
                .access_cb = gatt_svr_chr_access_sensor_metadata,
                .flags = BLE_GATT_CHR_F_READ,
                .val_handle = &sensor_metadata_attr_handle,
            },
            {
                0,
            }
        },
    },
    {
        0,
    },
};

esp_err_t update_historical_data(uint8_t *data, size_t len)
{
    if (len > sizeof(historical_data)) {
        len = sizeof(historical_data);
    }
    memcpy(historical_data, data, len);
    
    if (historical_data_attr_handle != 0) {
        struct os_mbuf *om = ble_hs_mbuf_from_flat(data, len);
        if (om) {
            ble_gatts_notify_custom(0, historical_data_attr_handle, om);
            return ESP_OK;
        }
    }
    return ESP_FAIL;
}

esp_err_t update_sensor_metadata(uint8_t *data, size_t len)
{
    if (len > sizeof(sensor_metadata)) {
        len = sizeof(sensor_metadata);
    }
    memcpy(sensor_metadata, data, len);
    
    if (sensor_metadata_attr_handle != 0) {
        struct os_mbuf *om = ble_hs_mbuf_from_flat(data, len);
        if (om) {
            ble_gatts_notify_custom(0, sensor_metadata_attr_handle, om);
            return ESP_OK;
        }
    }
    return ESP_FAIL;
}

void ble_init(void) 
{
    ESP_LOGI(TAG, "Initializing NimBLE");

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize NimBLE
    ESP_ERROR_CHECK(nimble_port_init());

    // Initialize the NimBLE host configuration
    ble_hs_cfg.sync_cb = ble_app_on_sync;

    // Initialize services
    ble_svc_gap_init();
    ble_svc_gatt_init();

    // Set the default device name
    ble_svc_gap_device_name_set(DEVICE_NAME);

    // Initialize our GATT services
    ble_gatts_count_cfg(gatt_svr_svcs);
    ble_gatts_add_svcs(gatt_svr_svcs);

    // Start the NimBLE host task
    nimble_port_freertos_init(host_task);
}

void ble_advertise(void) 
{
    struct ble_gap_adv_params adv_params = {0};
    struct ble_hs_adv_fields fields = {0};

    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
    fields.name = (uint8_t *)DEVICE_NAME;
    fields.name_len = strlen(DEVICE_NAME);
    fields.name_is_complete = 1;

    ble_gap_adv_set_fields(&fields);

    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
}

int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            ESP_LOGI(TAG, "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK" : "FAILED");
            if (event->connect.status == 0) {
                ble_connected = true;
            } else {
                ble_advertise();
            }
            break;
        case BLE_GAP_EVENT_DISCONNECT:
            ESP_LOGI(TAG, "BLE GAP EVENT DISCONNECT");
            ble_connected = false;
            ble_advertise();
            break;
        case BLE_GAP_EVENT_ADV_COMPLETE:
            ESP_LOGI(TAG, "BLE GAP EVENT ADV COMPLETE");
            ble_advertise();
            break;
        case BLE_GAP_EVENT_SUBSCRIBE:
            ESP_LOGI(TAG, "BLE GAP EVENT SUBSCRIBE");
            break;
        default:
            break;    
    }
    return 0;
}

void ble_app_on_sync(void) 
{
    ble_hs_id_infer_auto(0, &ble_addr_type);
    ble_advertise();
}

void host_task(void *param)
{
    ESP_LOGI(TAG, "BLE Host Task Started");
    nimble_port_run();
    nimble_port_freertos_deinit();
}

bool is_ble_connected(void)
{
    return ble_connected;
}