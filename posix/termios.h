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
 * \brief set the input and output baud rate.
 * \param termios_p pointer for control configuration.
 * \param speed speed (B19200, B57600, etc)
 * \retval 0 success
 * \retval -1 error
 */
extern int cfsetspeed (struct termios* termios_p, speed_t speed);


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
