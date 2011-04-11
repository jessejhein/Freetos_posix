/**
 * \addtogroup ethernet ETHERNET
 * @{
 * 
 * Implementation of Ethernet API
 */

/**
 * \defgroup socket Linux Socket
 * @{
 * 
 * Implementation of Socket API
 */

/**
 * \file
 * Socket
 * \author Dennis Tsang <dennis@amonics.com>
 */

/**
 * \page socket SOCKET
 * \section socket_sec1 COMPILE TIME CONFIGURATION
 * \par Set the following in <.config_freertos_posix> before compiling
 * \verbatim
    CONFIG_ETHERNET             =   y 
   \endverbatim
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

#ifndef _SYS_SOCKET_H
#define _SYS_SOCKET_H   1

#include <bits/socket.h>

/**
 * shutdown options
 */
enum SHUTDOWN
{
  /** Further receives are disallowed */
  SHUT_RD,
  /** Further sends are disallowed */
  SHUT_WR,
  /** Further sends and receives are disallowed */
  SHUT_RDWR
};


/**
 * \brief create an endpoint for communication
 * \param domain address family (supports only AF_INET)
 * \param type socket type (supports only SOCK_STREAM or SOCK_DGRAM)
 * \param protocol protocol family (supports only IPPROTO_IP/IPPROTO_TCP/IPPROTO_UDP) 
 * \retval >=0 socket descriptor
 * \retval -1 error
 */
extern int socket(int domain, int type, int protocol);


/**
 * \brief shutdown a communication
 * \param sockfd socket descriptor
 * \param how shutdown option #SHUTDOWN (ignored)
 * \retval 0 success
 * \retval -1 error
 */
extern int shutdown(int sockfd, int how);


/**
 * \brief attaches a local endpoint to a socket.
 * \param sockfd socket descriptor
 * \param my_addr information about local port and IP address
 * \param addrlen set to sizeof(struct sockaddr).
 * \retval 0 success
 * \retval -1 error
 */
extern int bind(int sockfd, struct sockaddr *my_addr, int addrlen);


/**
 * \brief initiate a connection on a socket.
 * \param sockfd socket descriptor
 * \param serv_addr information about the destination port and IP address
 * \param addrlen set to sizeof(struct sockaddr).
 * \retval 0 success
 * \retval -1 error
 */
extern int connect(int sockfd, struct sockaddr *serv_addr, int addrlen);


/**
 * \brief listen for connections on a socket
 * \param sockfd socket descriptor
 * \param backlog number of connections allowed on the incoming queue (not used)
 * \retval 0 success
 * \retval -1 error
 * \remarks call bind() before calling listen()
 */
extern int listen(int sockfd, int backlog);
       

/**
 * \brief accept a connection on a socket
 * \param sockfd socket descriptor
 * \param addr ptr to remote socket address
 * \param addrlen set to sizeof(struct sockaddr).
 * \retval >=0 socket descriptor for the connection
 * \retval -1 error
 * \remarks call listen() before calling accept()
 */
extern int accept(int sockfd, struct sockaddr *addr, int *addrlen);


/**
 * \brief send application data
 * \param sockfd socket descriptor
 * \param msg data to be sent
 * \param len length of data to be sent (in bytes)
 * \param flags normally 0
 * \retval >=0 number of bytes acutally sent
 * \retval -1 error
 */
extern int send(int sockfd, void *msg, int len, int flags);

 
/**
 * \brief receive application data
 * \param sockfd socket descriptor
 * \param buf buffer to read the information into
 * \param len maximum length of the buffer (in bytes)
 * \param flags normally 0
 * \retval >=0 number of bytes acutally read
 * \retval -1 error
 * \remarks not implemented yet
 */
extern int recv(int sockfd, void *buf, int len, unsigned int flags);


/**
 * \brief send application data
 * \param sockfd socket descriptor
 * \param msg data to be sent
 * \param len length of data to be sent (in bytes)
 * \param flags normally 0
 * \param to remote IP address and port
 * \param tolen sizeof(struct sockaddr)
 * \retval >=0 number of bytes acutally sent
 * \retval -1 error
 * \remarks not implemented yet
 */
extern int sendto(int sockfd, void *msg, int len, unsigned int flags, struct sockaddr *to, int tolen);


/**
 * \brief receive application data
 * \param sockfd socket descriptor
 * \param buf buffer to read the information into
 * \param len maximum length of the buffer (in bytes)
 * \param flags normally 0
 * \param from local IP address and port
 * \param fromlen sizeof(struct sockaddr)
 * \retval >=0 number of bytes acutally read
 * \retval -1 error
 * \remarks not implemented yet
 */
extern int recvfrom(int sockfd, void *buf, int len, unsigned int flags, struct sockaddr *from, int *fromlen);


/** 
 * \example socket_ex.c
 * This is an example of how to use the socket module
 */

#endif /* _SYS_SOCKET_H */

/** @} */
/** @} */
