#include "board_pins.h"
#include "button.h"
#include "esp_intr_alloc.h"
#include "esp_attr.h"
#include "driver/gpio.h"

static volatile bool button_pressed = false;

static void IRAM_ATTR gpio_isr_handler(void* arg) 
{
    button_pressed = true;
}

void dplock_button_init(void) 
{

    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << PIN_BUTTON);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;
    io_conf.pull_down_en = 0;
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    gpio_config(&io_conf);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(PIN_BUTTON, gpio_isr_handler, NULL);
}

bool button_is_pressed(void)
{
    return button_pressed;
}
void button_clear_press(void)
{
    button_pressed = false;
}