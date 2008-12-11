/************************************************************************************************
 * File:            app_idlehook.c
 * Description:     subroutine in idle task
 ***********************************************************************************************/

#include <define.h>
#include <stddef.h>

#ifdef KB_PUSH_KEY
extern void* kb_push_key(void);
#endif /* KB_PUSH_KEY */

#ifdef KB_FN_KEY
extern void* kb_fn_key(void);
#endif /* KB_FN_KEY */

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
#ifdef KB_PUSH_KEY
  kb_push_key();
#endif /* KB_PUSH_KEY */

#ifdef KB_FN_KEY
  kb_fn_key();
#endif /* KB_FN_KEY */

#ifdef LED_MOD
  led_ctrl();
#endif /* LED_MOD */

  /*
   * Application Idle Task
   */
  UserIdleTask();     //define this task in define.h, 
                      //  #define idle_process     UserIdleTask
}
