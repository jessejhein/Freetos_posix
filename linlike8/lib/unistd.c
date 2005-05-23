/*
 * Copyright (c) 2004 cheng chung yan <yan@amonics.com> for 8-bit system running in linlike8
 *
 * standard symbolic constants and types
 */

// including, get data linking from others
//==============================================================
//	linlike8 configuration
#include "config.h"
//	this lib
#include <unistd.h>
//	uart
#include "serial.h"

// methods
//========================================================================================

//	open =================================================================================
char open(char what_device)
{
	// do nothing, linlike8 already opens all devices when booting
	return what_device;
}

//	write ================================================================================
unsigned char write(char fd, unsigned char* ptr_pkg, unsigned char __n )	
{
	switch (fd) {
		case UART0 : 
			return uart_write_complete(ptr_pkg, __n);
//			 ** MUST modify serial_write_complete() has same return to write() in posix **
//			 i.e. -1 is error, 0 1 2 ... the number of byte tx 
			break;
		case 2 :
		 #if (I2C_MOD>0)		
			return i2c_write_complete(ptr_pkg, __n
			#if (I2C_MASTER==1||I2C_MULTI_MASTER==1)								//for the multi_master application
			,i2c_slave_address
			#endif
			);
		#endif
			 // return I2C write in master or slave, should be return number of bytes tx
			break;
	}
	return 0;
}

//	read =================================================================================
unsigned char read (unsigned char fd, unsigned char* ptr_pkg, unsigned char __n)	
{
	switch (fd) {
		 case UART0 : 
			return uart_read(ptr_pkg);
			 //** MUST modify read() has same return to read() in posix **
			 //i.e. -1 is error, 0 1 2 ... the number of byte tx 
			 //** and .... ***
			 //since the read() only receive 1 byte each time therefore when the return value is 1 it is completed
			break;
		 case 2 :
		#if (I2C_MOD>0)
			return i2c_read(ptr_pkg, __n							//do not use the i2c_read_complete()
			#if (I2C_MASTER==1||I2C_MULTI_MASTER==1)
			,i2c_slave_address
			#endif
			);
		#endif
			 // return I2C write in master or slave, should be return number of bytes tx
			break;
	}
	return 0;
}
