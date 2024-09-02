#ifndef SERVO_DRIVER_H
#define SERVO_DRIVER_H

#include "driver/ledc.h"
#include "board_pins.h"

#define LEDC_TIMER          LEDC_TIMER_0
#define LEDC_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL        LEDC_CHANNEL_0
#define LEDC_OUTPUT_IO      PIN_PWM
#define LEDC_DUTY_RES       LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY           (4096)  // Set duty to 50% (2 ** 13) * 50% = 4096
#define LEDC_FREQUENCY      (5000)  // 5kHz

void servo_init(void);

#endif  // SERV_DRIVER_H