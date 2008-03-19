/*
 * time.h
 */
 
#ifndef TIME_H_
#define TIME_H_ 1

#include <asm/types.h>
#include <stddef.h>     //for definition of NULL

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
 extern time_t time(time_t *t);

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
extern time_t os_time(time_t *t);


#endif /* TIME_H_ */
