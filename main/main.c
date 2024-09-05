#include <stdio.h>
#include "servo_driver.h"

void app_main(void)
{
    servo_init();
    servo_rotate();
}