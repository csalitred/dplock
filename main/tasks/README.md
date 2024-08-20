# DPLOCK task-specific implementations

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