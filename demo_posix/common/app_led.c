/************************************************************************************************
 * File: 			app_led.c
 * Description:		led task employing posix thread and file API
 ***********************************************************************************************
 * SUMMARY:
 * 	1) Use upto two LEDs 
 * 	2) Periodically toggle the LEDs at rates specified by SLEEP_TIME
 ***********************************************************************************************/

#include <define.h>
#include <unistd.h>

/************************************************************************************************
 * Global Variables
 ***********************************************************************************************/
const unsigned int SLEEP_TIME[] = {1, 3}; //sleep for 1 and 3 sec.

/************************************************************************************************
 * tskFlashLED()
 ***********************************************************************************************/
void* tskFlashLED(void *ptr)
{
    unsigned int index = *((unsigned int*)ptr);
    unsigned int sleep_index = index%2;
    start_process();
    //-------------------------------------------------------------------
    
    while(1){
        sleep((unsigned int)SLEEP_TIME[sleep_index]);	
        if(led_status(index) == 0){
            led_on(index);
        } else{
            led_off(index);
        }
    }
	
    //-------------------------------------------------------------------
    end_process();
}
