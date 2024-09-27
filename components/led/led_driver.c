#include "led_driver.h"
#include "driver/gpio.h"
#include "board_pins.h"
#include "esp_err.h"

void rgb_led_init(void)
{
    gpio_config_t led_config = {};
    led_config.pin_bit_mask = (1ULL << PIN_LED_RED | 1ULL << PIN_LED_GREEN | 1ULL << PIN_LED_BLUE);
    led_config.mode = GPIO_MODE_OUTPUT;
    led_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    led_config.pull_up_en = GPIO_PULLUP_DISABLE;
    led_config.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&led_config);

    gpio_set_level(PIN_LED_RED, 1);
    gpio_set_level(PIN_LED_GREEN, 1);
    gpio_set_level(PIN_LED_BLUE, 1);
}

void set_rgb_color(uint8_t red, uint8_t green, uint8_t blue) 
{
    gpio_set_level(PIN_LED_RED, red ? 0 : 1);
    gpio_set_level(PIN_LED_GREEN, green ? 0 : 1);
    gpio_set_level(PIN_LED_BLUE, blue ? 0 : 1);
}
