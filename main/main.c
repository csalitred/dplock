#include <stdio.h>
#include "servo_motor.h"
#include "photodiode.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    adc_read();
    // servo_init();
    // servo_rotate();
    // vTaskDelay(pdMS_TO_TICKS(1000));

    // servo_reset();
    // vTaskDelay(pdMS_TO_TICKS(10000));
    // servo_reset();

}