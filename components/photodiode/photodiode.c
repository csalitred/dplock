#include "photodiode.h"
#include <stdio.h>
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static adc_oneshot_unit_handle_t adc1_handle;
static adc_cali_handle_t adc1_cali_handle = NULL;
static bool calibrated = false;

bool adc_calibration_init(adc_unit_t unit, adc_cali_handle_t *out_handle)
{
    esp_err_t ret = ESP_FAIL;

    #if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated) {
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = ADC_ATTEN_DB_12,
            .bitwidth = ADC_BITWIDTH_12
        };

        ret = adc_cali_create_scheme_curve_fitting(&cali_config, out_handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
    #endif

    return calibrated;
}

void adc_init(void)
{
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_12
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, PIN_PHOTODIODE, &config));

    calibrated = adc_calibration_init(ADC_UNIT_1, &adc1_cali_handle);
}

void adc_read(void)
{
    adc_init();

    while (1) {
        int adc_raw;
        int voltage = 0;
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, PIN_PHOTODIODE, &adc_raw));
        if (calibrated) {
            ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_cali_handle, adc_raw, &voltage));
        }
        printf("ADC Raw: %d, Voltage: %d mV\n", adc_raw, voltage);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}