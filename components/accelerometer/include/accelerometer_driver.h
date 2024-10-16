#ifndef ACCELEROMETER_DRIVER_H
#define ACCELEROMETER_DRIVER_H

#include "esp_err.h"
#include <stdbool.h>

#define MMA8451_ADDR 0x1D
#define MMA8451_WHO_AM_I 0x0D
#define MMA8451_CTRL_REG1 0x2A
#define MMA8451_CTRL_REG2 0x2B
#define MMA8451_OUT_X_MSB 0x01
#define MMA8451_OUT_Y_MSB 0x03
#define MMA8451_OUT_Z_MSB 0x05

#define TILT_THRESHOLD 45.0f
#define SENSITIVITY_2G 4096 // LSB/g for +- 2g range
#define IMPACT_THRESHOLD 3.0f  // g-forces

typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    float tilt_angle;
    bool is_dropped;
} accel_data_t;

esp_err_t accelerometer_init(void);
esp_err_t accelerometer_read_data(accel_data_t *data);

#endif 