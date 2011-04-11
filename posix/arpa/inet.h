/**
 * \addtogroup ethernet ETHERNET
 * @{
 * 
 * Implementation of Ethernet API
 */

/**
 * \defgroup arpa_inet Internet Address Manipulation
 * @{
 */

/**
 * \file
 * Defines prototypes for those network library routines that convert Internet address and 
 * dotted-decimal notation, for example, inet_makeaddr().
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

#ifndef _ARPA_INET_H
#define _ARPA_INET_H    1

#include <netinet/in.h>

/**
 * \brief converts the Internet host address name from the standard numbers-and-dots 
 *        notation into binary data and stores it in the struct in_addr that addr points to.
 * \param name ip address in string format (e.g. 192.168.1.1)
 * \param addr ip address in binary format (e.g. C0 A8 01 01)
 * \retval 0 not valid name
 * \retval 1 valid name
 */
extern int inet_aton (const char *name, struct in_addr *addr);


/**
 * \brief converts the Internet host address addr to a string in the standard 
 *        numbers-and-dots notation. 
 * \param addr ip address in binary format (e.g. C0 A8 01 01)
 * \return a pointer into a statically-allocated buffer (e.g. 192.168.1.1)
 */
extern char* inet_ntoa (struct in_addr addr);

#endif /* arpa/inet.h */

/** @} */
/** @} */
