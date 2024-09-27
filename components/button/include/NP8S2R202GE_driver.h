#ifndef NP8S2R202GE_DRIVER_H
#define NP8S2R202GE_DRIVER_H

/*
* NP8S2R202GE
*/
#include "stdbool.h"

void button_init(void);
bool is_button_released(void);
bool get_button_state(void);

#endif