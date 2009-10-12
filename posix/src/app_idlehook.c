/**
 * \file
 * Idle task
 * \author Dennis Tsang <dennis@amonics.com>
 */

/**
 * \page app_idlehook Idle task
 * 
 * \section sec1 INTRODUCTION
 * \par Idle task in FreeRTOS
 * Functions include:
 * \li detect push key
 * \li detect function key
 * \li update led status
 * \li user thread
 * 
 * \par Configuration
 * Define user task (void* foo(void*)) in define.h, and implement it in project 
 * \verbatim
    #define UserIdleTask()                  foo(NULL)
   \endverbatim
 * 
 * \par Limitations
 * \li do not call usleep() to suspend task (coroutine sleep is ok)
 * \li do not implement infinite looping unless no other tasks is running
 */

#include <define.h>
#include <stddef.h>
#include <kb.h>
#include <gpdi.h>
#include <led.h>

/**
 * \brief Idle Task
 */
void 
vApplicationIdleHook(void)
{
#ifdef KB_PUSH_KEY
  kb_push_key ();
#endif /* KB_PUSH_KEY */

#ifdef KB_FN_KEY
  kb_fn_key ();
#endif /* KB_FN_KEY */

#ifdef LED_MOD
  led_ctrl (NULL);
#endif /* LED_MOD */

#ifdef GPDI_MOD
  gpdi_ctrl (NULL);
#endif /* GPDI_MOD */

  //Application Idle Task
  UserIdleTask ();
}
