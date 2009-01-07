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
void* tskFlashLED(void *ptr)
{
    unsigned int led = *(((unsigned int*)ptr));
    unsigned int sleep_time = *(((unsigned int*)ptr)+1);

    start_process();

    while(1){
        sleep(sleep_time);	
        led_toggle(led);
    }

    end_process();
}
