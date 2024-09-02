#include <stdio.h>
#include "servo_driver.h"
#include "esp_log.h"

static const char *TAG = "servo_driver";

void app_main(void)
{
    servo_init();
    
    ESP_LOGI(TAG, "Setting duty cycle to 50%%");
    // Set duty to 50%
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LEDC_DUTY));
    // Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
    
    ESP_LOGI(TAG, "servo pwm completed successfully");
}