/**
 * \file
 * Implementation for standard time.h
 * \author Dennis Tsang <dennis@amonics.com>
 */

#include <FreeRTOS.h>
#include <asm/types.h>
#include <time.h>

extern volatile time_t one_sec_cnt;
extern volatile int timer_count;
#ifndef FREERTOS_SCHED 
extern volatile time_t jiffies;
#endif /* NOT FREERTOS_SCHED */


/*
 * $one_sec_cnt is incremented by Timer Interrupt
 */
time_t 
time (time_t *t)
{
  if (t != NULL)
    *t = (time_t) one_sec_cnt;
  return (time_t) one_sec_cnt;
}


clock_t 
clock (void)
{
#ifndef FREERTOS_SCHED
  clock_t time = jiffies;
#else /* NOT FREERTOS_SCHED */
  portTickType time = xTaskGetTickCount();
#endif /* NOT FREERTOS_SCHED */
  return (clock_t) time;
}


int
gettimeofday (struct timeval* tv, struct timezone* tz)
{
  if (tv != NULL)
    {
      /** number of seconds */
      tv->tv_sec = (time_t) one_sec_cnt ;
      /** number of remaining microseconds */
      tv->tv_usec = (time_t) timer_count * (1000000UL / configTICK_RATE_HZ);
      return 0;
    }
  return -1;
}
