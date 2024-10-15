#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
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
#include "nvs_manager.h"

static const char* TAG = "MAIN";

typedef enum {IDLE, AUTHORIZED_ACCESS, DROP, BREACH, NUM_STATES} state_t;

typedef struct {
    bool is_button_released;
    bool is_light_detected;
    bool is_ble_connection;
    int led_position;
    accel_data_t accel_data;
} inputs_t;

extern volatile bool interrupt_occurred;

state_t run_idle_state(inputs_t*);
state_t run_authorized_access_state(inputs_t*);
state_t run_dropped_state(inputs_t*);
state_t run_breached_state(inputs_t*);

void init(void);

state_t (*state_table[NUM_STATES])(inputs_t*) = {run_idle_state, run_authorized_access_state, run_dropped_state, run_breached_state};

void app_main(void)
{
    init();
    esp_err_t ret;
    inputs_t inputs = {0};

    // Initialize accelerometer
    ret = accelerometer_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize accelerometer: %s", esp_err_to_name(ret));
        return;
    }
    ESP_LOGI(TAG, "Accelerometer initialized successfully");

    ret = nvs_manager_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize NVS: %s", esp_err_to_name(ret));
        return;
    }

    state_t current_state = IDLE;
    state_t next_state = current_state;

    while (1) {
        ret = accelerometer_read_data(&inputs.accel_data);
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "Accelerometer data - X: %d, Y: %d, Z: %d, x, y, z,", 
                    inputs.accel_data.accel_x, inputs.accel_data.accel_y, inputs.accel_data.accel_z);
            ESP_LOGI(TAG, "Tilt angle: %.2f degrees", inputs.accel_data.tilt_angle);
            ESP_LOGI(TAG, "Drop detected: %s", inputs.accel_data.is_dropped ? "Yes" : "No");
        } else {
            ESP_LOGE(TAG, "Failed to read accelerometer data: %s", esp_err_to_name(ret));
        } 

        switch(current_state) {
            case IDLE:
                next_state = run_idle_state(&inputs);
                break;
            case AUTHORIZED_ACCESS:
                next_state = run_authorized_access_state(&inputs);
                break;
            case DROP:
                next_state = run_dropped_state(&inputs);
                break;
            case BREACH:
                next_state = run_breached_state(&inputs);
                break;
            default:
                break;
        }
        current_state = next_state;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    
}

void init(void)
{
    button_init();
    servo_init();

    ESP_LOGI(TAG, "Initialization complete");
}

state_t run_idle_state(inputs_t* inputs)
{
    // wait for interrupts form accel, photodiode, button etc. 
    ESP_LOGI(TAG, "In IDLE state");

    if (inputs->is_ble_connection) {
        return AUTHORIZED_ACCESS;
    }
    if (inputs->accel_data.is_dropped || inputs->accel_data.tilt_angle) {
        return DROP;
    }
    return IDLE;
}

state_t run_authorized_access_state(inputs_t* inputs)
{
    ESP_LOGI(TAG, "Entering AUTHORIZED_ACCESS state");
    // LOG DATA FOR AUTHORIZED ACCESS IN FLASH MEMORY HERE: TIME STAMP
    // stay here until no longer BLE connection established
    if (!inputs->is_ble_connection) {
        return IDLE;
    }
    return AUTHORIZED_ACCESS;
}

state_t run_dropped_state(inputs_t* inputs)
{
    ESP_LOGI(TAG, "Entering DROP state");\
     // Log timestamp and data of event in flash memory
    return IDLE;
}

state_t run_breached_state(inputs_t* inputs)
{
    ESP_LOGI(TAG, "Entering BREACH state");
    while(1) {
         // Log timestamp and stay here forever
        vTaskDelay(pdMS_TO_TICKS(60000)); // Log every minute
    }
    return BREACH; // This line will never be reached
}