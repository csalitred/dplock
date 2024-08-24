import uuid

# Use the same UUID generation method for both esp32s3 code and smartphone app

def generate_ble_uuid():
    # Generate a random 128-bit UUID
    full_uuid = uuid.uuid4()

    # Convert to ESP32 format (little-endian, comma-seperated hex)
    uuid_bytes = full_uuid.bytes_le
    esp32_format = ', '.join(f'0x{byte:02x}' for byte in uuid_bytes)

    print(f"Full UUID: {full_uuid}")
    print(f"ESP format: {esp32_format}")
    return str(full_uuid), esp32_format

# Generate UUIDs for service and characteristics
service_uuid = generate_ble_uuid()
historical_data_uuid = generate_ble_uuid()
data_control_uuid = generate_ble_uuid()
data_metadata_uuid = generate_ble_uuid()

print("\nService UUID:", service_uuid[0])
print("Historical Data Characteristic UUID:", historical_data_uuid[0])
print("Data Control Characteristic UUID:", data_control_uuid[0])
print("Data Metadata Characteristic UUID:", data_metadata_uuid[0])

"""
OUTPUT:

Full UUID: 3c58821e-0801-42a1-aa10-cf5f094a2ae6
ESP format: 0x1e, 0x82, 0x58, 0x3c, 0x01, 0x08, 0xa1, 0x42, 0xaa, 0x10, 0xcf, 0x5f, 0x09, 0x4a, 0x2a, 0xe6
Full UUID: af19db44-effd-4a09-a7d3-245250a2bfe3
ESP format: 0x44, 0xdb, 0x19, 0xaf, 0xfd, 0xef, 0x09, 0x4a, 0xa7, 0xd3, 0x24, 0x52, 0x50, 0xa2, 0xbf, 0xe3
Full UUID: d2024fbc-ba60-4387-8da5-c8afd58fcf8c
ESP format: 0xbc, 0x4f, 0x02, 0xd2, 0x60, 0xba, 0x87, 0x43, 0x8d, 0xa5, 0xc8, 0xaf, 0xd5, 0x8f, 0xcf, 0x8c
Full UUID: 2a5cd4f2-d72b-4ecd-a701-54719cd56850
ESP format: 0xf2, 0xd4, 0x5c, 0x2a, 0x2b, 0xd7, 0xcd, 0x4e, 0xa7, 0x01, 0x54, 0x71, 0x9c, 0xd5, 0x68, 0x50

Service UUID: 3c58821e-0801-42a1-aa10-cf5f094a2ae6
Historical Data Characteristic UUID: af19db44-effd-4a09-a7d3-245250a2bfe3
Data Control Characteristic UUID: d2024fbc-ba60-4387-8da5-c8afd58fcf8c
Data Metadata Characteristic UUID: 2a5cd4f2-d72b-4ecd-a701-54719cd56850
"""