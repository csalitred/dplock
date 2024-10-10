#include "board_pins.h"
#include "i2c_manager.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define I2C_MASTER_FREQ_HZ 50000 // You can adjust this as needed
#define I2C_TIMEOUT_MS     1000

static const char *TAG = "I2C_MANAGER";

static i2c_master_bus_handle_t bus_handle = NULL;
static i2c_master_dev_handle_t dev_handle = NULL;

esp_err_t i2c_master_init(void) 
{
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = PIN_I2C_SCL,
        .sda_io_num = PIN_I2C_SDA,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,  // Set to false when testing on PCB
    };
    esp_err_t ret = i2c_new_master_bus(&i2c_mst_config, &bus_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create I2C master bus");
        return ret;
    }
    ESP_LOGI(TAG, "I2C manager initialized successfully");
    return ESP_OK;
}

esp_err_t i2c_add_device(uint8_t slave_addr)
{
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = slave_addr,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };
    esp_err_t ret = i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add slave device with address 0x%02X", slave_addr);
        return ret;
    }
    ESP_LOGI(TAG, "Slave device added with address 0x%02X", slave_addr);
    return ESP_OK;
}

esp_err_t i2c_master_write_reg(uint8_t slave_addr, uint8_t reg_addr, uint8_t data)
{
    uint8_t write_buf[2] = {reg_addr, data};
    printf("Here");
    esp_err_t ret = i2c_master_transmit(dev_handle, write_buf, sizeof(write_buf), pdMS_TO_TICKS(I2C_TIMEOUT_MS));
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write to register 0x%02X on device 0x%02X: %s", reg_addr, slave_addr, esp_err_to_name(ret));
    }
    return ret;
}

esp_err_t i2c_master_read_reg(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, size_t len)
{
    esp_err_t ret =  i2c_master_transmit_receive(dev_handle, &reg_addr, 1, data, len, pdMS_TO_TICKS(I2C_TIMEOUT_MS));
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read %d registers starting from 0x%02X on device 0x%02X: %s", len, reg_addr, slave_addr, esp_err_to_name(ret));
    }
    return ret;
}

esp_err_t read_mma8451_id(uint8_t *device_id) {
    uint8_t reg_addr = 0x0D;  // WHO_AM_I register
    esp_err_t ret;

    ret = i2c_master_transmit_receive(dev_handle, &reg_addr, 1, device_id, 1, -1);
    if (ret == ESP_OK) {
        ESP_LOGE(TAG, "WHO_AM_I register value: 0x%02X", *device_id);
    } else {
        ESP_LOGE(TAG, "Failed to read WHO_AM_I register: %s", esp_err_to_name(ret));
    }
    return ret;
}

esp_err_t reset_mma8451(void) {
    uint8_t write_buf[2] = {0x2B, 0x40};  // CTRL_REG2, RST bit set
    esp_err_t ret;

    ret = i2c_master_transmit(dev_handle, write_buf, sizeof(write_buf), -1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to reset device: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Device reset command sent successfully");
        vTaskDelay(pdMS_TO_TICKS(10));  // Wait for reset to complete
    }
    return ret;
}

esp_err_t set_mma8451_standby(void) {
    uint8_t write_buf[2] = {0x2A, 0x01};  // CTRL_REG1, standby mode
    esp_err_t ret;

    ret = i2c_master_transmit(dev_handle, write_buf, sizeof(write_buf), -1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set device to standby mode: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Device set to standby mode successfully");
    }
    return ret;
}