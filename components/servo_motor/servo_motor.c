#include "servo_motor.h"
#include "driver/mcpwm_prelude.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "SERVO_MOTOR";
static mcpwm_cmpr_handle_t comparator = NULL;

static inline uint32_t angle_to_compare(int angle) 
{
    if (angle < 0) {
        return SERVO_MID_PULSEWIDTH_US - ((SERVO_MID_PULSEWIDTH_US - SERVO_MIN_PULSEWIDTH_US) * abs(angle) / 90);
    } else {
        return SERVO_MID_PULSEWIDTH_US + ((SERVO_MAX_PULSEWIDTH_US - SERVO_MID_PULSEWIDTH_US) * angle / 90);
    }
}

void servo_init(void)
{
    ESP_LOGI(TAG, "Create timer and operator");
    mcpwm_timer_handle_t timer = NULL;
    mcpwm_timer_config_t timer_config = {};
    timer_config.group_id = 0;
    timer_config.clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT;
    timer_config.resolution_hz = SERVO_TIMEBASE_RESOLUTION_HZ;
    timer_config.period_ticks = SERVO_TIMEBASE_PERIOD;
    timer_config.count_mode = MCPWM_TIMER_COUNT_MODE_UP;
    ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer));

    mcpwm_oper_handle_t oper = NULL;
    mcpwm_operator_config_t operator_config = {};
    operator_config.group_id = 0;  // operator must be in the same group to the timer
    ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &oper));

    ESP_LOGI(TAG, "Connect timer and operator");
    ESP_ERROR_CHECK(mcpwm_operator_connect_timer(oper, timer));

    ESP_LOGI(TAG, "Create comparator and generator from the operator");
    mcpwm_comparator_config_t comparator_config = {};
    comparator_config.flags.update_cmp_on_tez = true;
    ESP_ERROR_CHECK(mcpwm_new_comparator(oper, &comparator_config, &comparator));

    mcpwm_gen_handle_t generator = NULL;
    mcpwm_generator_config_t generator_config = {};
    generator_config.gen_gpio_num = SERVO_PULSE_GPIO;
    ESP_ERROR_CHECK(mcpwm_new_generator(oper, &generator_config, &generator));

    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, angle_to_compare(0)));

    ESP_LOGI(TAG, "Set generator action on timer and compare event");
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(generator, 
                                                                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, 
                                                                                            MCPWM_TIMER_EVENT_EMPTY, 
                                                                                            MCPWM_GEN_ACTION_HIGH)));
    ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(generator, 
                                                                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP,
                                                                                                comparator, 
                                                                                                MCPWM_GEN_ACTION_LOW)));

    ESP_LOGI(TAG, "Enable and start timer");
    ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));
}

void servo_rotate(void)
{
    int angle = 0;
    int step = 90;

    while(1) {
        ESP_LOGI(TAG, "Angle of rotation: %d", angle);
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, angle_to_compare(angle)));
        vTaskDelay(pdMS_TO_TICKS(500));
        if ((angle + step) > 90 || (angle + step) < -90) {
            step *= -1;
        }
        angle += step;
    }
}


