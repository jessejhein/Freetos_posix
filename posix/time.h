/**
 * \addtogroup stdlib Standard Library
 * @{
 * 
 * Implementation of Standard Library using FreeRTOS API
 */

/**
 * \defgroup time_h time.h
 * @{
 * 
 * Implementation of Time function calls, interfacing between user applications and OS
 */

/**
 * \file
 * Implementation for standard time.h
 * \author Dennis Tsang <dennis@amonics.com>
 */

 
#ifndef TIME_H_
#define TIME_H_ 1

#include <asm/types.h>
#include <stddef.h>     //for definition of NULL

/**
 * \brief get the time (in seconds) since the Epoch (last reboot)
 * \param t If t is non-NULL, the return value is also stored in the memory pointed to by t.
 * \return the time since the Epoch
 * \remarks When using time() for comparing, do NOT compare like this:
 * \remarks \li while((int)time() <= wake_time) sleep(0); or
 * \remarks \li while(wake_time > (int)time()) sleep(0);
 * \remarks This cause the program to go to infinite loop when:
 * \remarks \li wake_time = max(int)
 * \remarks To resolve:
 * \remarks \li while((int)time() < wake_time) sleep(0); or
 * \remarks \li while(wake_time >= (int)time()) sleep(0);
 **/
extern time_t time(time_t *t);


/**
 * \brief get the time (in number of os clock ticks) since the Epoch (last reboot)
 * \param t If t is non-NULL, the return value is also stored in the memory pointed to by t.
 * \return the time since the Epoch
 * \remarks To compute the actual time (in ms) elapsed, multiply result by portTICK_RATE_MS
 * 
 * \remarks When using os_time() for comparing, do NOT compare like this:
 * \remarks \li while((int)os_time() <= wake_time) sleep(0); or
 * \remarks \li while(wake_time > (int)os_time()) sleep(0);
 * \remarks This cause the program to go to infinite loop when:
 * \remarks \li wake_time = max(int)
 * \remarks To resolve:
 * \remarks \li while((int)os_time() < wake_time) sleep(0); or
 * \remarks \li while(wake_time >= (int)os_time()) sleep(0);
 * 
 * \remarks Maximum time tick is limited by the resolution of counter set by
 *          configUSE_16_BIT_TICKS and the frequency of context switch set by 
 *          configTICK_RATE_HZ in <FreeRTOSConfig.h>
 * \remarks If configUSE_16_BIT_TICKS equals 1, the counter is 16-bit, otherwise the 
 *          counter is 32-bit
 * \remarks For 16-bit counter with context switch period of 10ms, maximum number of 
 *          sec is 65535*0.01 = 655 sec = 11 minutes
 * \remarks For 32-bit counter with context switch period of 10ms, maximum number of 
 *          sec is 4,294,967,295*0.01 = 42,949,672 sec = 3.3 yrs
 **/
extern time_t os_time(time_t *t);


#endif /* TIME_H_ */

/** @} */
/** @} */
