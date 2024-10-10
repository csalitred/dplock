#ifndef ACCELEROMETER_DRIVER_H
#define ACCELEROMETER_DRIVER_H

#include "esp_err.h"
#include <stdbool.h>

esp_err_t accelerometer_init(void);
esp_err_t accelerometer_read(int16_t *x, int16_t *y, int16_t *z);
esp_err_t accelerometer_detect_tilt(float *tilt_angle);
esp_err_t accelerometer_detect_drop(bool *drop_detected);

#endif // ACCELEROMETER_DRIVER_H