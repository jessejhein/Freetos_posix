/**
 * \addtogroup posix POSIX
 * @{
 *
 * Implementation of POSIX API using FreeRTOS API
 */

/**
 * \defgroup termios Terminal I/O
 * @{
 *
 * Implementation of Terminal I/O Interface
 * \li The termios functions describe a general terminal interface that is provided to control asynchronous communications ports
 * \li See: http://www.opengroup.org/onlinepubs/007908799/xsh/termios.h.html
 */

/**
 * \file
 * Terminal I/O Interface
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

#ifndef _TERMIOS_H
#define _TERMIOS_H    1

#include <asm/types.h>
#include <bits/termios.h>


/** 
 * \brief set the output baud rate.
 * \param termios_p pointer for control configuration
 * \param speed speed (B19200, B57600, etc)
 * \retval 0 success
 * \retval -1 error
 */
extern int cfsetospeed (struct termios* termios_p, speed_t speed);


/** 
 * \brief set the input baud rate.
 * \param termios_p pointer for control configuration
 * \param speed speed (B19200, B57600, etc)
 * \retval 0 success
 * \retval -1 error
 */
extern int cfsetispeed (struct termios* termios_p, speed_t speed);


/** 
 * \brief get the parameters associated with the terminal
 * \param fildes file descriptor for the terminal
 * \param termios_p pointer for control configuration.
 * \retval 0 success
 * \retval -1 error
 */
extern int tcgetattr (int fildes, struct termios *termios_p);


/** 
 * \brief set the parameters associated with the terminal
 * \param fildes file descriptor for the terminal
 * \param optional_actions optional actions
 * \param termios_p pointer for control configuration.
 * \retval 0 success
 * \retval -1 error
 * \remarks
 * optional_actions has no effect at the current implementation
 * \li If optional_actions is TCSANOW, the change will occur immediately.
 * \li If optional_actions is TCSADRAIN, the change will occur after all output written to fildes is transmitted. This function should be used when changing parameters that affect output.
 * \li If optional_actions is TCSAFLUSH, the change will occur after all output written to fildes is transmitted, and all input so far received but not read will be discarded before the change is made.
 */
extern int tcsetattr (int fildes, int optional_actions, const struct termios *termios_p);

#endif /* _TERMIOS_H */

/** @} */
/** @} */
