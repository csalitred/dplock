#ifndef I2C_MANAGER_H
#define I2C_MANAGER_H

#include "stdlib.h"

void i2c_master_init(void);
void i2c_master_read_accel(int16_t *x, int16_t *y, int16_t *z, uint8_t reg_addr);
void i2c_add_device(uint8_t slave_address);


#endif