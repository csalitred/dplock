#ifndef PINS_H
#define PINS_H

#include "driver/gpio.h"
#include "soc/gpio_num.h"  // library with all the gpio pin numbers


// GPIOS
#define PIN_NUM_I2C_SDA GPIO_NUM_13
#define PIN_NUM_I2C_SCL GPIO_NUM_14
#define PIN_NUM_PWM     GPIO_NUM_1
#define PIN_NUM_LED     GPIO_NUM_10
#define PIN_NUM_DIODE   GPIO_NUM_2
#define PIN_NUM_BUTTON  GPIO_NUM_3

#endif