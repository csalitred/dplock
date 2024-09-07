#ifndef I2C_MANAGER_H
#define I2C_MANAGER_H

#include "esp_err.h"

esp_err_t i2c_manager_init(void);
esp_err_t i2c_manager_read(uint8_t device_address, uint8_t reg_address, uint8_t *data, size_t len);
esp_err_t i2c_manager_write(uint8_t device_address, uint8_t reg_address, const uint8_t *data, size_t len);

#endif