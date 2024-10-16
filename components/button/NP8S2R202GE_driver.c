#include "board_pins.h"
#include "NP8S2R202GE_driver.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_intr_alloc.h"

volatile bool door_state_changed = false;

static void IRAM_ATTR gpio_isr_handler(void* arg) 
{
    door_state_changed = true;
}

void button_init(void) 
{
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << PIN_BUTTON);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;
    io_conf.pull_down_en = 0;
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    gpio_config(&io_conf);

    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
    gpio_isr_handler_add(PIN_BUTTON, gpio_isr_handler, NULL);
}

void button_read(button_input_t *data) 
{
    data->is_door_opened = (gpio_get_level(PIN_BUTTON) == 1);
    data->state_changed = door_state_changed;

    if (door_state_changed) {
        door_state_changed = false;
    }
}
