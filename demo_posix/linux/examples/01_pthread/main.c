/**
 * \file
 * This program creates 2 threads (tasks). 
 * 1) Thread 1 flashes an led every 2 sec.
 * 2) Thread 2 flashes an led every 4 sec.
 * \author Dennis Tsang <dennis@amonics.com>
 */

#include <pthread.h>
#include <define.h>
#include <fcntl.h>
#include <led.h>


/**
 * \brief set up hardware
 */
void 
vSetupHardware(void)
{
  led_init();
}


/**
 * \brief user main
 */
void 
vUserMain()
{
  //Identify your threads here
  pthread_t thread_led1, thread_led2;

  static unsigned int arg_led1[] = {0, 1};
  static unsigned int arg_led2[] = {1, 2};

  //Create your threads here
  pthread_create(&thread_led1, NULL, tskFlashLED, arg_led1);
  pthread_create(&thread_led2, NULL, tskFlashLED, arg_led2);
}
