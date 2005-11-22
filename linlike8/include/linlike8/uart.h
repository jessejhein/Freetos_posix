/*
 * Copyright (c) 24-05-2004 cheng chung yan <yan@amonics.com> / <chungyan5@hkem.com for 8-bit system
 *
 * header of serial
 *
 */
 
//	parameters ===========================================================
//		keep in %ROOT_DIR%/config.h

//	methods ===========================================================

//		init. uart module

//	define structure for the uart_write_complete
struct WR_COMPL_VAR{
	unsigned tmp	:7;
	unsigned state	:1;
};

extern void uart_open(void);

//		return : how many byte you send/receive
//		Remarks : write() -- can't always be called, since cpu always faster than uart
extern unsigned char uart_write(unsigned char *__buf, unsigned char __n,unsigned char which,unsigned char uart);

//		<rec 1 byte,remove>extern unsigned char read(unsigned char *__buf, unsigned char __n);
extern unsigned char uart_read(unsigned char *__buf, unsigned char which); //<rec 1 byte,add>

//		uart write, and make sure it must completes for you when return
#if (SWITCH_CASE==0)
	extern void uart_write_complete(unsigned char *__buf, unsigned char __n, unsigned char which,unsigned char uart);
#endif
#if (SWITCH_CASE==1)
//		Remarks :
//			return the numer of bytes that transmitted 
//			size of data to write must be limited =<128 bytes
extern unsigned char uart_write_complete(unsigned char *__buf, unsigned char __n,struct WR_COMPL_VAR *pa,unsigned char which,unsigned char uart);
#endif
