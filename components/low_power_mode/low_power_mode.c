#include "low_power_mode.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "board_pins.h"

static const char *TAG = "LOW POWER MODE";
 void configure_wake_up_sources(void)
 {
    // Double check with components so that GPIO modes are configured correctly
    ESP_LOGI(TAG, "Configuring wake-up sources...");
    esp_sleep_enable_ext0_wakeup(PIN_BUTTON, 1);
    esp_sleep_enable_ext1_wakeup((1ULL << PIN_ADC), ESP_EXT1_WAKEUP_ANY_HIGH);
    esp_sleep_enable_ext1_wakeup((1ULL << PIN_I2C_SDA), ESP_EXT1_WAKEUP_ANY_HIGH);

    // Handle BLE, and other interrupts needed
 }

 void enter_light_sleep_mode(void) 
 {
    ESP_LOGI(TAG, "Entering light sleep mode");
    configure_wake_up_sources();
    esp_light_sleep_start();
    ESP_LOGI(TAG, "Woke up from light sleep");
 }

 void enter_deep_sleep(void)
 {
    ESP_LOGI(TAG, "Entering deep sleep mode");
    configure_wake_up_sources();
    esp_deep_sleep_start();
 }
