#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#define DEVICE_NAME         "ESP32-S3 BLE Device"
#define SERVICE_UUID        //  Need generate UUID
#define CHARACTERISTIC_UUID // Need to generate UUID

void ble_init(void);
void ble_advertise(void);
int ble_gap_event(struct ble_gap_event *event, void *arg);
void ble_app_on_sync(void);
void host_task(void *param);

#endif 