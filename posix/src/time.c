/**
 * \file
 * Implementation for standard time.h
 * \author Dennis Tsang <dennis@amonics.com>
 */

#include <FreeRTOS.h>
#include <asm/types.h>

extern volatile time_t one_sec_cnt;
#ifndef FREERTOS_SCHED 
    extern volatile time_t jiffies;
#endif /* NOT FREERTOS_SCHED */

/*
 * $one_sec_cnt is incremented by Timer Interrupt
 */
time_t 
time(time_t *t)
{
  if(t != NULL)
    *t = (time_t) one_sec_cnt;
  return (time_t) one_sec_cnt;
}

//---------------------------------------------------------------------------
time_t 
os_time(time_t *t)
{
#ifdef FREERTOS_SCHED 
  portTickType time = xTaskGetTickCount();
#else /* NOT FREERTOS_SCHED */
  time_t time = jiffies;
#endif /* NOT FREERTOS_SCHED */
  if(t != NULL)
    *t = (time_t) time;
  return (time_t) time;
}
