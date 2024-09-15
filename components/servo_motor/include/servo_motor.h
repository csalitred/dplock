#ifndef SERVO_DRIVER_H
#define SERVO_DRIVER_H

/*
 * Supports Servo Motor HS-5086WP (Testing)
 * Duty Cycle: 750 - 2250
 * Operating Voltage: 4.8V - 6V
 * PWM period: 20ms, Freq: 50Hz
 * 
 * Will use HS-5086WP Servo for Actual project
 * https://www.servocity.com/hs-5086wp-servo/?sku=35086S00
*/

#include "board_pins.h"

#define SERVO_MIN_PULSEWIDTH_US       500
#define SERVO_MID_PULSEWIDTH_US       750
#define SERVO_MAX_PULSEWIDTH_US       2250
#define SERVO_MIN_DEGREE              0.0f
#define SERVO_MAX_DEGREE              155.0f

#define SERVO_PULSE_GPIO              PIN_PWM 
#define SERVO_TIMEBASE_RESOLUTION_HZ  1000000   // 1MHz, 1us per tick
#define SERVO_TIMEBASE_PERIOD         20000  // ticks, 20ms

void servo_init(void);
void servo_rotate(void);
void servo_reset(void);

#endif