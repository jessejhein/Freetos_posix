/************************************************************************************************
 * File: 			app_led.c
 * Description:		led task employing posix thread and file API
 ***********************************************************************************************/

#include <define.h>
#include <unistd.h>

/************************************************************************************************
 * Function:            void* tskFlashLED(void* ptr)
 * 
 * Description:         toggle led at a specified rate
 * 
 * Input:               unsigned int[2]
 *                      ptr[0]: led id
 *                      ptr[1]: 50% duty cycle (in sec)
 * 
 * Output:              should never return
 ***********************************************************************************************/
#define ARG     ((unsigned int*)ptr)    
void* tskFlashLED(void *ptr)
{
    start_process();

    while(1){
        sleep( ARG[1] );	
        led_toggle(ARG[0]);
    }

    end_process();
}
