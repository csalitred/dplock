#include "servo_driver.h"

esp_err_t servo_init(servo_driver_t* servo, gpio_num_t pin, ledc_channel_t channel, ledc_timer_t timer){
    servo->pin = pin;
    servo->channel = channel;
    servo->timer = timer;

    ledc_timer_config_t ledc_timer {};

    ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_timer.duty_resolution = SERVO_LEDC_TIMER_BIT;
    ledc_timer.timer_num = timer;
    ledc_timer.freq_hz = SERVO_LEDC_BASE_FREQ;
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;

    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {}
    ledc_channel.gpio_num = pin;
    ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_channel.channel = channel;
    ledc_channel.timer_sel = timer;
    ledc_channel.intr_type = LEDC_INTR_DISABLE;
    ledc_channel.duty = 0;
    ledc_channel.hpoint = 0;

    return ledc_channel_config(&ledc_channel);
}
esp_err_t servo_set_position(servo_driver_t* servo, uint32_t angle);
