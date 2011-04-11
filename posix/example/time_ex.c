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

#include <time.h>
#include <unistd.h>

void* 
foo (void* ptr)
{
  start_process ();
    {
      //...
      
      static time_t start_time;
      start_time = time ((time_t) NULL);
      while ((time_t) (time ((time_t) NULL) - start_time) < 50) usleep(0);
      
      //continue after 50 sec

      static clock_t begin_time;
      begin_time = clock ();
      while ((clock_t) (clock() - begin_time) < 50) usleep(0);
      
      //continue after 50 clock ticks (i.e. 50/CLOCKS_PER_SEC) sec

    }
  end_process ();
}

/** @} */
/** @} */
