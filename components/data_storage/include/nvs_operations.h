#ifndef NVS_OPERATIONS_H
#define NVS_OPERATIONS_H

#include "esp_err.h"
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

typedef enum {
    EVENT_BREACH,
    EVENT_CRASH_DROP,
    EVENT_AUTHORIZED_ACCESS
} door_event_type_t;

typedef struct {
    door_event_type_t event_type;
    time_t timestamp;
} door_event_t;

typedef struct {
    door_event_t events[10];
    uint8_t event_count;
    uint8_t next_event_index;
    time_t transit_start_time;
    uint32_t total_transit_time;
    bool in_transit;
} door_system_log_t;

esp_err_t save_door_system_log_to_nvs(const door_system_log_t* log);
esp_err_t read_door_system_log_from_nvs(door_system_log_t* log);
esp_err_t add_event_to_log(door_system_log_t* log, door_event_type_t event_type);
esp_err_t start_transit(door_system_log_t* log);
esp_err_t end_transit(door_system_log_t* log);
esp_err_t clear_door_system_log_in_nvs(void);

#endif