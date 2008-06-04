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
extern int inet_aton(const char *name, struct in_addr *addr);


/**
 * \brief converts the Internet host address addr to a string in the standard 
 *        numbers-and-dots notation. 
 * \param addr ip address in binary format (e.g. C0 A8 01 01)
 * \return a pointer into a statically-allocated buffer (e.g. 192.168.1.1)
 */
extern char* inet_ntoa(struct in_addr addr);

#endif /* arpa/inet.h */

/** @} */
/** @} */
