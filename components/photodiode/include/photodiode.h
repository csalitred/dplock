#ifndef PHOTODIODE_H
#define PHOTODIODE_H

#include "board_pins.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "stdbool.h"

#define PHOTODIODE_PIN ADC_CHANNEL_7
// BPW21R specific configurations
#define ADC_ATTEN           ADC_ATTEN_DB_12  // 11dB attenuation (0-2.6V)
#define ADC_BIT_WIDTH       ADC_BITWIDTH_12  // 12-bit resolution
#define SAMPLE_COUNT        64  

bool adc_calibration_init(adc_unit_t unit, adc_cali_handle_t *out_handle);
void adc_read(void);

#endif 