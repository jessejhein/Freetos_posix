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

#if !defined _SYS_SOCKET_H && !defined _NETINET_IN_H
# error "Never include <bits/socket.h> directly; use <sys/socket.h> instead."
#endif


/* Types of sockets.  */
#define SOCK_STREAM         1
#define SOCK_DGRAM          2
#define SOCK_RAW            3
#define SOCK_RDM            4
#define SOCK_SEQPACKET      5
#define SOCK_PACKET         10

/* Protocol families.  */
#define PF_UNSPEC           0   /* Unspecified.  */
#define PF_INET             2   /* IP protocol family.  */

/* Address families.  */
#define AF_INET             PF_INET

/** socket address */
struct sockaddr
{
  //unsigned char sa_len;
  /** address family (e.g. AF_INET) */         
  unsigned short sa_family;
  /** 14 bytes that are reserved to hold the address itself */
  char sa_data[14];
};

