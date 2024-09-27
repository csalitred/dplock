#include "board_pins.h"
#include "NP8S2R202GE_driver.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_intr_alloc.h"

volatile bool button_released = false;

static void IRAM_ATTR gpio_isr_handler(void* arg) 
{
    if (gpio_get_level(PIN_BUTTON) == 1) {
        button_released = true;
    }
}

void button_init(void) 
{
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << PIN_BUTTON);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;
    io_conf.pull_down_en = 0;
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    gpio_config(&io_conf);

    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
    gpio_isr_handler_add(PIN_BUTTON, gpio_isr_handler, NULL);
}

bool is_button_released(void) 
{
    if (button_released) {
        button_released = false;
        return true;
    }
    return false;
}

bool get_button_state(void) 
{
    return gpio_get_level(PIN_BUTTON) == 1;
}