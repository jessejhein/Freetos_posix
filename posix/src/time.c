/**
 * \file
 * Implementation for standard time.h
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

#include <FreeRTOS.h>
#include <asm/types.h>
#include <time.h>
#include <sys/time.h>

extern time_t one_sec_count;
extern __u16 ms_count;


/*
 * $one_sec_count is incremented by Timer Interrupt
 */
time_t 
time (time_t *t)
{
  if (t != NULL)
    *t = (time_t) one_sec_count;
  return (time_t) one_sec_count;
}


clock_t 
clock (void)
{
  portTickType time = xTaskGetTickCount ();
  return (clock_t) time;
}


int
gettimeofday (struct timeval* tv, struct timezone* tz)
{
  if (tv != NULL)
    {
      /** number of seconds */
      tv->tv_sec = (time_t) one_sec_count ;
      /** number of remaining microseconds */
      tv->tv_usec = (time_t) ms_count * 1000UL;
      return 0;
    }
  return -1;
}
