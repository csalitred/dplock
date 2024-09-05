#include "board_pins.h"
#include "dplock_i2c.h"

#define I2C_PORT_NUM_0  I2C_NUM_0

i2c_master_bus_handle_t bus_handle;

void i2c_bus_init(void)
{
    i2c_master_bus_config_t i2c_mst_config = {};
    i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT; 
    i2c_mst_config.i2c_port = I2C_PORT_NUM_0;
    i2c_mst_config.scl_io_num = PIN_I2C_SCL;
    i2c_mst_config.sda_io_num = PIN_I2C_SDA;
    i2c_mst_config.glitch_ignore_cnt = 7;
    i2c_mst_config.flags.enable_internal_pullup = true;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));
}