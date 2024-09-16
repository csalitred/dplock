#ifndef BPW21R_DRIVER_H
#define BPW21R_DRIVER_H

#include "esp_err.h"

typedef struct bpw21r_handle_t bpw21r_handle_t;

esp_err_t bpw21r_init(bpw21r_handle_t **handle);
esp_err_t bpw21r_read_voltage(bpw21r_handle_t *handle, float *out_voltage);
esp_err_t bpw21r_deinit(bpw21r_handle_t *handle);
void adc_read(void);
#endif 