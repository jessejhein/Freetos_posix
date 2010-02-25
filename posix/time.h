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
#include <FreeRTOS.h>


/** Number of clock ticks per second */
#define CLOCKS_PER_SEC             configTICK_RATE_HZ


/** A time value that is accurate to the nearest microsecond but also has a range of years. */
struct timeval
{
  /** This represents the number of whole seconds of elapsed time. */
  time_t tv_sec;        /* seconds */
  /** This is the rest of the elapsed time (a fraction of a second), represented as the number of microseconds. It is always less than one million. */
  suseconds_t tv_usec;  /* microseconds */
};


/** Structure crudely representing a timezone. */
struct timezone
{
  int tz_minuteswest;   /* Minutes west of GMT.  */
  int tz_dsttime;	/* Nonzero if DST is ever in effect.  */
};


/**
 * \brief get the time (in seconds) since the Epoch (last reboot)
 * \param t If t is non-NULL, the return value is also stored in the memory pointed to by t.
 * \return the time since the Epoch
 */
extern time_t time (time_t *t);


/**
 * \brief get the number of clock ticks since the Epoch (last reboot)
 * \return the time since the Epoch
 * \remarks To compute the actual time (in sec) elapsed, divide result by CLOCKS_PER_SEC
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
 */
extern clock_t clock (void);


/**
 * \brief get the current time, expressed as seconds and microseconds since the Epoch (last reboot)
 * \param tv store time of day
 * \param tz store timezone information, if tz is NULL, tz is not filled.
 * \retval 0 success.
 * \retval -1 errors.
 */
extern int gettimeofday (struct timeval *tv, struct timezone *tz);


/** 
 * \example time_ex.c
 * This is an example of how to use the time call module.
 */


#endif /* TIME_H_ */

/** @} */
/** @} */
