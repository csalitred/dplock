#ifndef DPLOCK_I2C_H
#define DPLOCK_I2C_H

#include "driver/i2c_master.h"

void i2c_bus_init(void);

extern i2c_master_bus_handle_t i2c_bus_handle;

#endif