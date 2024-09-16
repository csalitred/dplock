#include "bpw21r_driver.h"
#include "esp_log.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_timer.h"
#include "esp_task_wdt.h"

#define BPW21R_ADC_UNIT ADC_UNIT_1
#define BPW21R_ADC_CHANNEL ADC_CHANNEL_7  // GPIO 35 is ADC1_CHANNEL_7

static const char *TAG = "BPW21R_DRIVER";

struct bpw21r_handle_t {
    adc_oneshot_unit_handle_t adc_handle;
    adc_cali_handle_t cali_handle;
    bool is_calibrated;
};

esp_err_t bpw21r_init(bpw21r_handle_t **handle)
{
    *handle = calloc(1, sizeof(bpw21r_handle_t));
    if (*handle == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for handle");
        return ESP_ERR_NO_MEM;
    }

    esp_err_t ret;

    // Initialize ADC
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = BPW21R_ADC_UNIT,
    };
    ret = adc_oneshot_new_unit(&init_config, &((*handle)->adc_handle));
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize ADC");
        free(*handle);
        return ret;
    }

    // Configure ADC
    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,  // Full 0-3.3V range
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ret = adc_oneshot_config_channel((*handle)->adc_handle, BPW21R_ADC_CHANNEL, &config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure ADC channel");
        adc_oneshot_del_unit((*handle)->adc_handle);
        free(*handle);
        return ret;
    }

    // Initialize calibration
    adc_cali_handle_t cali_handle = NULL;
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = BPW21R_ADC_UNIT,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ret = adc_cali_create_scheme_curve_fitting(&cali_config, &cali_handle);
    if (ret == ESP_OK) {
        (*handle)->cali_handle = cali_handle;
        (*handle)->is_calibrated = true;
    } else {
        ESP_LOGW(TAG, "ADC calibration failed, using uncalibrated readings");
        (*handle)->is_calibrated = false;
    }

    return ESP_OK;
}

esp_err_t bpw21r_read_voltage(bpw21r_handle_t *handle, float *out_voltage)
{
    esp_err_t ret;
    int raw_value;

    // Read raw ADC value
    ret = adc_oneshot_read(handle->adc_handle, BPW21R_ADC_CHANNEL, &raw_value);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read raw value");
        return ret;
    }

    // Convert to voltage
    if (handle->is_calibrated) {
        int voltage_mv;
        ret = adc_cali_raw_to_voltage(handle->cali_handle, raw_value, &voltage_mv);
        if (ret == ESP_OK) {
            *out_voltage = voltage_mv / 1000.0f;  // Convert mV to V
        } else {
            ESP_LOGE(TAG, "Failed to convert raw to voltage");
            return ret;
        }
    } else {
        // If not calibrated, perform a simple conversion
        *out_voltage = (raw_value * 3.3f) / 4095.0f;  // Assuming 12-bit ADC and 3.3V reference
    }

    return ESP_OK;
}

esp_err_t bpw21r_deinit(bpw21r_handle_t *handle)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret;

    ret = adc_oneshot_del_unit(handle->adc_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to delete ADC unit");
    }

    if (handle->is_calibrated) {
        ret = adc_cali_delete_scheme_curve_fitting(handle->cali_handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to delete calibration scheme");
        }
    }

    free(handle);
    return ESP_OK;
}

void adc_read(void) 
{
        bpw21r_handle_t *photodiode_handle = NULL;
    esp_err_t ret;

    // Initialize the photodiode driver
    ret = bpw21r_init(&photodiode_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize BPW21R driver");
        return;
    }

    // Add the main task to the watchdog
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    // Main loop
    while (1) {
        float voltage;
        // Read voltage from the photodiode
        ret = bpw21r_read_voltage(photodiode_handle, &voltage);
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "Photodiode Voltage: %.3f V", voltage);
        } else {
            ESP_LOGE(TAG, "Failed to read from BPW21R");
        }

        // Reset the watchdog timer
        ESP_ERROR_CHECK(esp_task_wdt_reset());

        // Delay for 1 second, allowing other tasks to run
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // This part won't be reached in this example
    bpw21r_deinit(photodiode_handle);
}