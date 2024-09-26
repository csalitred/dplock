#include "nvs_operations.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

static const char* TAG = "NVS_OPS";

esp_err_t save_door_system_log_to_nvs(const door_system_log_t* log) {
    nvs_handle_t nvs_handle;
    esp_err_t err;

    err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return err;
    }

    err = nvs_set_blob(nvs_handle, "door_system_log", log, sizeof(door_system_log_t));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) writing to NVS!", esp_err_to_name(err));
    } else {
        err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) committing to NVS!", esp_err_to_name(err));
        } else {
            ESP_LOGI(TAG, "Door system log saved successfully!");
        }
    }

    nvs_close(nvs_handle);
    return err;
}

esp_err_t read_door_system_log_from_nvs(door_system_log_t* log) {
    nvs_handle_t nvs_handle;
    esp_err_t err;

    err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return err;
    }

    size_t required_size = sizeof(door_system_log_t);
    err = nvs_get_blob(nvs_handle, "door_system_log", log, &required_size);
    if (err != ESP_OK) {
        if (err == ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGW(TAG, "Door system log not found in NVS");
        } else {
            ESP_LOGE(TAG, "Error (%s) reading from NVS!", esp_err_to_name(err));
        }
    } else {
        ESP_LOGI(TAG, "Door system log read successfully!");
    }

    nvs_close(nvs_handle);
    return err;
}

esp_err_t add_event_to_log(door_system_log_t* log, door_event_type_t event_type) {
    if (log == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    door_event_t new_event = {
        .event_type = event_type,
        .timestamp = time(NULL)
    };

    // Add event to circular buffer
    log->events[log->next_event_index] = new_event;
    log->next_event_index = (log->next_event_index + 1) % 10;
    if (log->event_count < 10) {
        log->event_count++;
    }

    // Save updated log to NVS
    return save_door_system_log_to_nvs(log);
}

esp_err_t start_transit(door_system_log_t* log) {
    if (log == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!log->in_transit) {
        log->transit_start_time = time(NULL);
        log->in_transit = true;
        return save_door_system_log_to_nvs(log);
    }

    return ESP_OK;
}

esp_err_t end_transit(door_system_log_t* log) {
    if (log == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (log->in_transit) {
        time_t end_time = time(NULL);
        log->total_transit_time += (uint32_t)(end_time - log->transit_start_time);
        log->in_transit = false;
        return save_door_system_log_to_nvs(log);
    }

    return ESP_OK;
}

esp_err_t clear_door_system_log_in_nvs(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err;

    err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return err;
    }

    err = nvs_erase_key(nvs_handle, "door_system_log");
    if (err != ESP_OK) {
        if (err == ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGW(TAG, "Door system log not found in NVS");
        } else {
            ESP_LOGE(TAG, "Error (%s) erasing door system log from NVS!", esp_err_to_name(err));
        }
    } else {
        err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) committing to NVS!", esp_err_to_name(err));
        } else {
            ESP_LOGI(TAG, "Door system log erased successfully!");
        }
    }

    nvs_close(nvs_handle);
    return err;
}