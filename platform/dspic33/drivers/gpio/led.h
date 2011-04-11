/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup led LED
 * @{
 */

/**
 * \file
 * LED Driver
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

#ifndef LED_H_
#define LED_H_

#ifdef LED_MOD

/**
 * \brief get led port ready
 * \param flags accessing mode
 * \retval 0 OK
 */
extern int led_open (int flags);


/**
 * \brief write the status to selected led channel
 * \param buf pointer to buffer for writing
 * \retval -1 error not opened for write operation (errno = EBADF)
 * \retval 1 indicating 1 byte has been written
 */
extern int led_write (unsigned char *buf);


/**
 * \brief change the configuration of led module
 * \param request Request code - defined in ioctl.h (LED_SELECT_CH) 
 * \param argp channel id
 * \retval 0 success
 * \retval -1 error
 */
extern int led_ioctl (int request, unsigned char* argp);


/**
 * \brief led control job
 * \param arg no use
 */
extern void* led_ctrl (void* arg);

#endif /* LED_MOD */

#endif /* LED_H_ */

/** @} */
/** @} */
