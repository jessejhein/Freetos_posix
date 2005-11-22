/*
 * in.h
 * 
 * following linux libc, big-endian, little-endian converter
 * run under linlike8 8 bit system
 * 
 * yan@amonics.com
 * using license of GPL, same to linux
 * 
 */ 

#ifndef _NETINET_IN_H
#define _NETINET_IN_H   1

// including, get data linking from others ===============================================
#include <asm/types.h>

// methods ==============================================================================

//	htonl
//		converts the 32 bit data from little-endian to big-endian
extern __u32 htonl(__u32 __hostlong);

#endif  /* netinet/in.h */
