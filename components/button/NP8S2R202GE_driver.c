#include "board_pins.h"
#include "NP8S2R202GE_driver.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_intr_alloc.h"

volatile bool button_pressed = false;

typedef enum {CLOSED, OPEN} button_state_t;

typedef struct {
    bool is_button_pressed;
} button_input_t;

button_state_t door_closed(button_input_t *button_input);
button_state_t door_open(button_input_t *button_input);

static void IRAM_ATTR gpio_isr_handler(void* arg) 
{
    button_pressed = gpio_get_level(PIN_BUTTON) == 0;
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
    // printf("Minimum free heap size: %"PRIu32" bytes\n", esp_get_free_heap_size());
}

void button_state(void) 
{
    button_init();
    
    while(1) {
        if (button_pressed) {
            printf("button is pressed\n");
        } else {
            printf("Button is not pressed\n");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
