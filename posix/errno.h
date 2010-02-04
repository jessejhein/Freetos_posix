/**
 * \addtogroup posix POSIX
 * @{
 *
 * Implementation of POSIX API using FreeRTOS API
 */

/**
 * \defgroup errno System Error Numbers
 * @{
 *
 * Implementation of System Error Numbers
 * \li It provides a declaration for errno and give positive values for symbolic constants
 * \li See http://www.opengroup.org/onlinepubs/009695399/basedefs/errno.h.html
 */

/**
 * \file
 * System Error Numbers
 * \author Dennis Tsang <dennis@amonics.com>
 */

#ifndef ERRNO_H_
#define ERRNO_H_    1

#include <bits/errno.h>

/** store the system error number */
extern int errno;

#endif  /* ERRNO_H_ */

/** @} */
/** @} */
