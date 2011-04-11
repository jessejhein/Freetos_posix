/**
 * \addtogroup ethernet ETHERNET
 * @{
 * 
 * Implementation of Ethernet API
 */

/**
 * \defgroup netinet_in Socket Definition
 * @{
 */

/**
 * \file
 * Defines prototypes, macros, variables, and the sockaddr_in structure to use 
 * with Internet domain sockets.
 * author Dennis Tsang <dennis@amonics.com>
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

#ifndef _NETINET_IN_H
#define _NETINET_IN_H   1

#include <asm/types.h>

#ifdef ETHERNET_MOD
#include <uip.h>
#endif /* ETHERNET_MOD */


/* 
 * Standard well-defined IP protocols, for creating socket()
 */
/** Dummy protocol for TCP */
#define IPPROTO_IP      0
/** Transmission Control Protocol */
#define IPPROTO_TCP     6
/** User Datagram Protocol */
#define IPPROTO_UDP     17


/**
 * Primitive data type for ip address
 */
typedef unsigned char in_addr_t[4];

/**
 * \brief stores ipaddress in binary format
 */
struct in_addr
{
  /** ip address (network byte order) */
  in_addr_t s_addr;
};


/**
 * Function pointer type to a function in the form of
 * void myFunc (void) which is used within uip_app_register
 */
typedef void    (*eth_appcall_t)(void);


/**
 * \brief linux socket
 */
struct sockaddr_in 
{
  //unsigned char sin_len;
  /** address family (e.g. AF_INET) */
  unsigned short sin_family;
  /** port number (network byte order) */
  unsigned short sin_port;
  /** ip address (network byte order) */
  struct in_addr sin_addr;
  union
    {
      /** Same size as struct sockaddr */
      char sin_zero[8];
#ifdef ETHERNET_MOD
      /** appcall for uip */
      eth_appcall_t appcall;
#endif
    };
};

/*
#define ntohl(x)   (x)
#define ntohs(x)   (x)
#define htonl(x)   (x)
#define htons(x)   (x)
*/


#endif  /* netinet/in.h */

/** @} */
/** @} */
