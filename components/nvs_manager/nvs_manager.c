#include "nvs_manager.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

static const char* TAG = "NVS_MANAGER";
static const char* NVS_NAMESPACE = "storage";

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
    return ret;
}

esp_err_t nvs_manager_set_int(const char* key, int32_t value)
{
    esp_err_t ret = nvs_set_i32(my_nvs_handle, key, value);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) setting value for key %s!", esp_err_to_name(ret), key);
    } else {
        ret = nvs_commit(my_nvs_handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) committing updates!", esp_err_to_name(ret));
        }
    }
    return ret;
}

esp_err_t nvs_manager_get_int(const char* key, int32_t* value)
{
    esp_err_t ret = nvs_get_i32(my_nvs_handle, key, value);
    if (ret != ESP_OK && ret != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "Error (%s) getting value for key %s!", esp_err_to_name(ret), key);
    }
    return ret;
}

esp_err_t nvs_manager_set_string(const char* key, const char* value)
{
    esp_err_t ret = nvs_set_str(my_nvs_handle, key, value);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) setting string for key %s!", esp_err_to_name(ret), key);
    } else {
        ret = nvs_commit(my_nvs_handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) committing updates!", esp_err_to_name(ret));
        }
    }
    return ret;
}

esp_err_t nvs_manager_get_string(const char* key, char* value, size_t* length)
{
    esp_err_t ret = nvs_get_str(my_nvs_handle, key, value, length);
    if (ret != ESP_OK && ret != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "Error (%s) getting string for key %s!", esp_err_to_name(ret), key);
    }
    return ret;
}

esp_err_t nvs_manager_erase_key(const char* key)
{
    esp_err_t ret = nvs_erase_key(my_nvs_handle, key);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) erasing key %s!", esp_err_to_name(ret), key);
    } else {
        ret = nvs_commit(my_nvs_handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) committing updates!", esp_err_to_name(ret));
        }
    }
    return ret;
}

esp_err_t nvs_manager_erase_all(void)
{
    esp_err_t ret = nvs_erase_all(my_nvs_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) erasing all keys!", esp_err_to_name(ret));
    } else {
        ret = nvs_commit(my_nvs_handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Error (%s) committing updates!", esp_err_to_name(ret));
        }
    }
    return ret;
}