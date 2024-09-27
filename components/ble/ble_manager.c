#include "ble_manager.h"
#include "esp_log.h"

static const char *TAG = "BLE_DEVICE";

static uint8_t ble_addr_type;
// static uint8_t characteristic_value = 0;

void ble_init(void) 
{
    ESP_LOGI(TAG, "Initializing NimBLE");

    esp_err_t ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init NimBLE: %d", ret);
        return;
    }

    ble_hs_cfg.sync_cb = ble_app_on_sync;
    nimble_port_freertos_init(host_task);
}

void ble_advertise(void) 
{
    struct ble_hs_adv_fields fields;
    memset(&fields, 0, sizeof(fields));

    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.tx_pwr_lvl_is_present = 1;
    fields.name = (uint8_t *)DEVICE_NAME;
    fields.name_len = strlen(DEVICE_NAME);
    fields.name_is_complete = 1;

    ble_gap_adv_set_fields(&fields);
}

int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            ESP_LOGI(TAG, "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK" : "FAILED");
            if (event->connect.status != 0) {
                ble_advertise();
            }
            break;
        case BLE_GAP_EVENT_DISCONNECT:
            ESP_LOGI(TAG, "BLE GAP EVENT DISCONNECT");
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
    nimble_port_run();
}