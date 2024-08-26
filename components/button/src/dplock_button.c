#include "dplock_pins.h"
#include "dplock_button.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define DEBOUNCE_TIME_MS  50

static const char *TAG = "DPLOCK_BUTTON";
TaskHandle_t button_task_handle = NULL;
static volatile bool is_button_pressed = false;

 static void dplock_button_isr_handler(void *arg);

static void dplock_button_task(void *arg) {
    TickType_t last_change_time = 0;

    while(1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        TickType_t current_time = xTaskGetTickCount();
        if ((current_time - last_change_time) >= pdMS_TO_TICKS(DEBOUNCE_TIME_MS)) {
            last_change_time = current_time;

            bool current_state = gpio_get_level(PIN_BUTTON) == 0;
            if (current_state != is_button_pressed) {
                is_button_pressed = current_state;
                ESP_LOGI(TAG, "Button state changed: %s", is_button_pressed ? "PRESSED" : "RELEASED");
            }
        }
    }
}

esp_err_t dplock_button_init(void) {
    gpio_config_t io_conf = {};

    io_conf.pin_bit_mask = (1ULL << PIN_BUTTON);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    
    esp_err_t ret = gpio_config(&io_conf);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        return ret;
    }

    return gpio_isr_handler_add(PIN_BUTTON, dplock_button_isr_handler, NULL);
}

static void IRAM_ATTR dplock_button_isr_handler(void *arg) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(button_task_handle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

esp_err_t dplock_button_start_task(void) {
    if (button_task_handle == NULL) {
        BaseType_t result = xTaskCreate(dplock_button_task, "button_task", 2048, NULL, 10, &button_task_handle);
        if (result != pdPASS) {
            return ESP_FAIL;
        }
    }
    return ESP_OK;
}

bool dplock_button_is_pressed(void) {
    return is_button_pressed;
}