#include "photodiode.h"
#include <stdio.h>
#include "esp_adc/adc_cali_scheme.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "math.h"

static const char *TAG = "BPW21R_ADC";

#define MOVING_AVERAGE_SIZE 10
#define ALPHA 0.1 // Low-pass filter coefficient

static int moving_average_buffer[MOVING_AVERAGE_SIZE];
static int moving_average_index = 0;
static float filtered_voltage = 0;

static int calculate_moving_average(int new_value) {
    static int sum = 0;
    sum -= moving_average_buffer[moving_average_index];
    sum += new_value;
    moving_average_buffer[moving_average_index] = new_value;
    moving_average_index = (moving_average_index + 1) % MOVING_AVERAGE_SIZE;
    return sum / MOVING_AVERAGE_SIZE;
}

static float apply_low_pass_filter(float new_value, float prev_value) {
    return ALPHA * new_value + (1 - ALPHA) * prev_value;
}

void adc_read(void)
{
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BIT_WIDTH,
        .atten = ADC_ATTEN,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, PHOTODIODE_PIN, &config));

    adc_cali_handle_t adc1_cali_handle = NULL;
    bool do_calibration = adc_calibration_init(ADC_UNIT_1, &adc1_cali_handle);

    while (1) {
        int adc_raw;
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, PHOTODIODE_PIN, &adc_raw));

        int voltage = 0;
        if (do_calibration) {
            ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_cali_handle, adc_raw, &voltage));
        } else {
            voltage = adc_raw; // Use raw value if calibration failed
        }

        int avg_voltage = calculate_moving_average(voltage);
        filtered_voltage = apply_low_pass_filter(avg_voltage, filtered_voltage);

        // Non-linear mapping for light intensity
        float light_intensity = 100.0 * (1.0 - exp(-filtered_voltage / 1000.0));
        
        ESP_LOGI(TAG, "ADC Raw: %d, Voltage: %d mV, Filtered: %.2f mV, Light: %.2f%%", 
                 adc_raw, voltage, filtered_voltage, light_intensity);

        vTaskDelay(pdMS_TO_TICKS(100)); // 10ms delay for faster response
    }
}

bool adc_calibration_init(adc_unit_t unit, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

    #if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = ADC_ATTEN,
            .bitwidth = ADC_BIT_WIDTH,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
    #endif

    *out_handle = handle;
    if (calibrated) {
        ESP_LOGI(TAG, "Calibration Success");
    } else {
        ESP_LOGI(TAG, "Calibration Failed");
    }
    return calibrated;
}