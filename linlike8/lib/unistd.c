/*
 * Copyright (c) 2004 cheng chung yan <yan@amonics.com> for 8-bit system running in linlike8
 *
 * standard symbolic constants and types
 */

// including, get data linking from others
//==============================================================
//	linlike8 configuration
#include <linlike8/config.h>
#if (UART_MOD>0)		
//	uart
#include "uart.h"
#endif
#if (I2C_MOD>0)		
//	i2c
#include "i2c.h"
#endif
//	this lib
#include <unistd.h>

// data
//==============================================================

//	I2C variables
#if (I2C_MOD==1||UART_MOD==1)
unsigned char	slave_address; 
#endif

// methods
//========================================================================================

//	open =================================================================================
char open(char what_device)
{
	// do nothing, linlike8 already opens all devices when booting
	return what_device;
}

//	io ctrl ==============================================================================
#if (I2C_MOD==1||UART_MOD==1)
unsigned char ioctl(unsigned char device, unsigned char flag, unsigned char data)
{
	switch (device) {
		 case I2C0 : 
			 switch (flag) {
				 case 1 :
				 	slave_address = data;
				 	break;
			 }
			break;
		case UART1 : 
			 switch (flag) {
				 case 1 :
				 	slave_address = data;
				 	break;
			 }
			break;
	}
	return 0;
}
#endif

//	write ================================================================================
unsigned char write(char fd, unsigned char* ptr_pkg, unsigned char __n, struct WR_COMPL_VAR *tmp_uart)
{
#if ((UART_MOD>0)||(I2C_MOD>0))
	switch (fd) {
#if (UART_MOD>0)		
		case UART0 : 
			return uart_write_complete(ptr_pkg, __n, tmp_uart,UART0,UART0);
//			 ** MUST modify serial_write_complete() has same return to write() in posix **
//			 i.e. -1 is error, 0 1 2 ... the number of byte tx 
			break;
		case UART1 : 
			return uart_write_complete(ptr_pkg, __n, tmp_uart,UART1,UART1);
//			 ** MUST modify serial_write_complete() has same return to write() in posix **
//			 i.e. -1 is error, 0 1 2 ... the number of byte tx 
			break;
#endif
		 #if (I2C_MOD>0)		
		case I2C0 :
			return i2c_write_complete(ptr_pkg, __n
			#if (I2C_MASTER==1||I2C_MULTI_MASTER==1)								//for the multi_master application
			,slave_address
			#endif
			);
			 // return I2C write in master or slave, should be return number of bytes tx
			break;
		#endif
	}
#endif
	return 0;
}

//	read =================================================================================
unsigned char read (unsigned char fd, unsigned char* ptr_pkg, unsigned char __n)	
{
#if ((UART_MOD>0)||(I2C_MOD>0))
	switch (fd) {
		#if (UART_MOD>0)		
		 case UART0 : 
			return uart_read(ptr_pkg,UART0);
			 //** MUST modify read() has same return to read() in posix **
			 //i.e. -1 is error, 0 1 2 ... the number of byte tx 
			 //** and .... ***
			 //since the read() only receive 1 byte each time therefore when the return value is 1 it is completed
			break;
		case UART1 : 
			return uart_read(ptr_pkg,UART1);
			 //** MUST modify read() has same return to read() in posix **
			 //i.e. -1 is error, 0 1 2 ... the number of byte tx 
			 //** and .... ***
			 //since the read() only receive 1 byte each time therefore when the return value is 1 it is completed
			break;
		#endif
		#if (I2C_MOD>0)
		 case I2C0 :
			return i2c_read(ptr_pkg, __n							//do not use the i2c_read_complete()
			#if (I2C_MASTER==1||I2C_MULTI_MASTER==1)
			,slave_address
			#endif
			);
			 // return I2C write in master or slave, should be return number of bytes tx
			break;
		#endif
	}
#endif
	return 0;
}
