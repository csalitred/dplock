#ifndef I2C_MANAGER_H
#define I2C_MANAGER_H

#include <stdint.h>
#include "esp_err.h"

esp_err_t i2c_master_init(void);
esp_err_t i2c_add_device(uint8_t slave_addr);
esp_err_t i2c_master_write_reg(uint8_t slave_addr, uint8_t reg_addr, uint8_t data);
esp_err_t i2c_master_read_reg(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, size_t len);
esp_err_t read_mma8451_id(uint8_t *device_id);
esp_err_t reset_mma8451(void);
esp_err_t set_mma8451_standby(void);

#endif