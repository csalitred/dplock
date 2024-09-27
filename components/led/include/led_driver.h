#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include <stdlib.h>
/* 
* Refer to datasheet for Forward Voltage and Forward Current
* https://www.digikey.com/en/products/detail/everlight-electronics-co-ltd/EASV3015RGBA0/6156087?s=N4IgTCBcDaIKIEEDKA1AzABgIwFYBKA4gEIIYgC6AvkA
* Resistors needed to add on to PCB (Assuming we are using 5V)
* RED: (5V - 2.0V) / 25 mA = 120 ohm
* GREEN: (5V - 3.3V) / 25 mA = 68 ohm
* BLUE: (5V - 3.3V) / 20 mA = 85 ohm
*/

void rgb_led_init(void);
void set_rgb_color(uint8_t red, uint8_t green, uint8_t blue);

#endif