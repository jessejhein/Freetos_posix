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

/**
 * \page unistd_page UNIX Standard API
 * This header file contains the mixed operation for FreeRTOS Task and CRTHREAD, the selection
 * of which operation is determined by \#define directives
 *
 * \section sec1 COMPILE TIME CONFIGURATION
 * \par Define MAX_CRTHREAD in source code
 * \verbatim
    #define USE_COROUTINE               1
    #include <unistd.h>
    void*
    foo (void* arg)
    {
      start_process ();
      while (1)
        {
          //Do something here
          usleep (0);
        }
      end_process ();
    }
   \endverbatim
 */

/*
 * Copyright (C) 2007-2011  Dennis Tsang <dennis@amonics.com>
 *
 * This file is part of freertos_posix.
 *
 * freertos_posix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * freertos_posix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with freertos_posix.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef UNISTD_H_
#define UNISTD_H_       1

#include <FreeRTOS.h>
#include <task.h>
#include <asm/types.h>
#include <sys/ioctl.h>

#ifndef TYPES_H_
#error "==================================================================="
#error "Project has not been setup correctly! Please setup by >$make config"
#error "==================================================================="
#endif /* TYPES_H_ */

//-----------------------------------------------------------------------------------------------

/**
 * \brief get the device ready
 * \param pathname device BASE (e.g. "0") + sub-item
 * \param flags accessing mode
 * \retval 0 - 9: valid file descriptors
 * \retval -1: error, errno is set appropriately
 */
extern int open (const char *pathname, int flags);


/**
 * \brief release the device
 * \param fd file descriptor obtained by open()
 * \retval 0 success
 * \retval -1: error, errno is set appropriately
 */
extern int close (int fd);


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
extern int write (int fd, void* buf, int count);


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
extern int read (int fd, void* buf, int count);


/**
 * \brief change the device control setting (type referenced by $request) using data in $argp
 * \param fd file descriptor obtained by open()
 * \param request a device-dependent request code, defined in <sys/ioctl.h>
 * \param argp argument pointer for control data
 * \retval 0 success
 * \retval -1 error occur
 */
extern int ioctl (int fd, int request, void* argp);


#ifndef SEEK_SET
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
#endif /* SEEK_SET */


/**
 * \brief adjust the position of the file pointer
 * \param fd file descriptor obtained by open()
 * \param offset indicates how much offset is needed #LSEEK_OFFSET
 * \param whence indicates what kind of offset is needed, currently unimplemented
 * \retval >=0 current offset from base address
 * \retval -1 error occur
 */
extern int lseek (int fd, int offset, int whence);


/**
 * \example unistd_ex.c
 * This is an example of how to use the system call module.
 */


//-----------------------------------------------------------------------------------------------
/**
 * \note MARCO for void usleep (unsigned long usec)
 * \brief suspends process for (at least) usec microseconds
 * \param usec time (in us) to sleep
 * \remarks Minimum sleep time is 0 or greater than t, where t is the period for context switch,governed by configTICK_RATE_HZ in <FreeRTOSConfig.h>
 * \remarks Maximum sleep time is limited by the resolution of counter set by configUSE_16_BIT_TICKS and the frequency of context switch set by configTICK_RATE_HZ in <FreeRTOSConfig.h>
 * \li If configUSE_16_BIT_TICKS equals 1, the counter is 16-bit, otherwise the counter is 32-bit
 * \li For 16-bit counter with context switch period of 10ms, maximum number of sec is 65535*0.01 = 655 sec = 11 minutes
 * \li For 32-bit counter with context switch period of 10ms, maximum number of sec is 4,294,967,295*0.01 = 42,949,672 sec = 3.3 yrs
 */
#define usleep(usec)            vTaskDelay((portTickType)((__u64)usec/(1000*portTICK_RATE_MS)))


/**
 * \note MARCO for unsigned int sleep (unsigned int seconds);
 * \brief makes the current process sleep until seconds seconds have elapsed
 * \param seconds time (in s) to sleep
 * \retval >0 the number of seconds left to sleep (not implemented)
 * \retval 0 if the requested time has elapsed (not implemented)
 * \remarks Maximum sleep time is limited by the resolution of counter set by configUSE_16_BIT_TICKS and the frequency of context switch set by configTICK_RATE_HZ in <FreeRTOSConfig.h>
 * \li If configUSE_16_BIT_TICKS equals 1, the counter is 16-bit, otherwise the counter is 32-bit
 * \li For 16-bit counter with context switch period of 10ms, maximum number of sec is 65535*0.01 = 655 sec = 11 minutes
 * \li For 32-bit counter with context switch period of 10ms, maximum number of sec is 4,294,967,295*0.01 = 42,949,672 sec = 3.3 yrs
 */
#define sleep(seconds)          usleep((__u64)seconds*1000000)

#endif /* UNISTD_H_ */

#ifdef USE_COROUTINE
#include <coroutine_st.h>
//-----------------------------------------------------------------
#ifdef start_process
#undef start_process
#endif /* start_process */
/**
 * \brief mark the start of coroutine thread
 */
#define start_process()         scrBegin
//-----------------------------------------------------------------
#ifdef end_process
#undef end_process
#endif /* end_process */
/**
 * \brief mark the end of coroutine thread
 */
#define end_process()           scrFinish((void*)0)
//-----------------------------------------------------------------
#ifdef usleep
#undef usleep
#endif /* usleep */
/**
 * \remarks coroutine version
 */
#define usleep(usec)            scrReturn((void*)-1)
//-----------------------------------------------------------------
#ifdef sleep
#undef sleep
#endif /* sleep */
/**
 * \remarks coroutine version
 */
#define sleep(sec)              scrReturn((void*)-1)
//-----------------------------------------------------------------
#endif /* USE_COROUTINE */


#ifdef USE_COROUTINE_REENTRANT
#include <coroutine_st.h>
//-----------------------------------------------------------------
#ifdef start_process
#undef start_process
#endif /* start_process */
/**
 * \brief mark the start of coroutine thread
 * \remarks reentrant version
 */
#define start_process()         switch (ARG->scrLine) { case 0:;
//-----------------------------------------------------------------
#ifdef end_process
#undef end_process
#endif /* end_process */
/**
 * \brief mark the end of coroutine thread
 * \remarks reentrant version
 */
#define end_process()           ARG->scrLine = 0;} return ((void*)0)
//-----------------------------------------------------------------
#ifdef usleep
#undef usleep
#endif /* usleep */
/**
 * \remarks reentrant version
 */
#define usleep(usec)            do { \
                                    ARG->scrLine =__LINE__; \
                                    return ((void*)-1); case __LINE__:; \
                                 } while (0)
//-----------------------------------------------------------------
#ifdef sleep
#undef sleep
#endif /* sleep */
/**
 * \remarks reentrant version
 */
#define sleep(sec)              usleep(sec)
//-----------------------------------------------------------------
#endif /* USE_COROUTINE_REENTRANT */

/** @} */
/** @} */
