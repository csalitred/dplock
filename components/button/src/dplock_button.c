#include "dplock_button.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "DPLOCK_BUTTON";

esp_err_t dplock_button_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PIN_BUTTON),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    if (gpio_config(&io_conf) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure button GPIO");
        return ESP_FAIL;
    }
    return ESP_OK;
}

void dplock_button_isr_handler(void *arg) {
    // ISR code here
}

// Function to read the state of the button
bool dplock_is_button_pressed(void) {
    return (gpio_get_level(PIN_BUTTON) == 0);  // check if button press pulls the pin low or high
}
