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

#ifndef SYS_TIME_H_
#define SYS_TIME_H_ 1

#include <asm/types.h>
#include <stddef.h>     //for definition of NULL


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


#endif /* SYS_TIME_H_ */

/** @} */
/** @} */
