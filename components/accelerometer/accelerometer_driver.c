#include "accelerometer_driver.h"
#include "i2c_manager.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>

#define MMA8451_ADDR 0x1D
#define MMA8451_WHO_AM_I 0x0D
#define MMA8451_CTRL_REG1 0x2A
#define MMA8451_CTRL_REG2 0x2B
#define MMA8451_OUT_X_MSB 0x01
#define MMA8451_OUT_Y_MSB 0x03
#define MMA8451_OUT_Z_MSB 0x05

static const char *TAG = "ACCELEROMETER_DRIVER";

esp_err_t accelerometer_init(void)
{
    esp_err_t ret;

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

esp_err_t accelerometer_read(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t data[6];

    esp_err_t ret = i2c_master_read_reg(MMA8451_ADDR, MMA8451_OUT_X_MSB, data, 6);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read accelerometer data");
        return ret;
    }

    *x = (int16_t)((data[0] << 8) | data[1]) >> 2;
    *y = (int16_t)((data[2] << 8) | data[3]) >> 2;
    *z = (int16_t)((data[4] << 8) | data[5]) >> 2;

    return ESP_OK;
}

esp_err_t accelerometer_detect_tilt(float *tilt_angle)
{
    int16_t x, y, z;
    esp_err_t ret = accelerometer_read(&x, &y, &z);
    if (ret != ESP_OK) {
        return ret;
    }

    // Calculate tilt angle
    *tilt_angle = atan2(sqrt(x*x + y*y), z) * 180.0 / M_PI;

    return ESP_OK;
}

esp_err_t accelerometer_detect_drop(bool *drop_detected)
{
    int16_t x, y, z;
    esp_err_t ret = accelerometer_read(&x, &y, &z);
    if (ret != ESP_OK) {
        return ret;
    }

    // Calculate total acceleration
    float total_accel = sqrt(x*x + y*y + z*z);

    // Detect drop if total acceleration is close to 0 (free fall)
    *drop_detected = (total_accel < 100); // Adjust threshold as needed

    return ESP_OK;
}