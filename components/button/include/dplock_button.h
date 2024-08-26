#ifndef DPLOCK_BUTTON_H
#define DPLOCK_BUTTON_H

#include "dplock_pins.h"
#include "esp_err.h"
#include <stdbool.h>
// #include "esp_attr.h"

esp_err_t dplock_button_init(void);
void dplock_button_isr_handler(void *arg);  // removed  "IRAM_ATTR" issues with conflicts 
bool dplock_is_button_pressed(void);

#endif