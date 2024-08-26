#ifndef DPLOCK_BUTTON_H
#define DPLOCK_BUTTON_H

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

esp_err_t dplock_button_init(void);
esp_err_t dplock_button_start_task(void);
bool dplock_button_is_pressed(void);

#endif