#ifndef HS5086WP_H
#define HS5086WP_H

/*
 * Supports Servo Motor HS-5086WP
 * Pulse Width: 750 - 2250
 * Operating Voltage: 4.8V - 6V
 * PWM period: 20ms, Freq: 50Hz
 * 
 * https://www.servocity.com/hs-5086wp-servo/?sku=35086S00
*/

#include "board_pins.h"

#define SERVO_MIN_PULSEWIDTH_US       750
#define SERVO_MAX_PULSEWIDTH_US       2250
#define SERVO_MIN_DEGREE              0
#define SERVO_MAX_DEGREE              155

#define SERVO_PULSE_GPIO              PIN_PWM 
#define SERVO_TIMEBASE_RESOLUTION_HZ  1000000   // 1MHz, 1us per tick
#define SERVO_TIMEBASE_PERIOD         20000  // ticks, 20ms

void servo_init(void);
void servo_rotate(void);
void servo_reset(void);
void servo_unlock(void);

#endif