/*
 * time.c
 */

#include <FreeRTOS.h>
#include <asm/types.h>

extern volatile time_t one_sec_cnt;
#ifndef FREERTOS_SCHED 
    extern volatile time_t jiffies;
#endif

/**************************************************************************
 * Name:        time_t time(time_t *t)
 * 
 * Input:       If t is non-NULL, the return value is also stored in the memory 
 *              pointed to by t.
 * 
 * Output:      returns the time since the Epoch (time since last 
 *              reboot), measured in seconds.
 * 
 * CAUTION:     when using time() for comparing, do NOT compare like this:
 *                      while((int)time() <= wake_time) sleep(0); or
 *                      while(wake_time > (int)time()) sleep(0);
 *              this cause the program to go to infinite loop when
 *                      wake_time = max(int)
 *              to resolve
 *                      while((int)time() < wake_time) sleep(0); or
 *                      while(wake_time >= (int)time()) sleep(0);
 ***************************************************************************/
time_t time(time_t *t)
{
    if(t != NULL)
        *t = (time_t) one_sec_cnt;
    return (time_t) one_sec_cnt;
}

/**************************************************************************
 * Name:        time_t os_time(time_t *t)
 * 
 * Input:       If t is non-NULL, the return value is also stored in the memory 
 *              pointed to by t.
 * 
 * Output:      returns the time since the Epoch (time since last 
 *              reboot), measured in number of os clock ticks.
 * 
 * Note:        to compute the actual time (in ms) elapsed, multiply result by 
 *              portTICK_RATE_MS
 * 
 * CAUTION:     when using os_time() for comparing, do NOT compare like this:
 *                      while((int)os_time() <= wake_time) sleep(0); or
 *                      while(wake_time > (int)os_time()) sleep(0);
 *              this cause the program to go to infinite loop when
 *                      wake_time = max(int)
 *              to resolve
 *                      while((int)os_time() < wake_time) sleep(0); or
 *                      while(wake_time >= (int)os_time()) sleep(0);
 *************************************************************************** 
 * Maximum time tick is limited by the resolution of counter set by 
 * configUSE_16_BIT_TICKS and the frequency of context switch set by 
 * configTICK_RATE_HZ in <FreeRTOSConfig.h>
 * If configUSE_16_BIT_TICKS equals 1, the counter is 16-bit, otherwise the 
 * counter is 32-bit
 * For 16-bit counter with context switch period of 10ms, maximum number of 
 * sec is 65535*0.01 = 655 sec = 11 minutes
 * For 32-bit counter with context switch period of 10ms, maximum number of 
 * sec is 4,294,967,295*0.01 = 42,949,672 sec = 3.3 yrs
 ***************************************************************************/
time_t os_time(time_t *t)
{
#ifdef FREERTOS_SCHED 
    portTickType time = xTaskGetTickCount();
#else
    time_t time = jiffies;
#endif        
    if(t != NULL)
        *t = (time_t) time;
    return (time_t) time;
}
