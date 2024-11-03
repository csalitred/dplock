#ifndef NVS_MANAGER_H
#define NVS_MANAGER_H

#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define MAX_STRING_LENGTH 64
#define MAX_EVENTS 50

typedef enum {
    EVENT_BREACH,
    EVENT_AUTHORIZED_ACCESS,
    EVENT_POTENTIAL_DROP,
    EVENT_TRIP_START
} event_type_t;

typedef struct {
    char origin[MAX_STRING_LENGTH];
    char destination[MAX_STRING_LENGTH];
    char contents[MAX_STRING_LENGTH];
    char container_id[MAX_STRING_LENGTH];
    uint32_t start_tick;
} trip_info_t;

typedef struct {
    event_type_t type;
    uint32_t tick_count;
} event_record_t;

esp_err_t nvs_manager_init(void);
esp_err_t nvs_manager_start_trip(const trip_info_t* trip_info);
esp_err_t nvs_manager_get_trip_info(trip_info_t* trip_info);
esp_err_t nvs_manager_log_event(event_type_t type);
esp_err_t nvs_manager_get_all_events(event_record_t* events, uint32_t* count);
esp_err_t nvs_manager_erase_all(void);

#endif // NVS_MANAGER_H