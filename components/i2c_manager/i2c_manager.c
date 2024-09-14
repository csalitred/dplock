#include "board_pins.h"
#include "i2c_manager.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "stdlib.h"

#define PORT_NUMBER    -1  // auto-selecting
#define I2C_TIMEOUT_MS 1000
#define DATA_LENGTH     100  // arbitrary number for testing

static const char *TAG = "I2C_MANAGER";
static i2c_master_bus_handle_t bus_handle = NULL;
static i2c_master_dev_handle_t dev_handle = NULL;

void i2c_manager_init(void) 
{
    i2c_master_bus_config_t i2c_mst_config = {};
    i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_mst_config.i2c_port = I2C_NUM_0;
    i2c_mst_config.scl_io_num = PIN_I2C_SCL;
    i2c_mst_config.sda_io_num = PIN_I2C_SDA;
    i2c_mst_config.glitch_ignore_cnt = 7;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_7;
    dev_cfg.device_address = 0x58;
    dev_cfg.scl_speed_hz = 100000;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));
    ESP_ERROR_CHECK(i2c_master_probe(bus_handle, 0x22, -1));

    ESP_LOGI(TAG, "I2C manager initialized successfully");
}

/*
void i2c_manager_read(uint8_t device_address, uint8_t reg_address, uint8_t *data, size_t len)
{
    i2c_master_receive(dev_handle, data_rd, DATA_LENGTH, -1);
}

void i2c_manager_write(uint8_t device_address, uint8_t reg_address, const uint8_t *data, size_t len)
{
    ESP_ERROR_CHECK(12c_master_transmit(dev_handle, data_wr, DATA_LENTH, -1));
}

void i2c_manager_delete(void) {
    ESP_ERROR_CHECK(i2c_del_master_bus(bus_handle));
}
*/