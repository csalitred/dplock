#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "stdbool.h"
#include "nvs_manager.h"  // Add this for manifest types

#define DEVICE_NAME         "ESP32-S3 BLE Device"

// Your existing UUIDs
#define SERVICE_UUID 0x1e, 0x82, 0x58, 0x3c, 0x01, 0x08, 0xa1, 0x42, 0xaa, 0x10, 0xcf, 0x5f, 0x09, 0x4a, 0x2a, 0xe6
#define HISTORICAL_DATA_SERVICE_UUID 0x44, 0xdb, 0x19, 0xaf, 0xfd, 0xef, 0x09, 0x4a, 0xa7, 0xd3, 0x24, 0x52, 0x50, 0xa2, 0xbf, 0xe3
#define SENSOR_CONTROL_UUID 0xbc, 0x4f, 0x02, 0xd2, 0x60, 0xba, 0x87, 0x43, 0x8d, 0xa5, 0xc8, 0xaf, 0xd5, 0x8f, 0xcf, 0x8c
#define SENSOR_METADATA_UUID 0xf2, 0xd4, 0x5c, 0x2a, 0x2b, 0xd7, 0xcd, 0x4e, 0xa7, 0x01, 0x54, 0x71, 0x9c, 0xd5, 0x68, 0x50

// Control commands
#define CMD_UNLOCK      0x01
#define CMD_GET_MANIFEST 0x02

// Manifest data structure for BLE transmission
typedef struct {
    trip_info_t info;
    uint32_t event_count;
    event_record_t events[];  // Flexible array member
} __attribute__((packed)) manifest_data_t;

// Your existing function declarations
void ble_init(void);
void ble_advertise(void);
int ble_gap_event(struct ble_gap_event *event, void *arg);
void ble_app_on_sync(void);
void host_task(void *param);
bool is_ble_connected(void);
esp_err_t update_historical_data(uint8_t *data, size_t len);
esp_err_t update_sensor_metadata(uint8_t *data, size_t len);

// Add new manifest-specific function declarations
esp_err_t send_manifest_data(void);
esp_err_t process_control_command(uint8_t command);

#endif