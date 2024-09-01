#ifndef I2C_H
#define I2C_H

#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"

void i2c_bus_init();

extern i2c_master_bus_handle_t i2c_bus_handle;

extern SemaphoreHandle_t i2c_semaphore;

#endif