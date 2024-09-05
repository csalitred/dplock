idf_component_register(SRCS "servo_driver.c"
                    INCLUDE_DIRS "include"
                    REQUIRES board esp_timer esp_driver_mcpwm)
