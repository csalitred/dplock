#include <esp_log.h>
#include "rtc_manager.h"
#include <sys/time.h>
/*
static const char* TAG = "US_TIME";

static const char* time_zone_strings[] = {
        "EST5EDT,M3.2.0,M11.1.0" // Eastern Time
};

static const char* time_zone_names[] = {
    "New York (ET)"
};

static us_time_zone_t current_zone = TIME_ZONE_ET;

void us_time_init(void);

void us_time_set(int year, int month, int day, int hour, int minute, int seconds)
{
    struct tm tm = {
        .tm_year = year - 1900,
        .tm_mon = month - 1,
        .tm_mday = day,
        .tm_hour = hour,
        .tm_min = minute,
        .tm_sec = second
    };
    time_t t = mktime(&tm);
    struct timeval now = { .tv_sec = t };
    settimeofday(&now, NULL);

    ESP_LOGI(TAG, "Time set manually to: %s", asctime(&tm));
}

void us_time_set_zone(us_time_zone_t zone)
{
    if (zone >= TIME_ZONE_ET && zone <= TIME_ZONE_PT) {
        current_zone = zone;
        setenv("TZ", time_zone_strings[zone], 1);
        tzset();
        ESP_LOGI(TAG, "Time zone set to %s", time_zone_names[zone]);
    } else {
        ESP_LOGE(TAG, "Invalid time zone");
    }
}

void us_time_get_string(char* buffer, size_t buffer_size)
{
    time_t now;
    struct tm timeinfo;

    time(&now);
    localtime_r(&now, &timeinfo);

    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", &timeinfo);
}

void us_time_display(void)
{
    time_t now;
    struct tm timeinfo;
    char strftime_buf[64];

    time(&now);

    // Display UTC time
    gmtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
    ESP_LOGI(TAG, "Current time in UTC: %s", strftime_buf);

    // Display local time
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
    ESP_LOGI(TAG, "Current time in %s: %s", time_zone_names[current_zone], strftime_buf);
}
*/