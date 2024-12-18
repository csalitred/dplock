#ifndef BOARD_PINS_H
#define BOARD_PINS_H

#include "soc/gpio_num.h"  

/********************************
* Accelerometer / RTC (Shared I2C)
*********************************/
#define PIN_I2C_SDA   GPIO_NUM_47    
#define PIN_I2C_SCL   GPIO_NUM_46  
#define PIN_I2C_SA0   GPIO_NUM_38  // Address Config
#define PIN_I2C_INT1  GPIO_NUM_9   // Interrupt Line

/*********************************
* Photodiode
**********************************/
#define PIN_ADC       GPIO_NUM_8

/*********************************
* USB
**********************************/
#define PIN_USB_DPOS  GPIO_NUM_11
#define PIN_USB_DNEG  GPIO_NUM_12

/**********************************
*  DC Motor
***********************************/
#define PIN_PWM       GPIO_NUM_10

/**********************************
* Button   
***********************************/
#define PIN_BUTTON    GPIO_NUM_13

/**********************************
* LED RGB 
***********************************/
#define PIN_LED_RED   GPIO_NUM_2        
#define PIN_LED_GREEN GPIO_NUM_4
#define PIN_LED_BLUE  GPIO_NUM_5
#endif