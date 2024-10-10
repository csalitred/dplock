#ifndef RTC_MANAGER_H
#define RTC_MANAGER_H

#include <time.h>
#include <stdbool.h>

typedef enum {
    TIME_ZONE_ET,
} us_time_zone_t;

void us_time_init(void);
void us_time_set(int year, int month, int day, int hour, int minute, int second);
void us_time_set_zone(us_time_zone_t zone);
void us_time_get_string(char* buffer, size_t buffer_size);
void us_time_display(void);

#endif