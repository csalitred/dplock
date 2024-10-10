idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES photodiode servo_motor button ble i2c_manager lwip esp_netif accelerometer)
