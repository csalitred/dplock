/* 
GATT (Generic Attribute Profile)
Services: "Sensor Data Service"
Characteristics:
    1. "Historical Data" (Read, Notify): used to transfer JSON data from esp32s3 to smartphone.
    2. "Data Control" (Write): request data, cleared stored data etc. commands sent from smartphone
    3. "Data Metadata" (Read)[Optional]: info about stored data (e.g time range, number of records etc.)
*/

#ifndef GATT_H
#define GATT_H

#include <stdint.h>

// Service UUID
#define SENSOR_DATA_SERVICE_UUID

// Characteristic UUIDs
#define HISTORICAL_DATA_CHAR_UUID
#define DATA_CONTROL_CHAR_UUID
#define DATA_METADATA_CHAR_UUID

// Attribute table size
#define ATTR_TABLE_SIZE

// Characteristic properties
#define HISTORICAL_DATA_CHAR_PROPS  (ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY)
#define DATA_CONTROL_CHAR_PROPS     ESP_GATT_CHAR_PROP_BIT_WRITE
#define DATA_METADATA_CHAR_PROPS    ESP_GATT_CHAR_PROP_BIT_READ

// Function properties
void init_sensor_data_service(void);
void handle_historical_data_read(void);
void handle_data_control_write(uint8_t *data, uint16_t length);
void notify_historical_data(uint8_t *data, uint16_t length);

#endif

