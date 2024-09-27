#include <stdlib.h>
#include "stdio.h"
// #include "bpw21r_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #include "i2c_manager.h"
// #include "hs5086wp.h"

void app_main(void)
{
    /*

    init all components 
    wait for interrupts to happen (button, accel, photodiode(if possible), ble(if possible))

    if no interrupts -> IDLE
    if interrupts -> case will match component interrupt 
    
    State Machine
    * case 0 : IDLE
        LOw POWER MODE
    * case 1 : ???

    * case 2 : AUTHORIZED ACCESS
        Log access in mem
    * case 3 : BLE
        Data transmit and recieve (for unlock & manifest)
    * case 4 : DROP
        Log time stamp, and x,y,z data in mem
    * case 5 : BREACH (LOOPS??)
        End of state machine 
    * 
    * 
    */
}