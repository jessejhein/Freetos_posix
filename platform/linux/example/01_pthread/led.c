/**
 * \file
 * led task employing posix thread and file API
 * \author Dennis Tsang <dennis@amonics.com>
 */

#include <define.h>
#include <unistd.h>

/**
 * \brief toggle led at a specified rate
 * \param ptr [0]: led id
 * \n         [1]: 50% duty cycle (in sec)
 * \return never return
 */
void* 
tskFlashLED (void *ptr)
{
  unsigned int led = *(((unsigned int*)ptr));
  unsigned int sleep_time = *(((unsigned int*)ptr) + 1);

  start_process ();

  while (1)
    {
      sleep (sleep_time);
      if (led_status (led) == 0)
        {
          led_on (led);
        }
      else
        {
          led_off (led);
        }	
    }

  end_process ();
}
