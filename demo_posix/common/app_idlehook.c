/************************************************************************************************
 * File:            app_idlehook.c
 * Description:     subroutine in idle task
 ***********************************************************************************************/

#include <define.h>

#ifdef KB_MOD
extern void gpio_enter_key(void);
#endif

/************************************************************************************************
 * void vApplicationIdleHook(void)
 * +-- a subroutine that runs by Idle Task
 * +-- Idle Task will run every 1/configTICK_RATE_HZ (i.e. 10ms) and when there are no other tasks 
 *     running
 * +-- this function must not call usleep()
 * +-- this function must not implement infinite looping unless no other tasks is running 
 ************************************************************************************************/
void 
vApplicationIdleHook(void)
{
#ifdef KB_MOD
  gpio_enter_key();
#endif //end KB_MOD

  /*
   * Application Idle Task
   */
  UserIdleTask();     //define this task in define.h, 
                      //  #define idle_process     UserIdleTask
}
