#ifndef SERVO_DRIVER_H
#define SERVO_DRIVER_H

#include "driver/ledc.h"

#define SERVO_LEDC_TIMER_BIT  14
#define SERVO_LEDC_BASE_FREQ  50

typedef struct {
    gpio_num_t pin;
    ledc_channel_t channel;
    ledc_timer_t timer;
} servo_driver_t;

esp_err_t servo_init(servo_driver_t* servo, gpio_num_t pin, ledc_channel_t channel, ledc_timer_t timer);
esp_err_t servo_set_position(servo_driver_t* servo, uint32_t angle);

#endif