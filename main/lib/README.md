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