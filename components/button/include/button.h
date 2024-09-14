#ifndef BUTTON_H
#define BUTTON_H

/*
* Supports NP8S2R202GE model
*/


#include "stdbool.h"

void button_init(void);
bool button_is_pressed(void);
void button_clear_press(void);

#endif