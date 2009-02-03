/**
 * \mainpage FreeRTOS-POSIX
 *
 * \section sec1 INTRODUCTION
 * All source codes found under freertos_posix is designed to work in companion with
 * FreeRTOS version 5.0.3. FreeRTOS is downloadable from http://www.freertos.org/
 * or https://freertos.svn.sourceforge.net/svnroot/freertos
 * \n
 * \n 
 * \section sec2 COMPILE TIME CONFIGURATION
 * Include FreeRTOSConfig.h in project, see /config/FreeRTOSConfig.h.demo for example
 */

/**
 * \addtogroup posix POSIX
 * @{
 * 
 * Implementation of POSIX API using FreeRTOS API
 */

/**
 * \defgroup unistd System Call
 * @{
 * 
 * Implementation of System calls, interfacing between user applications and OS
 */

/**
 * \file
 * POSIX standard API for file stream
 * \author Dennis Tsang <dennis@amonics.com>
 */

#ifndef UNISTD_H_
#define UNISTD_H_       1

#ifdef FREERTOS_SCHED
#include <FreeRTOS.h>
#include <task.h>
#endif /* FREERTOS_SCHED */

#include <asm/types.h>
#include <sys/ioctl.h>

/**
 * \note MARCO for void usleep(unsigned long usec)
 * \li FreeRTOS version: vTaskDelay((portTickType)((u64)usec/(1000*portTICK_RATE_MS)))
 * \li Coroutine version: scrReturn((void*)-1)
 * \brief suspends process for (at least) usec microseconds
 * \param usec time (in us) to sleep
 * \remarks Minimum sleep time is 0 or greater than t, where t is the period for context switch,governed by configTICK_RATE_HZ in <FreeRTOSConfig.h>
 * \remarks Maximum sleep time is limited by the resolution of counter set by configUSE_16_BIT_TICKS and the frequency of context switch set by configTICK_RATE_HZ in <FreeRTOSConfig.h>
 * \li If configUSE_16_BIT_TICKS equals 1, the counter is 16-bit, otherwise the counter is 32-bit
 * \li For 16-bit counter with context switch period of 10ms, maximum number of sec is 65535*0.01 = 655 sec = 11 minutes
 * \li For 32-bit counter with context switch period of 10ms, maximum number of sec is 4,294,967,295*0.01 = 42,949,672 sec = 3.3 yrs
 */
#ifdef FREERTOS_SCHED
  #define usleep(usec)    vTaskDelay((portTickType)((__u64)usec/(1000*portTICK_RATE_MS)))
#else /* not FREERTOS_SCHED */
  #define usleep(usec)    scrReturn((void*)-1)
#endif /* not FREERTOS_SCHED */


/**
 * \note MARCO for unsigned int sleep(unsigned int seconds);
 * \li FreeRTOS version: usleep((u64)sec*1000000)
 * \li Coroutine version: scrReturn((void*)-1)
 * \brief makes the current process sleep until seconds seconds have elapsed
 * \param seconds time (in s) to sleep
 * \retval >0 the number of seconds left to sleep (not implemented)
 * \retval 0 if the requested time has elapsed (not implemented)
 * \remarks Maximum sleep time is limited by the resolution of counter set by configUSE_16_BIT_TICKS and the frequency of context switch set by configTICK_RATE_HZ in <FreeRTOSConfig.h>
 * \li If configUSE_16_BIT_TICKS equals 1, the counter is 16-bit, otherwise the counter is 32-bit
 * \li For 16-bit counter with context switch period of 10ms, maximum number of sec is 65535*0.01 = 655 sec = 11 minutes
 * \li For 32-bit counter with context switch period of 10ms, maximum number of sec is 4,294,967,295*0.01 = 42,949,672 sec = 3.3 yrs
 */
#ifdef FREERTOS_SCHED
  #define sleep(seconds)    usleep((__u64)seconds*1000000)
#else /* not FREERTOS_SCHED */
  #define sleep(seconds)    scrReturn((void*)-1)
#endif /* not FREERTOS_SCHED */


/**
 * \brief get the device ready
 * \param pathname device BASE (e.g. "0") + sub-item
 * \param flags accessing mode
 * \retval 0 - 9: valid file descriptors
 * \retval -1: error, errno is set appropriately
 */
extern int open(const char *pathname, int flags);


/**
 * \brief release the device
 * \param fd file descriptor obtained by open()
 * \retval 0 success
 * \retval -1: error, errno is set appropriately 
 */
extern int close(int fd);


/**
 * \brief write $count bytes from $buf to device
 * \param fd file descriptor obtained by open()
 * \param buf buffer which contains data to write out
 * \param count length of data (in bytes) to write out
 * \retval >0 number of bytes actually written to the file associated
 * \retval 0 no byte is written (busy/count = 0/communication fail)
 * \retval -1 error occur
 * \remarks For i2c devices, if there are multiple devices, write cannot be used in ISR 
 */
extern int write(int fd, void* buf, int count);


/**
 * \brief read $count bytes from device to $buf
 * \param fd file descriptor obtained by open()
 * \param buf buffer which contains allocation to read in data
 * \param count length of data (in bytes) to read from
 * \retval >0 number of bytes actually read from the file associated
 * \retval 0 no byte is read (busy/count = 0/communication fail)
 * \retval -1 error occur
 * \remarks For i2c devices, if there are multiple devices, read cannot be used in ISR
 */
extern int read(int fd, void* buf, int count);


/**
 * \brief change the device control setting (type referenced by $request) using data in $argp
 * \param fd file descriptor obtained by open()
 * \param request a device-dependent request code, defined in <sys/ioctl.h>
 * \param argp argument pointer for control data
 * \retval 0 success
 * \retval -1 error occur
 */
extern int ioctl(int fd, int request, void* argp);


/**
 * \enum LSEEK_OFFSET
 * Offset code for lseek()
 */
enum LSEEK_OFFSET
{
  /** Seek from beginning of file */
  SEEK_SET,
  /** Seek from current position of file, currently unimplemented */
  SEEK_CUR,
  /** Seek from the end of file, currently unimplemented */
  SEEK_END
};


/**
 * \brief adjust the position of the file pointer
 * \param fd file descriptor obtained by open()
 * \param offset indicates how much offset is needed #LSEEK_OFFSET
 * \param whence indicates what kind of offset is needed, currently unimplemented
 * \retval >=0 current offset from base address
 * \retval -1 error occur
 */
extern int lseek(int fd, int offset, int whence);


/** 
 * \example unistd_ex.c
 * This is an example of how to use the system call module.
 */

#endif /* UNISTD_H_  */

/** @} */
/** @} */
