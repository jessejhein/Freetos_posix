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

#ifndef ERRNO_H_
#define ERRNO_H_    1

#include <bits/errno.h>

/** store the system error number */
extern int errno;

#endif  /* ERRNO_H_ */

/** @} */
/** @} */
