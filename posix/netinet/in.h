/*
 * netinet/in.h
 * 
 * Defines prototypes, macros, variables, and the sockaddr_in structure to use 
 * with Internet domain sockets.
 */

#ifndef _NETINET_IN_H
#define _NETINET_IN_H   1

#include <asm/types.h>

#ifdef ETHERNET_MOD
#include <uip.h>
#endif


/* 
 * Standard well-defined IP protocols, for creating socket()
 */
#define IPPROTO_IP      0           //Dummy protocol for TCP
#define IPPROTO_TCP     6           //Transmission Control Protocol
#define IPPROTO_UDP     17          //User Datagram Protocol


typedef unsigned char in_addr_t[4];
/*
 * Structure:   in_addr
 * 
 * Member:      s_addr:     ip address (network byte order)
 * 
 */
struct in_addr
{
    in_addr_t s_addr;
};


/*
 * Function pointer type to a function in the form of
 * void myFunc (void) which is used within uip_app_register
 */
typedef void    (*eth_appcall_t)(void);


/*
 * Structure:   sockaddr_in
 * 
 * Member:      sin_len:
 *              sin_family: address family (e.g. AF_INET)
 *              sin_port:   port number (network byte order)
 *              sin_addr:   ip address (network byte order)
 *              sin_zero:   reserved
 * 
 */
struct sockaddr_in 
{
    //unsigned char sin_len;
    unsigned short sin_family;     //Address family
    unsigned short sin_port;       //Port number
    struct in_addr sin_addr;        //Internet or IP address
    union{
        char sin_zero[8];           //Same size as struct sockaddr
#ifdef ETHERNET_MOD
        eth_appcall_t appcall;      //appcall for uip
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
