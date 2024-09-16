#include <stdio.h>
#include "servo_motor.h"
#include "bpw21r_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{

    // adc_read();
    servo_init();
    // servo_rotate();

    // servo_reset();
    // vTaskDelay(pdMS_TO_TICKS(10000));
    // servo_reset();

}