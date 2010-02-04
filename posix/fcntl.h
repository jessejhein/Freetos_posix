/**
 * \addtogroup posix POSIX
 * @{
 *
 * Implementation of POSIX API using FreeRTOS API
 */

/**
 * \defgroup fcntl File Control
 * @{
 *
 * Implementation of file control options
 * \li It defines the requests and arguments for use by the functions fcntl() and open().
 * \li See http://www.opengroup.org/onlinepubs/009695399/basedefs/fcntl.h.html
 */

/**
 * \file
 * File Control
 * \author Dennis Tsang <dennis@amonics.com>
 */

#ifndef _FCNTL_H
#define _FCNTL_H    1

/* Get the definitions of O_*, F_*, FD_*: all the
   numbers and flag bits for `open', `fcntl', et al.  */
#include <bits/fcntl.h>

#endif /* _FCNTL_H  */

/** @} */
/** @} */
