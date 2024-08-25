#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include "esp_err.h"
#include <time.h>

typedef struct {
    float accelerometer_x;
    float accelerometer_y;
    float accelerometer_z;
    time_t timestamp;
    float photodiode_value;
} sensor_data_t;

esp_err_t data_storage_inti(void);
esp_err_t data_storage_save_record(const sensor_data_t* data);
esp_err_t data_storage_get_all_records(sensor_data_t** data, size_t* const);
esp_err_t data_storage_clear_all(void);


#endif