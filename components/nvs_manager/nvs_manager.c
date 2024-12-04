#include "nvs_manager.h"
#include <time.h>

static const char* TAG = "NVS_MANAGER";
static const char* NVS_NAMESPACE = "storage";
#define TRIP_INFO_KEY "trip_info"
#define EVENT_KEY_PREFIX "event_"
#define EVENT_COUNT_KEY "event_count"

static nvs_handle_t my_nvs_handle;

esp_err_t nvs_manager_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ret = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &my_nvs_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(ret));
    }

    init_rtc();

    return ret;
}

esp_err_t nvs_manager_start_trip(const trip_info_t* trip_info)
{
    if (trip_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    trip_info_t info = *trip_info;
    info.start_tick = xTaskGetTickCount() * portTICK_PERIOD_MS;

    esp_err_t ret = nvs_set_blob(my_nvs_handle, TRIP_INFO_KEY, &info, sizeof(trip_info_t));
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to store trip info: %s", esp_err_to_name(ret));
        return ret;
    }

    // Reset event count
    ret = nvs_set_i32(my_nvs_handle, EVENT_COUNT_KEY, 0);
    if (ret != ESP_OK) {
        return ret;
    }

    // Log trip start event
    ret = nvs_manager_log_event(EVENT_TRIP_START);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(TAG, "Trip started - Origin: %s, Destination: %s, Container: %s",
             info.origin, info.destination, info.container_id);

    return nvs_commit(my_nvs_handle);
}

esp_err_t nvs_manager_get_trip_info(trip_info_t* trip_info)
{
    if (trip_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    size_t required_size = sizeof(trip_info_t);
    return nvs_get_blob(my_nvs_handle, TRIP_INFO_KEY, trip_info, &required_size);
}

esp_err_t nvs_manager_log_event(event_type_t type)
{
    uint32_t count = 0;
    esp_err_t ret = nvs_get_i32(my_nvs_handle, EVENT_COUNT_KEY, (int32_t*)&count);
    if (ret != ESP_OK && ret != ESP_ERR_NVS_NOT_FOUND) {
        return ret;
    }
    
    if (count >= MAX_EVENTS) {
        ESP_LOGW(TAG, "Event storage full, rolling over");
        count = 0;
    }
    
    event_record_t event;
    event.type = type;
    event.tick_count = xTaskGetTickCount() * portTICK_PERIOD_MS;
    gettimeofday(&event.timestamp, NULL);
    
    char key[16];
    snprintf(key, sizeof(key), EVENT_KEY_PREFIX "%lu", (unsigned long)count);
    
    ret = nvs_set_blob(my_nvs_handle, key, &event, sizeof(event_record_t));
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to store event: %s", esp_err_to_name(ret));
        return ret;
    }
    
    count++;
    ret = nvs_set_i32(my_nvs_handle, EVENT_COUNT_KEY, count);
    if (ret != ESP_OK) {
        return ret;
    }

    struct tm timeinfo;
    time_t now = event.timestamp.tv_sec;
    localtime_r(&now, &timeinfo);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &timeinfo);

    const char* event_type_str;
    switch(type) {
        case EVENT_BREACH: event_type_str = "BREACH"; break;
        case EVENT_AUTHORIZED_ACCESS: event_type_str = "AUTH_ACCESS"; break;
        case EVENT_POTENTIAL_DROP: event_type_str = "POT_DROP"; break;
        case EVENT_TRIP_START: event_type_str = "TRIP_START"; break;
        default: event_type_str = "UNKNOWN"; break;
    }

    
    ESP_LOGI(TAG, "Event logged - Type: %s, Time: %s", event_type_str, time_str);
    
    return nvs_commit(my_nvs_handle);
}

esp_err_t nvs_manager_get_all_events(event_record_t* events, uint32_t* count)
{
    if (events == NULL || count == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    int32_t stored_count = 0;
    esp_err_t ret = nvs_get_i32(my_nvs_handle, EVENT_COUNT_KEY, &stored_count);
    if (ret != ESP_OK && ret != ESP_ERR_NVS_NOT_FOUND) {
        return ret;
    }
    
    *count = (uint32_t)stored_count;
    
    for (uint32_t i = 0; i < *count; i++) {
        char key[16];
        snprintf(key, sizeof(key), EVENT_KEY_PREFIX "%lu", (unsigned long)i);
        size_t required_size = sizeof(event_record_t);
        ret = nvs_get_blob(my_nvs_handle, key, &events[i], &required_size);
        if (ret != ESP_OK) {
            return ret;
        }
    }
    
    return ESP_OK;
}

esp_err_t nvs_manager_erase_all(void)
{
    esp_err_t ret = nvs_erase_all(my_nvs_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) erasing NVS!", esp_err_to_name(ret));
        return ret;
    }
    return nvs_commit(my_nvs_handle);
}

void init_rtc(void)
{
    struct tm time = {
        .tm_year = 2024 - 1900,
        .tm_mon = 10,
        .tm_mday = 12,
        .tm_hour = 12,
        .tm_min = 0,
        .tm_sec = 0
    };
    struct timeval tv = {
        .tv_sec = mktime(&time),
        .tv_usec = 0
    };
    settimeofday(&tv, NULL);

    ESP_LOGI(TAG, "RTC initialized with build time");
}