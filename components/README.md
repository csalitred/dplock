# Library Components

## GATT Profile (gatt.c & gatt.h)

These files define and manage the GATT (Generic Attribute Profile) for our BLE device.
### Key Components:

1. **Services**: Define the services offered by our device
   - Sensor Data Service: 
      - e.g Battery Service, Thermometer Service 

2. **Characteristics**: Define the characteristics within each service
   - Sensor Data Characteristic (Read/Write, Notify/Indicate, security)
   - e.g Battery level, Temperature

3. **Profile**: Defining behavior of both the client and server for services, characteristics, connections and security requirements.
   - Definition of roles & relationship between GATT server and the client.
   - Required services
   - Service requirements
   - Security considerations

![GATT Profile Example](github.com/csalitred/dplock/docs/gatt_profile_example.png)

```
Attributes: generic term for any type of data exposed by the server & defines the structure of this data.
    Attribute type (UUID):
        - 16 bit number (SIG-Adopted Attributes, e.g temp measurement: 0x2A1C)
        - 128 bit (custom attribute types defined by developer also reffered to as vendor-specific UUIDs)
    Attribute Handle:
        - 16 bit value assigned to each attribute by the server (like an address)
        - range of handles is 0x0001-0xFFFF (value of 0x0000 is reserved)
    Attributes Permissions:
        - read/write, notified/indicated, and what security levels required for each operation.
```

## Bluetooth 

ESP-IDF has the option for NimBLE-based Host APIs. Which has low power consumption, is light weight (smaller memory footprint), simple to implement and has good performance for data transfer. 

### BLE Implementation Overview:

1. **BLE Stack Initialization**: Set up NimBLE stack, HCI, and controller
2. **GAP Configuration**: Set device name and properties
3. **GATT Setup**: Define services and characteristics for sensor data and motor control
4. **Advertisement**: Configure and start BLE advertising
5. **Connection Handling**: Manage BLE connections and state transitions
6. **Power Management**: Implement low-power and active modes
7. **Data Transmission**: Send sensor data upon request when connected
8. **Command Reception**: Receive and process motor actuation commands

## Power States

- **Low Power Mode**: Active when no BLE connection is established
- **Normal Mode**: Activated upon BLE connection for data transmission and command processing

## Data Flow

- Sensor data is only transmitted when a BLE connection is established and data is requested
- Motor actuation only occurs in response to commands received over an active BLE connection

### Programmin Sequence

```
Typical programming sequence with NimBLE stack:
    - Initialize NVS flash using nvs_flash_init() API. This is because ESP controller uses NVS - 
    
    - during initialization.

    - Initialize the host and controller stack using nimble_port_init.

    - Initialize the required NimBLE host configuration parameters and callbacks

    - Perform application specific tasks/initialization

    - Run the thread for host stack using nimble_port_freertos_init