#ifndef PINS_H
#define PINS_H

#include "driver/gpio.h"   // library with gpio funcs
#include "soc/gpio_num.h"  // library with all the gpio pin numbers


/**************************************
*   Accelerometer / RTC 
***************************************/
#define PIN_NUM_I2C_SDA     GPIO_NUM_47
#define PIN_NUM_I2C_SCL     GPIO_NUM_46
#define PIN_NUM_I2C_SDO     GPIO_NUM_38
#define PIN_NUM_RST         GPIO_NUM_13
#define PIN_NUM_INT1        GPIO_NUM_9


/***************************************
*   Photodiode
***************************************/
#define PIN_NUM_OPAMP       GPIO_NUM_35


/***************************************
*   USB
****************************************/
#define PIN_NUM_USB_DPOS    GPIO_NUM_11
#define PIN_NUM_USB_DNEG    GPIO_NUM_12


/***************************************
*   DC Motor
****************************************/
#define PIN_NUM_PWM         GPIO_NUM_10


/***************************************
*   Button   
****************************************/
#define PIN_NUM_BUTTON      GPIO_NUM_12


/****************************************
*   LED - Need to connect to GPIO, As of now it's being fed 3v3 from mcu.
*****************************************/
// #define PIN_NUM_RGB         



#endif

