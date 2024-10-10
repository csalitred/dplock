idf_component_register(SRCS "accelerometer_driver.c"
                    INCLUDE_DIRS "include"
                    REQUIRES board i2c_manager)
