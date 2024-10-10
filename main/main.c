#include <stdlib.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c_manager.h"
#include "hs5086wp.h"
#include "NP8S2R202GE_driver.h"
#include "esp_sntp.h"
#include "esp_netif_sntp.h"
#include "esp_log.h"
#include "accelerometer_driver.h"
#include "ble_manager.h"
#include "nvs_flash.h"

static const char* TAG = "MAIN";

typedef enum {IDLE, AUTHORIZED_ACCESS, DROP, BREACH, MAX_STATES} state_t;

typedef struct {
    bool is_button_released;
    bool is_light_detected;
    bool is_ble_connection;
    bool is_dropped;
    int led_position;
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    bool tilt_detected;
} inputs_t;

extern volatile bool interrupt_occurred;

state_t run_idle_state(inputs_t*);
state_t run_authorized_access_state(inputs_t*);
state_t run_dropped_state(inputs_t*);
state_t run_breached_state(inputs_t*);

state_t (*state_table[MAX_STATES])(inputs_t*) = {run_idle_state, run_authorized_access_state, run_dropped_state, run_breached_state};

inputs_t inputs = {0};

void init(void)
{
    button_init();
    servo_init();

    ESP_LOGI(TAG, "Initialization complete");
}

state_t run_idle_state(inputs_t* inputs)
{
    ESP_LOGI(TAG, "In IDLE state");
    ESP_LOGI(TAG, "Accelerometer data - X: %d, Y: %d, Z: %d", 
             inputs->accel_x, inputs->accel_y, inputs->accel_z);

    if (inputs->is_ble_connection) {
        return AUTHORIZED_ACCESS;
    }
    if (inputs->is_dropped || inputs->tilt_detected) {
        return DROP;
    }
    return IDLE;
}

state_t run_authorized_access_state(inputs_t* inputs)
{
    ESP_LOGI(TAG, "Entering AUTHORIZED_ACCESS state");
    // Implement authorized access logic here
    if (!inputs->is_ble_connection) {
        return IDLE;
    }
    return AUTHORIZED_ACCESS;
}

state_t run_dropped_state(inputs_t* inputs)
{
    ESP_LOGI(TAG, "Entering DROP state");
    ESP_LOGI(TAG, "Accelerometer data - X: %d, Y: %d, Z: %d", 
             inputs->accel_x, inputs->accel_y, inputs->accel_z);
     // Log timestamp
    return IDLE;
}

state_t run_breached_state(inputs_t* inputs)
{
    ESP_LOGI(TAG, "Entering BREACH state");
    while(1) {
         // Log timestamp
        vTaskDelay(pdMS_TO_TICKS(60000)); // Log every minute
    }
    return BREACH; // This line will never be reached
}

void accelerometer_scan(void)
{
        esp_err_t ret;

    // Initialize I2C
    ret = i2c_master_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize I2C: %s", esp_err_to_name(ret));
        return;
    }
    ESP_LOGI(TAG, "I2C initialized successfully");

    // Initialize accelerometer
    ret = accelerometer_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize accelerometer: %s", esp_err_to_name(ret));
        return;
    }
    ESP_LOGI(TAG, "Accelerometer initialized successfully");

    // Main loop
    while (1) {
        int16_t x, y, z;
        ret = accelerometer_read(&x, &y, &z);
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "Accelerometer data - X: %d, Y: %d, Z: %d", x, y, z);
        } else {
            ESP_LOGE(TAG, "Failed to read accelerometer data: %s", esp_err_to_name(ret));
        }

        float tilt_angle;
        ret = accelerometer_detect_tilt(&tilt_angle);
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "Tilt angle: %.2f degrees", tilt_angle);
        } else {
            ESP_LOGE(TAG, "Failed to detect tilt: %s", esp_err_to_name(ret));
        }

        bool drop_detected;
        ret = accelerometer_detect_drop(&drop_detected);
        if (ret == ESP_OK) {
            if (drop_detected) {
                ESP_LOGI(TAG, "Drop detected!");
            }
        } else {
            ESP_LOGE(TAG, "Failed to detect drop: %s", esp_err_to_name(ret));
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
    }
}

void app_main(void)
{
    esp_err_t ret;

    // Initialize NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize BLE
    ble_init();
    ESP_LOGI(TAG, "BLE initialized");

    // Start BLE advertising
    ble_advertise();
    ESP_LOGI(TAG, "BLE advertising started");

}