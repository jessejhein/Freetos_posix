/*
 * arpa/inet.h
 * 
 * Defines prototypes for those network library routines that convert Internet address and 
 * dotted-decimal notation, for example, inet_makeaddr().
 */

#ifndef _ARPA_INET_H
#define _ARPA_INET_H    1

#include <netinet/in.h>

/******************************************************************************
 * Function:        int inet_aton(char *name, struct in_addr *addr)
 * 
 * Description:     converts the Internet host address name from the standard 
 *                  numbers-and-dots notation into binary data and stores it in 
 *                  the struct in_addr that addr points to.
 * 
 * Input:           *name: ip address in string format
 *                  *addr: ip address in binary format
 * 
 * Output:          0, not valid
 *                  1, valid
 ******************************************************************************/
extern int inet_aton(char *name, struct in_addr *addr);


/******************************************************************************
 * Function:        char* inet_ntoa(struct in_addr addr)
 * 
 * Description:     converts the Internet host address addr to a string in the 
 *                  standard numbers-and-dots notation. 
 * 
 * Input:           *addr: ip address in binary format
 * 
 * Output:          a pointer into a statically-allocated buffer.
 ******************************************************************************/
extern char* inet_ntoa(struct in_addr addr);

#endif /* arpa/inet.h */

