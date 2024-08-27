#include "dplock_pins.h"
#include "dplock_button.h"
#include "driver/gpio.h"
#include "esp_log.h"

/*
* Need to review FreeRTOS documentation on how to implement Task handling
* with semaphores and queues
* https://www.freertos.org/Documentation/02-Kernel/02-Kernel-features/01-Tasks-and-co-routines/00-Tasks-and-co-routines
*/
TaskHandle_t button_task_handle = NULL;

static SemaphoreHandle_t buton_semaphore = NULL;

static IRAM_ATTR dplock_button_isr_handler(void *arg) {
    xSemaphoreGiveFromISR(button_semaphore, NULL);
}

static void dplock_button_task(void *pvParameters) {
    dplock_button_state_t button_state = BUTTON_CLOSED;
}

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

static void  dplock_button_start_task(void) {
}
