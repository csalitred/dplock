#ifndef SERVO_DRIVER_H
#define SERVO_DRIVER_H

/*
 * Supports Servo Motor MG90S 
 * Duty Cycle: 1-2ms
 * Operating Voltage: 4.8V - 6V
 * PWM period: 20ms, Freq: 50Hz
 * 1.5ms = Middle 0 degrees
 * ~2ms = All the way to the right 90 degrees
 * ~1ms = All the way to the left -90 degrees 
 * 
 * https://www.electronicoscaldas.com/datasheet/MG90S_Tower-Pro.pdf
*/

#include "board_pins.h"

#define SERVO_MIN_PULSEWIDTH_US       100 
#define SERVO_MAX_PULSEWIDTH_US       200
#define SERVO_MIN_DEGREE              -90
#define SERVO_MAX_DEGREE              90

#define SERVO_PULSE_GPIO              PIN_PWM 
#define SERVO_TIMEBASE_RESOLUTION_HZ  1000000   // 1MHz, 1us per tick
#define SERVO_TIMEBASE_PERIOD         20000  // ticks, 20ms

void servo_init(void);
void servo_rotate(void);

#endif