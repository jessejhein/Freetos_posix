/*	Copyright (C) 2004		cheng chung yan	yan@amonics.com> for 8-bit system
 *	lib C in 8 bit system
 *
 *	cirbuf.h
 *
 *	header of circular buffer
 *
 */

// return
//	255 -- full
//	not 255 -- new updated write pointer
// limitation
//	buffer size as 254 byte
extern unsigned char pre_wr_cir254buf(unsigned char wr, unsigned char rd, unsigned char max);
extern unsigned char pre_rd_cir254buf(unsigned char wr, unsigned char rd, unsigned char max);

