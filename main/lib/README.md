# Library Components

## GATT Profile (gatt.c & gatt.h)

These files define and manage the GATT (Generic Attribute Profile) for our BLE device.
### Key Components:

1. **Services**: Define the services offered by our device
   - Sensor Data Service
   - Motor Control Service

2. **Characteristics**: Define the characteristics within each service
   - Sensor Data Characteristic (Read, Notify)
   - Motor Control Characteristic (Write)

3. **Callbacks**: Implement callback functions for handling:
   - Read requests (for sensor data)
   - Write requests (for motor control)
   - Notifications (if implementing sensor data push)

   