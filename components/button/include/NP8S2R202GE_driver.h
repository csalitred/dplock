#ifndef NP8S2R202GE_DRIVER_H
#define NP8S2R202GE_DRIVER_H

/*
* NP8S2R202GE
*/
#include "stdbool.h"

typedef struct {
    bool is_door_opened;
    bool state_changed;
} button_input_t;

void button_init(void);
void button_read(button_input_t *data);

#endif