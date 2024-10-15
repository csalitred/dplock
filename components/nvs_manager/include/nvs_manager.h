#ifndef NVS_MANAGER_H
#define NVS_MANAGER_H

#include "esp_err.h"
#include <stddef.h> 

esp_err_t nvs_manager_init(void);
esp_err_t nvs_manager_set_int(const char* key, int32_t value);
esp_err_t nvs_manager_get_int(const char* key, int32_t* value);
esp_err_t nvs_manager_set_string(const char* key, const char* value);
esp_err_t nvs_manager_get_string(const char* key, char* value, size_t* length);
esp_err_t nvs_manager_erase_key(const char* key);
esp_err_t nvs_manager_erase_all(void);

#endif