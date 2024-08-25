#include "dplock_data_storage.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <string.h>
#include "esp_log.h"

static const char *TAG = "DPLOCK";

static authorized_devices[MAX_AUTH_DEVICES][6];  // 6 bytes for BD_ADDR
static authorized_device_count = 0;

/*
* Static arrays for storing data
*/
static dplock_accel_data_point_t accel_data_storage[MAX_DATA_POINTS];
static size_t accel_data_count = 0;

static dplock_rtc_data_t rtc_data_storage[MAX_DATA_POINTS];
static size_t rtc_data_count = 0;

static dplock_breach_event_t breach_event_storage[MAX_DATA_POINTS];
static size_t breach_event_count = 0;


esp_err_t dplock_data_storage_init(void) {
    accel_data_count = 0;
    rtc_data_count = 0;
    breach_event_count = 0;

    // initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize NVS");
        return ret;
    }

    return ESP_OK;
}

esp_err_t dplock_store_accel_data(const dplock_accel_data_point_t *data) {
    if (accel_data_count >= MAX_DATA_POINTS) {
        return ESP_ERR_NO_MEM;  // No space left
    }
    accel_data_storage[accel_data_count++] = *data;
    return ESP_OK;
}

esp_err_t dplock_get_accel_data(dplock_accel_data_point_t *data) {
    if (accel_data_count == 0) {
        return ESP_ERR_NOT_FOUND;  // No data available
    }
    *data = accel_data_storage[--accel_data_count];
    return ESP_OK;
}

esp_err_t dplock_store_rtc_data(const dplock_rtc_data_t *data) {
    if (rtc_data_count >= 100) {
        return ESP_ERR_NO_MEM;
    }
    rtc_data_storage[rtc_data_count++] = *data;
    return ESP_OK;
}

esp_err_t dplock_get_rtc_data(dplock_rtc_data_t *data) {
    if (rtc_data_count == 0) {
        return ESP_ERR_NOT_FOUND;
    }
    *data = rtc_data_storage[--rtc_data_count];
    return ESP_OK;
}

esp_err_t dplock_store_breach_event(const dplock_breach_event_t *event) {
    if (breach_event_count >= 100) {
        return ESP_ERR_NO_MEM;
    }
    breach_event_storage[breach_event_count++] = *event;
    return ESP_OK;
}

esp_err_t dplock_get_breach_event(dplock_breach_event_t *event) {
    if (breach_event_count == 0) {
        return ESP_ERR_NOT_FOUND;
    }
    *event = breach_event_storage[--breach_event_count];
    return ESP_OK;
}

esp_err_t dplock_clear_data(void) {
    accel_data_count = 0;
    rtc_data_count = 0;
    breach_event_count = 0;

    return ESP_OK;
}

void dplock_add_authorized_device(const bd_addr_t bd_addr) {
    if (authorized_device_count >= MAX_AUTH_DEVICES) {
        ESP_LOGW(TAG, "Max authorized devices reached");
        return;
    }
    memcpy(authorized_devices[authorized_device_count++], bd_addr, sizeof(bd_addr_t));
}

bool dplock_is_device_authorized(const bd_addr_t bd_addr) {
    for (size_t i = 0; i < authorized_device_count; i++) {
        if (memcmp(authorized_devices[i], bd_addr, sizeof(bd_addr_t)) == 0) {
            return true;
        }
    }   
    return false;
}

void dplock_start_ble_security(const bd_addr_t bd_addr) {
    // Example BLE security function
    if (dplock_is_device_authorized(bd_addr)) {
        ESP_LOGI(TAG, "Starting BLE security with authorized device");
        // Implement security procedures here
    } else {
        ESP_LOGW(TAG, "Unauthorized device attempting to pair");
    }
}

void dplock_remove_authorized_device(const bd_addr_t bd_addr) {
    for (size_t i = 0; i < authorized_device_count; i++) {
        if (memcmp(authorized_devices[i], bd_addr, sizeof(bd_addr_t)) == 0) {
            // Shift remaining devices down
            memmove(&authorized_devices[i], &authorized_devices[i + 1],
                (authorized_device_count - i -1) * sizeof(bd_addr_t));        
            authorized_device_count--;
            ESP_LOGI(TAG, "Device removed from authorized list");
        }
    }
    ESP_LOGW(TAG, "Device not found in authorized list");
}
