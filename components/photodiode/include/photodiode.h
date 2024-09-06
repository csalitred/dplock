#ifndef PHOTODIODE_H
#define PHOTODIODE_H

#include "board_pins.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"

#define PIN_PHOTODIODE PIN_ADC

void adc_init(void);
bool adc_calibration_init(adc_unit_t unit, adc_cali_handle_t *out_handle);
void adc_read(void);

#endif 