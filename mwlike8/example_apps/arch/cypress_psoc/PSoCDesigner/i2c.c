/*
 * Copyright (c) 10-06-2004 cheng chung yan <yan@amonics.com> / <chungyan5@hkem.com for 8-bit system
 *
 * this module is i2c in cypress psoc, driver specification. So each arch. has different code here
 * 
 * using cypress psoc i2c
 *
 * hardware configuration
 *	port 2.0, 2.2, 2.4, 2.6, ==> input with pull-down
 */

#include "app.h"									// this linlike8

#if (I2C_MOD==1)
#include <m8c.h>									// part specific constants and macros
#include "PSoCAPI.h"									// PSoC API definitions for all User Modules
	#if (AUTO_SW_MOD==0)
#include "sched.h"								// linlike8 system
#include "timer.h"
	#endif

#if (I2C_SLAVE==1)
unsigned char i2c_rx_buf[MAX_I2C_BUF];							// buffer for data from master to this slave
unsigned char i2c_tx_buf[MAX_I2C_BUF];							// buffer for data send to master from this slave
//unsigned char i2c_slave_addr;
#endif
#if (SWITCH_CASE==1)
unsigned char i2c_status;
#endif

void i2c_open(void)
{
	#if ((SWITCH_CASE==1) && (AUTO_ZERO==0))
	i2c_status = 0;
	#endif
	
	#if (I2C_SLAVE==1)
		#if (AUTO_ZERO==0)
	unsigned char i;
	for (i=0;i<MAX_I2C_BUF;i++)
		i2c_tx_buf[i] = 0;							// indicate empty buffer
		#endif
//	i2c_slave_addr = (PRT2DR & 0x55);						// dynamic to load the i2c slave address, valid only in 0101 0101
											// port 2 must set as pull-down
	I2CHW_1_EnableSlave();
	I2CHW_1_InitWrite(i2c_rx_buf,MAX_I2C_BUF);
	I2CHW_1_InitRamRead(i2c_tx_buf,MAX_I2C_BUF);
	#endif
	#if (I2C_MASTER==1)
	I2CHW_1_EnableMstr();
	#endif
	I2CHW_1_EnableInt();
}

#if (SWITCH_CASE==0)
unsigned char i2c_write(unsigned char *__buf, unsigned char __n
#if (I2C_MASTER==1)
, unsigned char slave_addr
#endif
)
{
	#if (I2C_MASTER==1)
	I2CHW_1_bWriteBytes(slave_addr, __buf, __n, 0);
	while (!(I2CHW_1_bReadI2CStatus() & I2CHW_WR_COMPLETE))				// Wait for the operation to complete
		#if (AUTO_SW_MOD==0)
	sched_yield()
		#endif
	;
	I2CHW_1_ClrWrStatus();								// Clear the Write Status
	return __n;
	#endif
	#if (I2C_SLAVE==1)
	unsigned char i = 0;
	for (;i<__n;i++)
		i2c_tx_buf[i] = *(__buf + i);
	while (I2CHW_1_bReadI2CStatus() & I2CHW_RD_COMPLETE)
		#if (AUTO_SW_MOD==0)
	sched_yield()
		#endif
	;
	I2CHW_1_ClrRdStatus();								// Clear the Read Complete Flag
	I2CHW_1_InitRamRead(i2c_tx_buf,MAX_I2C_BUF);					// Initialize Read Buffer for Master to read the response
	return i;
	#endif
}
#else
unsigned char i2c_write(unsigned char *__buf, unsigned char __n
#if (I2C_MASTER==1)
, unsigned char slave_addr
#endif
)
{
	#if (I2C_MASTER==1)
	switch (i2c_status) {
		case 0 : 
			 I2CHW_1_bWriteBytes(slave_addr, __buf, __n, 0);
			 i2c_status = 1;
			break;
		case 1 : 
			 if (I2CHW_1_bReadI2CStatus() & I2CHW_WR_COMPLETE) {
				 I2CHW_1_ClrWrStatus();
				 i2c_status = 0;
				 return __n;
			 }
			break;
	}
	return 0;
	#endif
	#if (I2C_SLAVE==1)
	unsigned char i = 1;
//			 i = 0								==> don't know where is the error, so handle by code
	switch (i2c_status) {
		case 0 : 
//			 for (;i<__n;i++)						err. as i2c_tx_buf[0,1,2] = 0x02, 0x08, 0x20
//				i2c_tx_buf[i] = *(__buf + i);				return as 0x20, 0x02. 0x08
			 i2c_tx_buf[__n-1] = *__buf;
			 for (;i<__n;i++)
				i2c_tx_buf[i-1] = *(__buf + i);
			 i2c_status = 1;
			break;
		case 1 : 
			 if (!(I2CHW_1_bReadI2CStatus() & I2CHW_RD_COMPLETE)) {
				I2CHW_1_ClrRdStatus();								// Clear the Read Complete Flag
				I2CHW_1_InitRamRead(i2c_tx_buf,MAX_I2C_BUF);					// Initialize Read Buffer for Master to read the response
				i2c_status = 0;
				return __n;
			 }
			break;
	}
	return 0;
	#endif
}
#endif

unsigned char i2c_read(unsigned char *__buf, unsigned char __n
#if (I2C_MASTER==1)
, unsigned char slave_addr
#endif
)
{
	#if (I2C_MASTER==1)
#if (SWITCH_CASE==0)
	I2CHW_1_fReadBytes(slave_addr, __buf, __n, 0);
	while (!(I2CHW_1_bReadI2CStatus() & I2CHW_RD_COMPLETE))
		#if (AUTO_SW_MOD==0)
	sched_yield()
		#endif
	;
	I2CHW_1_ClrRdStatus();								// Clear the Read Status
	return 1;
#else
	switch (i2c_status) {
		case 0 : 
			 I2CHW_1_fReadBytes(slave_addr, __buf, __n, 0);
			 i2c_status = 1;
			break;
		case 1 : 
			 if (I2CHW_1_bReadI2CStatus() & I2CHW_RD_COMPLETE) {
				 I2CHW_1_ClrRdStatus();								// Clear the Read Status
				 i2c_status = 0;
				 return __n;
			 }
			break;
	}
	return 0;
#endif
	#endif
	#if (I2C_SLAVE==1)
	unsigned char i = 0;
	if (I2CHW_1_bReadI2CStatus() & I2CHW_WR_COMPLETE) {
		for (; i<__n; i++)
			*(__buf + i) = i2c_rx_buf[i];
		I2CHW_1_ClrWrStatus();							// Clear the Write Complete Flag
		I2CHW_1_InitWrite(i2c_rx_buf,i);					// ReInitialize Write Buffer for next write from Master
	}
	return i;
	#endif
}

#endif


