#include "bpw21r_driver.h"
#include "esp_log.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

static const char *TAG = "BPW21R_DRIVER";

static adc_oneshot_unit_handle_t adc_handle;
static bool do_calibration = false;
static adc_cali_handle_t adc_cali_handle = NULL;

static bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

    *out_handle = handle;
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else {
        ESP_LOGE(TAG, "Invalid arg or no memory");
    }

    return calibrated;
}

static void adc_calibration_deinit(adc_cali_handle_t handle)
{
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Curve Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(handle));

#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "deregister %s calibration scheme", "Line Fitting");
    ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));
#endif
}

esp_err_t photodiode_init(void)
{
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, PHOTODIODE_ADC_CHANNEL, &config));

    do_calibration = adc_calibration_init(ADC_UNIT_1, PHOTODIODE_ADC_CHANNEL, ADC_ATTEN_DB_12, &adc_cali_handle);

    ESP_LOGI(TAG, "BPW21R photodiode initialized successfully");
    return ESP_OK;
}

esp_err_t photodiode_read(int *light_value)
{
    if (light_value == NULL) {
        ESP_LOGE(TAG, "Invalid pointer for light_value");
        return ESP_ERR_INVALID_ARG;
    }

    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, PHOTODIODE_ADC_CHANNEL, light_value));
    ESP_LOGI(TAG, "ADC Raw Data: %d", *light_value);

    if (do_calibration) {
        int voltage;
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc_cali_handle, *light_value, &voltage));
        ESP_LOGI(TAG, "Calibrated Voltage: %d mV", voltage);
    }

    return ESP_OK;
}

bool is_light_detected(void)
{
    int light_value;
    if (photodiode_read(&light_value) == ESP_OK) {
        return light_value > LIGHT_THRESHOLD;
    }
    ESP_LOGE(TAG, "Failed to read photodiode");
    return false;
}

void photodiode_deinit(void)
{
    ESP_ERROR_CHECK(adc_oneshot_del_unit(adc_handle));
    if (do_calibration) {
        adc_calibration_deinit(adc_cali_handle);
    }
}