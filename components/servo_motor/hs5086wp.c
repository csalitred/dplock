#include "hs5086wp.h"
#include "driver/mcpwm_prelude.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

static const char *TAG = "SERVO_MOTOR";
static mcpwm_cmpr_handle_t comparator = NULL;
static uint8_t motor_shift = 90;

static inline uint32_t angle_to_compare(int angle) 
{
    return (angle - SERVO_MIN_DEGREE) * (SERVO_MAX_PULSEWIDTH_US - SERVO_MIN_PULSEWIDTH_US) / (SERVO_MAX_DEGREE - SERVO_MIN_DEGREE) + SERVO_MIN_PULSEWIDTH_US;
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

    ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, angle_to_compare(SERVO_MAX_DEGREE)));

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
    int step = 5;
    int angle = SERVO_MAX_DEGREE;
    if (angle > SERVO_MAX_DEGREE || angle < SERVO_MIN_DEGREE) {
       // ESP_LOGE(TAG, "Invalid angle: %d. Angle should be between %d and %d degrees.", angle, SERVO_MIN_DEGREE, SERVO_MAX_DEGREE); 
        return;
    }
    while (angle >= 50) {
        ESP_LOGI(TAG, "Rotating servo to angle: %d degress", angle);
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, angle_to_compare(angle)));
        vTaskDelay(pdMS_TO_TICKS(20));
        angle -= step;
    }
}

void servo_reset(void)
{
    int step = 5;
    int angle = 50;
    while (angle <= SERVO_MAX_DEGREE) {
        if (angle > SERVO_MAX_DEGREE || angle < SERVO_MIN_DEGREE) {
       // ESP_LOGE(TAG, "Invalid angle: %d. Angle should be between %d and %d degrees.", angle, SERVO_MIN_DEGREE, SERVO_MAX_DEGREE); 
        return;
        }
        ESP_LOGI(TAG, "Rotating servo to angle: %d degress", angle);
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, angle_to_compare(angle)));
        vTaskDelay(pdMS_TO_TICKS(20));
        angle += step;
    }
}

void servo_unlock(void)
{
    servo_rotate();
    vTaskDelay(pdMS_TO_TICKS(5000));
    servo_reset();
}


