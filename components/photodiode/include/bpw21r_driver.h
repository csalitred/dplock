#ifndef BPW21R_DRIVER_H
#define BPW21R_DRIVER_H

#include "esp_err.h"
#include "stdbool.h"
#include "esp_adc/adc_oneshot.h"

#define PHOTODIODE_ADC_CHANNEL ADC_CHANNEL_2
#define LIGHT_THRESHOLD 2000

esp_err_t photodiode_init(void);
esp_err_t photodiode_read(int* light_value);
bool is_light_detected(void);

#endif 