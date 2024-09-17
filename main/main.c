#include <stdlib.h>
#include "stdio.h"
#include "bpw21r_driver.h"

void app_main(void)
{
    adc_init();
    adc_read();
}