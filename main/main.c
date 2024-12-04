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
#include "esp_bt.h"
#include "bpw21r_driver.h"

static const char* TAG = "MAIN";

typedef enum {IDLE, AUTHORIZED_ACCESS, BREACH, NUM_STATES} state_t;

typedef struct {
    accel_data_t accel_data;
    button_input_t button_state;
} inputs_t;

extern volatile bool interrupt_occurred;
bool first_breach = false;
volatile bool door_unlock = false;

state_t run_idle_state(inputs_t*);
state_t run_authorized_access_state(inputs_t*);
state_t run_breached_state(inputs_t*);

void init(void);

state_t (*state_table[NUM_STATES])(inputs_t*) = {run_idle_state, run_authorized_access_state, run_breached_state};

void app_main(void)
{
    init();
    esp_err_t ret;
    inputs_t inputs = {0};
    int light_value;

    state_t current_state = IDLE;
    state_t next_state = current_state;

    while (1) {
        if (current_state != BREACH) {
            ret = accelerometer_read_data(&inputs.accel_data);
            if (ret == ESP_OK) {
                ESP_LOGI(TAG, "Accelerometer data - X: %d, Y: %d, Z: %d, x, y, z,", 
                        inputs.accel_data.accel_x, inputs.accel_data.accel_y, inputs.accel_data.accel_z);
                ESP_LOGI(TAG, "Tilt angle: %.2f degrees", inputs.accel_data.tilt_angle);
                ESP_LOGI(TAG, "Drop detected: %s", inputs.accel_data.is_dropped ? "Yes" : "No");
            } else {
                ESP_LOGE
                (TAG, "Failed to read accelerometer data: %s", esp_err_to_name(ret));
            } 

            button_read(&inputs.button_state);
            if (inputs.button_state.state_changed) {
                if (inputs.button_state.is_door_opened) {
                    ESP_LOGI(TAG, "Door was opened!");
                } else {
                    ESP_LOGI(TAG, "Door has been closed");
                }
            }

            if (photodiode_read(&light_value) == ESP_OK) {
                ESP_LOGI(TAG, "Light value %d", light_value);
            }

            if (is_light_detected()) {
                ESP_LOGI(TAG, "Light is Detected");
            }

            if (is_ble_connected()) {
                ESP_LOGI(TAG, "BLE is connected");
            } else {
                ESP_LOGI(TAG, "BLE is not connected");
            }
        }

        switch(current_state) {
            case IDLE:
                next_state = run_idle_state(&inputs);
                break;
            case AUTHORIZED_ACCESS:
                next_state = run_authorized_access_state(&inputs);
                break;
            case BREACH:
                next_state = run_breached_state(&inputs);
                break;
            default:
                break;
        }
        current_state = next_state;

        if (current_state != BREACH) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
    
}

void init(void)
{
    button_init();
    servo_init();
    ble_init();
    ESP_ERROR_CHECK(photodiode_init()); 

    //Initialize accelerometer
    esp_err_t ret = accelerometer_init();
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to initialize accelerometer: %s", esp_err_to_name(ret));
    return;
    }

    ESP_LOGI(TAG, "Accelerometer initialized successfully");

    esp_err_t ret = nvs_manager_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize NVS: %s", esp_err_to_name(ret));
        return;
    }

    trip_info_t trip_info = {
        .origin = "Charlotte",
        .destination = "Texas",
        .contents = "Apparel",
        .container_id = "CONT123"
    };

    ret = nvs_manager_start_trip(&trip_info);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to store trip info: %s", esp_err_to_name(ret));
    }
    ESP_LOGI(TAG, "Trip info stored successfully");

    trip_info_t retrieved_info;
    ret = nvs_manager_get_trip_info(&retrieved_info);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to retrieve trip info: %s", esp_err_to_name(ret));
        return;
    }
    ESP_LOGI(TAG, "Retrieved trip info - Origin: %s, Destination: %s, Container: %s", 
             retrieved_info.origin, retrieved_info.destination, retrieved_info.container_id);

    ESP_LOGI(TAG, "Retrieved trip info - Origin: %s, Destination: %s, Container: %s", 
             retrieved_info.origin, retrieved_info.destination, retrieved_info.container_id);
    
}

state_t run_idle_state(inputs_t* inputs)
{
    // wait for interrupts form accel, photodiode, button etc. 
    ESP_LOGI(TAG, "In IDLE state");
    
    if (is_ble_connected()) {
        return AUTHORIZED_ACCESS;
    }

    if (inputs->accel_data.is_dropped || inputs->accel_data.tilt_angle >= 45.0) {
        nvs_manager_log_event(EVENT_POTENTIAL_DROP);
        return IDLE;
    }

    if (inputs->button_state.is_door_opened && !is_ble_connected() && !door_unlock) {  // removed is_light_detected due to issues with photodiode
        first_breach = true;
        return BREACH;  
    }
    return IDLE;
}

state_t run_authorized_access_state(inputs_t* inputs)
{
    ESP_LOGI(TAG, "In AUTHORIZED_ACCESS state");
    ESP_LOGI(TAG, "Door unlock flag: %d", door_unlock);
    ESP_LOGI(TAG, "BLE connected: %d", is_ble_connected());

    if (!is_ble_connected()) {
        ESP_LOGI(TAG, "BLE disconnected, returning to IDLE");
        return IDLE;
    }

    if (door_unlock) {
        ESP_LOGI(TAG, "Processing door unlock command");
        servo_unlock();
        door_unlock = false;
        ESP_LOGI(TAG, "Door unlock processed");

        if (inputs->button_state.is_door_opened) {
            nvs_manager_log_event(EVENT_AUTHORIZED_ACCESS);
            ESP_LOGI(TAG, "Authorized access event logged");
        }
    }

    return AUTHORIZED_ACCESS;
}

state_t run_breached_state(inputs_t* inputs)
{
    if (first_breach) {
    ESP_LOGI(TAG, "CONTAINER BREACH!!!!");
    esp_err_t ret = nvs_manager_log_event(EVENT_BREACH);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to log breach event: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Breach event logged succesfully");
    }
    first_breach = false;
    }
    ESP_LOGI(TAG, "Container remains in breached state");
    return BREACH; 
}