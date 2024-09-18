#include "board_pins.h"
#include "i2c_manager.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"

/*
* 0011101b (address for LISDS12) here for reference but will be utilized in components/lisds12 
* 
*/

#define DATA_LENGTH 100 

static const char *TAG = "I2C_MANAGER";
static i2c_master_bus_handle_t bus_handle = NULL;
static i2c_master_dev_handle_t dev_handle = NULL;

void i2c_master_init(void) 
{
    i2c_master_bus_config_t i2c_mst_config = {};
    i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_mst_config.i2c_port = I2C_NUM_0;
    i2c_mst_config.scl_io_num = PIN_I2C_SCL;
    i2c_mst_config.sda_io_num = PIN_I2C_SDA;
    i2c_mst_config.glitch_ignore_cnt = 7;
    i2c_mst_config.flags.enable_internal_pullup = true;  // false: when testing on PCB.
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));
    // ESP_ERROR_CHECK(i2c_master_probe(bus_handle, 0x22, -1));
    ESP_LOGI(TAG, "I2C manager initialized successfully");
}

void i2c_add_device(uint8_t slave_addr)
{
    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_7;
    dev_cfg.device_address = slave_addr;
    dev_cfg.scl_speed_hz = 100000;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));
    ESP_LOGI(TAG, "Slave device added with address 0x%02X", slave_addr);
}

void i2c_master_read_accel(int16_t *x, int16_t *y, int16_t *z, uint8_t reg_addr)
{
    uint8_t data[6];
    ESP_ERROR_CHECK(i2c_master_transmit_receive(dev_handle, &reg_addr, 1, data, 6, -1));
    *x = (int16_t)((data[1] << 8 | data[0]));  // MSB | LSB
    *y = (int16_t)((data[1] << 8 | data[0]));
    *z = (int16_t)((data[1] << 8 | data[0]));
}
