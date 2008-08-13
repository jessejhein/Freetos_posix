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
 * Implementation for standard time.h example
 * \author Dennis Tsang <dennis@amonics.com>
 */

#include <time.h>
#include <unistd.h>

void* 
foo(void* ptr)
{
  start_process();
    {
      //...
      
      static time_t start_time;
      start_time = time( (time_t) NULL );
      while( (time_t) (time( (time_t) NULL ) - start_time ) < 50 ) usleep(0);
      
      //continue after 50 sec

      static clock_t begin_time;
      begin_time = clock();
      while( (clock_t) (clock() - begin_time ) < 50 ) usleep(0);
      
      //continue after 50 clock ticks (i.e. 50/CLOCKS_PER_SEC) sec

    }
  end_process();
}

/** @} */
/** @} */
