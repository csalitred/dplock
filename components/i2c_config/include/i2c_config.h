#ifndef I2C_CONFIG_H
#define I2C_CONFIG_H

#include "driver/i2c_master.h"

void i2c_bus_init(void);

extern i2c_master_bus_handle_t i2c_bus_handle;

#endif