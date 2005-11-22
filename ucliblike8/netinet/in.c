/*
 * in.c
 * 
 * yan@amonics.com
 * using license of GPL, same to linux
 * 
 */ 

// including, get data linking from others ===============================================
#include <asm/types.h>

// methods ==============================================================================

//	htonl
__u32 htonl(__u32 __hostlong)
{
	unsigned char* p = (unsigned char*) &__hostlong;
	return ( (( (__u32) *(p+0) )<<24)
		+ (( (__u32) *(p+1) )<<16)
		+ (( (__u32) *(p+2))<<8)
		+ ( (__u32) *(p+3)) );
}
