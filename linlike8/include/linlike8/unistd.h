/*
 *	header of Symbolic Constants
 *	
 *	include/linlike8/unistd.h
 *
 *	Symbolic Constants
 *
 *	26-04-2004	yan	create this following linux
 *
 */

//	data ===========================================================
//		file handling number of uart0
#define	UART0	0
#define	UART1	1
#define	I2C0	2

//	methods ===========================================================

//	serial open ==========================================================================
//	parameters
//		what_device -- which physical device to use: UART0, UART1, I2C0
//	return
//		return the file descriptor, then let read/write to use this file descriptor
extern char open(char what_device);

//	serial write
//	parameters
//		fd -- open() init. this file descriptor, write() will write data into this file descriptor
//	return
//		number of bytes actually written to the file associated with fildes
extern unsigned char write(char fd, unsigned char* ptr_pkg, unsigned char __n);

//	serial write
//	return
//		number of bytes actually read from the file associated with fildes
extern unsigned char read (unsigned char fd, unsigned char* ptr_pkg, unsigned char __n);

//		
extern unsigned char sleep(unsigned char seconds);

