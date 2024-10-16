#include "accelerometer_driver.h"
#include "i2c_manager.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>

static const char *TAG = "ACCELEROMETER_DRIVER";

esp_err_t accelerometer_init(void)
{
    esp_err_t ret;

    // Initialize I2C
    ret = i2c_master_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize I2C: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "I2C initialized successfully");

    // Add MMA8451 device to I2C bus
    ret = i2c_add_device(MMA8451_ADDR);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to add MMA8451 device");
        return ret;
    }

    // Read WHO_AM_I register
    uint8_t device_id;
    ret = read_mma8451_id(&device_id);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read MMA8451 device ID");
        return ret;
    }
    if (device_id != 0x1A) {
        ESP_LOGE(TAG, "Incorrect device ID: expected 0x1A, got 0x%02X", device_id);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "MMA8451 device ID verified successfully: 0x%02X", device_id);

    // Reset the device
    ret = reset_mma8451();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to reset MMA8451");
        return ret;
    }

    // Set to standby mode
    ret = set_mma8451_standby();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set MMA8451 to standby mode");
        return ret;
    }

    // Configure the device (example: 2g range, 800 Hz ODR)
    uint8_t config = 0x01;  // 800 Hz ODR, normal mode
    ret = i2c_master_write_reg(MMA8451_ADDR, MMA8451_CTRL_REG1, config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure MMA8451");
        return ret;
    }

    // Set to active mode
    config |= 0x01;  // Set ACTIVE bit
    ret = i2c_master_write_reg(MMA8451_ADDR, MMA8451_CTRL_REG1, config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set MMA8451 to active mode");
        return ret;
    }

    ESP_LOGI(TAG, "MMA8451 initialized successfully");
    return ESP_OK;
}

esp_err_t accelerometer_read_data(accel_data_t *data)
{
    uint8_t raw_data[6];
    esp_err_t ret = i2c_master_read_reg(MMA8451_ADDR, MMA8451_OUT_X_MSB, raw_data, 6);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read accelerometer data");
        return ret;
    }

    data->accel_x = (int16_t)((raw_data[0] << 8) | raw_data[1]) >> 2;
    data->accel_y = (int16_t)((raw_data[2] << 8) | raw_data[3]) >> 2;
    data->accel_z = (int16_t)((raw_data[4] << 8) | raw_data[5]) >> 2;

    float x_g = (float)data->accel_x / SENSITIVITY_2G;
    float y_g = (float)data->accel_y / SENSITIVITY_2G;
    float z_g = (float)data->accel_z / SENSITIVITY_2G;

    float total_accel_g = sqrt(x_g*x_g + y_g*y_g + z_g*z_g);
    data->is_dropped = (total_accel_g >= IMPACT_THRESHOLD);
    data->tilt_angle = atan2(sqrt(data->accel_x*data->accel_x + data->accel_y*data->accel_y), data->accel_z) * 180.0 / M_PI;
    
    // TODO: DROP DETECTION FIX!!!
    if (data->is_dropped) {
        ESP_LOGI(TAG, "Impact detected! Total acceleration: %.2f g", total_accel_g);
    } 

    return ESP_OK;
}
