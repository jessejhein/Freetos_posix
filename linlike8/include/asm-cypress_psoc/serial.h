/*
 * Copyright (c) 24-05-2004 cheng chung yan <yan@amonics.com> / <chungyan5@hkem.com for 8-bit system
 *
 * header of serial
 *
 */

// return : how many byte you send/receive
// Remarks : write() -- can't be always be called, since cpu always faster than uart
extern unsigned char write(unsigned char *__buf, unsigned char __n);
//<rec 1 byte,remove>extern unsigned char read(unsigned char *__buf, unsigned char __n);
extern unsigned char read(unsigned char *__buf);//<rec 1 byte,add>
#if (SWITCH_CASE==0)
extern void serial_write_complete(unsigned char *__buf, unsigned char __n);
#else
// Return : 1 as not completed, 0 as completed
// Remarks : 
//	size of data to write mu be limited =<128 bytes
extern unsigned char serial_write_complete(unsigned char *__buf, unsigned char __n);
#endif

