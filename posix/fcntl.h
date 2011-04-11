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

#ifndef _FCNTL_H
#define _FCNTL_H    1

/* Get the definitions of O_*, F_*, FD_*: all the
   numbers and flag bits for `open', `fcntl', et al.  */
#include <bits/fcntl.h>

#endif /* _FCNTL_H  */

/** @} */
/** @} */
