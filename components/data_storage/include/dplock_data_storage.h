#ifndef DPLOCK_DATA_STORAGE_H
#define DPLOCK_DATA_STORAGE_H

#include "esp_err.h"
#include <time.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_AUTH_DEVICES 10  // BLE devices
#define MAX_DATA_POINTS  100

typedef uint8_t bd_addr_t[6];  // 6 bytes for BD_ADDR

typedef struct {
    float x;
    float y;
    float z;
    uint64_t timestamp;  // Unix time in milliseconds
} dplock_accel_data_point_t;

typedef struct {
    uint64_t transit_time;  // elapsed time in milliseconds
    uint64_t timestamp;
} dplock_rtc_data_t;

typedef struct {
    int64_t event_timestamp;
    bool motor_actuated;
    bool photodiode_detected_light;
    bool button_pressed;
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
} dplock_breach_event_t;


/* 
* Initialize the data storage system
*/
esp_err_t dplock_data_storage_init(void);

/*
* Store and retrieve accelerometer data
*/ 
esp_err_t dplock_store_accel_data(const dplock_accel_data_point_t *data);
esp_err_t dplock_get_accel_data(dplock_accel_data_point_t *data);

/*
* Store and retrieve RTC data
*/ 
esp_err_t dplock_store_rtc_data(const dplock_rtc_data_t *data);
esp_err_t dplock_get_rtc_data(dplock_rtc_data_t *data);


/*
* Store and retrieve breach event data
*/ 
esp_err_t dplock_store_breach_event(const dplock_breach_event_t *event);
esp_err_t dplock_get_breach_event(dplock_breach_event_t *event);


/*
* Clear all stored data
*/ 
esp_err_t dplock_clear_data(void);

/*
* Device authorization functions
*/
void dplock_add_authorized_device(const bd_addr_t bd_addr);
bool dplock_is_device_authorized(const bd_addr_t bd_addrr);
void dplock_start_ble_security(const bd_addr_t bd_addr);

void dplock_remove_authorized_device(const bd_addr_t bd_addr);

#endif