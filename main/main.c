#include <stdlib.h>
#include "stdio.h"
// #include "bpw21r_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #include "i2c_manager.h"
#include "hs5086wp.h"
#include "NP8S2R202GE_driver.h"

typedef enum {IDLE, AUTHORIZED_ACCESS, DROP, BREACH, MAX_STATES} state_t;

typedef struct {
    bool is_button_released;
    bool is_light_detected;
    bool is_ble_connection;
    bool is_dropped;
    int led_position;
} inputs_t;

state_t run_idle_state(inputs_t*);
state_t run_authorized_access_state(inputs_t*);
state_t run_dropped_state(inputs_t*);
state_t run_breached_state(inputs_t*);

state_t (*state_table[MAX_STATES])(inputs_t*) = {run_idle_state, run_authorized_access_state, run_dropped_state, run_breached_state};

inputs_t inputs;

void init(void);

void app_main(void)
{
    while(1) {

    }
}

void init(void)
{
    button_init();
    servo_init();
}

state_t run_idle_state(inputs_t* inputs)
{
    state_t next_state = IDLE;
    /*
    *   enter low power mode
    *   wait for interrupts from peripherals to wake up cpu
    *   
    *   if (is_ble_connection) enter run_authorized_access_state
    *   if (is_dropped) enter run_drop_state
    *       
    *   
    */   
   return next_state;
}

state_t run_authorized_access_state(inputs_t* inputs)
{
    state_t next_state = IDLE;
    /*
    *   allow for data transfer 
    *   allow for peripheral control via ble
    *   if (!is_ble_connection) go to run_idle_state
    */
   return next_state;
}

state_t run_dropped_state(inputs_t* inputs)
{
    state_t next_state = IDLE;
    /*
    *   log accelerometer data and time stamp
    *   state_t next_state = run_idle_state
    */
    return next_state;
}

state_t run_breached_state(inputs_t* inputs)
{
    state_t next_state = IDLE;
    /*
    *   stay in loop forever 
    *   log time stamp
    */
    return next_state;
}
