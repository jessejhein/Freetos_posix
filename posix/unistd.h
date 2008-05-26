/**
 * \mainpage FreeRTOS-POSIX
 *
 * \section sec1 INTRODUCTION
 * All source codes found under freertos_posix is designed to work in companion with
 * FreeRTOS version 4.1.3. FreeRTOS is downloadable from http://www.freertos.org/
 * or https://freertos.svn.sourceforge.net/svnroot/freertos
 * \n
 * \n 
 * \section sec2 COMPILE TIME CONFIGURATION
 * Set the following before compiling:
 * \par <./include/FreeRTOSConfig.h>
 * \li Set uP speed for desired MIPs
 * \verbatim
      #define configCPU_CLOCK_HZ             ( ( unsigned portLONG ) 30000000 )
   \endverbatim
 * \li Set desired kernel clock rate (time for context switch)
 * \verbatim
      #define configTICK_RATE_HZ             ( ( portTickType ) 100 )    //10ms
   \endverbatim
 * \li Set desired HEAP_SIZE managed by kernel
 * \verbatim
      #define configTOTAL_HEAP_SIZE          ( ( size_t ) 1024 )
   \endverbatim
 * \li Set desired stack size for task (e.g. sprintf() requires extra stack space)
 * \verbatim
      #define configMINIMAL_STACK_SIZE       ( 205 )                     //x2 for number of bytes
   \endverbatim
 * \li Reduce RAM requirement by lowering the priority
 * \verbatim
      #define configMAX_PRIORITIES           ( ( unsigned portBASE_TYPE ) 1 ) //Idle Task priority
   \endverbatim
 * \li Reduce RAM requirement by using cooperative scheduling
 * \verbatim
      #define configUSE_PREEMPTION           1
   \endverbatim
 * \li Reduce code size by excluding unnecessary task functions
 * \verbatim
      #define INCLUDE_vTaskPrioritySet       0
      #define INCLUDE_uxTaskPriorityGet      0
      #define INCLUDE_vTaskDelete            0
      #define INCLUDE_vTaskCleanUpResources  0
      #define INCLUDE_vTaskSuspend           0
      #define INCLUDE_vTaskDelayUntil        0
      #define INCLUDE_vTaskDelay             1
   \endverbatim
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
 * \li FreeRTOS version: vTaskDelay((portTickType)((__u64)usec/(1000*portTICK_RATE_MS)))
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
 * \li FreeRTOS version: usleep((__u64)sec*1000000)
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
 * \remarks all devices can be open with O_RDWR/O_RDONLY/O_WRONLY except 
 * \li ADC can only be opened for O_RDONLY mode
 * \li PWM can be opened with all mode, but only write operation is supported
 * \li when I2C EEPROM is in NON-BLOCKING mode, application can only write upto 64 bytes at a time to eeprom
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
 * \remarks When I2C EEPROM is in NON-BLOCKING mode, application can only write upto 64 bytes at a time to eeprom
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
