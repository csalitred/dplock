#include "board_pins.h"
#include "button.h"
#include "driver/gpio.h"
#include "esp_log.h"



void dplock_button_init(void) {
    // intialize all processes needed
    
    gpio_config_t io_conf = {};

    io_conf.pin_bit_mask = (1ULL << PIN_BUTTON);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    
    gpio_config(&io_conf);
}

