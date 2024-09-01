#ifndef BUTTON_H
#define BUTTON_H

/*
* Supports NP8S2R202GE model
*/

#include "esp_err.h"
#include "driver/gpio.h"

typedef enum {
    BUTTON_OPEN = 0,
    BUTTON_CLOSED
} dplock_button_state_t;

void dplock_button_init(void);

#endif