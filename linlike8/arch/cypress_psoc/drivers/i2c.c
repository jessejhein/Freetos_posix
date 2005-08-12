/*
 * Copyright (c) 10-06-2004 cheng chung yan <yan@amonics.com> / <chungyan5@hkem.com for 8-bit system
 *
 * this module is i2c in cypress psoc, driver specification. So each arch. has different code here
 * 
 * using cypress psoc i2c
 *
 * hardware configuration
 *	port 2.0, 2.2, 2.4, 2.6, ==> input with pull-down
 *	3-11-2004	hei		modified for the i2c adding i2c_write_complete and i2c_read_complete
 						remark: the position error have been corrected by using the same code that yan has used before
 *	5-11-2004	Hei		modified for the return number of the i2c_complete_read and i2c_complete_write so that the return number would be the number of bytes transmitted and received
 *	9-12-2004	Hei 	modified for the transmition of the i2c, the data transmit for the i2c would be contain the length of the data including the data length itself.
 						Ex.if the data transmit is {0x01,0x03,0x02}, it would modified into {0x04,0x01,0x03,0x02} where 0x04 is the length of the data. After receiving the data, it would decode it into the orginial data.
 *						To do this modification, because in the application, it would only read 1 byte for each time however in the past application, after read 1 byte from the buffer of the i2c, it would clear the buffer. To solve this problem, the length of data is added into the modified data so that the receiver can know the length of data.
 						remark: it is not recommed to use the i2c_read_complete in the comm.c apllication, it would make the system handed since the i2c_read_complete need to read something, if there is nothing to read, the system would be handed.
 */

// including, get data linking from others ===============================================
//	this appl. layer
#include <pin_define.h>
//	linlike8 configuration
#include <linlike8/config.h>
#include "app.h"									// this linlike8

#if (I2C_MOD==1)
#include "comm.h"
#include <m8c.h>									// part specific constants and macros
#include "PSoCAPI.h"									// PSoC API definitions for all User Modules
	#if (AUTO_SW_MOD==0)
#include <linlike8/sched.h>									// timer
#include <linlike8/timer.h>
	#endif
#define MAX_DELAY	50											//delay for the write so that the receiver has enough time to analyse the data
#define MAX_I2C_BUF	10
unsigned char i2c_rx_buf[MAX_I2C_BUF];							// buffer for data from master to this slave
unsigned char i2c_tmp_buffer[MAX_I2C_BUF];						//buffer for the data store from the master temperately for the slave
unsigned char i2c_tx_buf[MAX_I2C_BUF];							// buffer for data send to master from this slave
//unsigned char i2c_slave_addr;

unsigned char i2c_data_length;														//the length of the data
unsigned char i2c_read_status = 0;
unsigned char i2c_write_status = 0;
unsigned char k = 1;
int delay = 0;															//to give the delay to the i2c write such that there is enough time for the i2c read to finish it work and read another data from the i2c write

#if (SWITCH_CASE==1)
unsigned char i2c_status;
struct compl_struct {
	unsigned tmp	:7;
	unsigned state	:1;
} compl_var;
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

// To make sure that the receiver has enough time to analyse the data, there is a dalay for each writing process and the delay time 
// is determine by the MAX_DELAY
// remark :	the delay mey be change into (__n+1)*MAX_DELAY so that for long data, it would give more time for the receiver to analyse the data. Try later.

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
	
	/* un-tested code for multi-master
	unsigned char i = 0;
	#if (I2C_MASTER==1)
switch (i2c_write_status)	{
	case 0:
		delay++;
		if (delay==MAX_DELAY)	{									//the delay should be added, if no, the error may be occured
			delay = 0;
			i2c_write_status=1;
			}
		break;	
	case 1:
		i2c_tx_buf[i]=__n+1;												//the first byte of the i2c will be the length of the data
		for (;i<__n;i++)
			i2c_tx_buf[i+1] = *(__buf + i);
		I2CHW_1_bWriteBytes(slave_addr, i2c_tx_buf, MAX_I2C_BUF, 0);
	#if (I2C_MULTI_MASTER==1)
		while (!(I2CHW_1_bReadMasterStatus() & I2CHW_WR_COMPLETE))
	#else
		while (!(I2CHW_1_bReadI2CStatus() & I2CHW_WR_COMPLETE))				// Wait for the operation to complete
	#endif		
			#if (AUTO_SW_MOD==0)
				sched_yield()
			#endif
				;
	#if (I2C_MULTI_MASTER==1)
		I2CHW_1_ClrMasterStatus();
	#else
		I2CHW_1_ClrWrStatus();								// Clear the Write Status
	#endif
		i2c_write_status=2;
		break;
	case 2:
		delay++;
		if (delay==MAX_DELAY)	{									//if the data of tx_pkg have been increased, the number of delay may need to incerase
			delay = 0;
			i2c_write_status=3;
			}
		break;
	case 3:
		i2c_write_status=0;
		return __n;
	}
	#endif
	#if (I2C_SLAVE==1)
	switch (i2c_write_status)	{
	case 0:
		i2c_write_status=1;
		break;	
	case 1:
		i2c_tx_buf[i]=__n+1;												//the first byte of the i2c will be the length of the data
			for (;i<__n;i++)
				i2c_tx_buf[i+1] = *(__buf + i);
			while (I2CHW_1_bReadI2CStatus() & I2CHW_RD_COMPLETE)
			#if (AUTO_SW_MOD==0)
				sched_yield()
			#endif
				;
		I2CHW_1_ClrRdStatus();								// Clear the Read Complete Flag
		I2CHW_1_InitRamRead(i2c_tx_buf,MAX_I2C_BUF);					// Initialize Read Buffer for Master to read the response
		i2c_write_status=2;
		break;
	case 2:
		i2c_write_status=0;
		return i;
		break;	
	}
	#endif
	return 0;*/
}
#else
// To make sure that the receiver has enough time to analyse the data, there is a dalay for each writing process and the delay time 
// is determine by the MAX_DELAY
// remark :	the delay mey be change into (__n+1)*MAX_DELAY so that for long data, it would give more time for the receiver to analyse the data. Try later.
// for the slave trasmition, it would not give delay to it, since the refrash of data in the slave is controled by the master-read. 
unsigned char i2c_write(unsigned char *__buf, unsigned char __n
#if (I2C_MASTER==1)
, unsigned char slave_addr
#endif
)
{
	unsigned char i = 0;
	switch (i2c_write_status)	{
	case 0:
		delay++;
		if (delay==MAX_DELAY)	{									//the delay should be added, if no, the error may be occured
			delay = 0;
			i2c_write_status=1;
			}
		break;	
	case 1:
		#if (I2C_MASTER==1)
		switch (i2c_status) {
		case 0 : 
			 i2c_tx_buf[i]=__n+1;												//the first byte of the i2c will be the length of the data
			 	for (;i<__n;i++)
			 		i2c_tx_buf[i+1] = *(__buf + i);
				I2CHW_1_bWriteBytes(slave_addr, i2c_tx_buf, MAX_I2C_BUF, 0);
			 i2c_status = 1;
			break;
		case 1 : 
	#if (I2C_MULTI_MASTER==1)
			if (I2CHW_1_bReadMasterStatus() & I2CHW_WR_COMPLETE) {
			 	I2CHW_1_ClrMasterWrStatus();
	#else
			if (I2CHW_1_bReadI2CStatus() & I2CHW_WR_COMPLETE) {
			 	I2CHW_1_ClrWrStatus();
	#endif
			 
				 i2c_status = 0;
				 i2c_write_status=2;
			 	 }
			 	 break;
		}	// end of switch (i2c_status)
			break;
		#endif
		#if (I2C_SLAVE==1)
//			 i = 0								==> don't know where is the error, so handle by code
		switch (i2c_status) {
		case 0 : 
//			 for (;i<__n;i++)						err. as i2c_tx_buf[0,1,2] = 0x02, 0x08, 0x20
//				i2c_tx_buf[i] = *(__buf + i);				return as 0x20, 0x02. 0x08
			 //i2c_tx_buf[__n] = *__buf;
			 i2c_tx_buf[i]=__n+1;												//the first byte of the i2c will be the length of the data
			 for (;i<__n;i++)
			 i2c_tx_buf[i+1] = *(__buf + i);
			 i2c_status = 1;
			break;
		case 1 : 				
			 if (I2CHW_1_bReadI2CStatus() & I2CHW_RD_COMPLETE) {
			 	I2CHW_1_ClrRdStatus();								// Clear the Read Complete Flag
				I2CHW_1_InitRamRead(i2c_tx_buf,MAX_I2C_BUF);					// Initialize Read Buffer for Master to read the response
				i2c_status = 0;
				i2c_write_status=3;												//the slave do not need to delay so jump to case 3
			 	}
			break;
		}
		break;
		#endif
		case 2:
			delay++;
			if (delay==(MAX_DELAY))	{									//if the data of tx_pkg have been increased, the number of delay may need to incerase
				delay = 0;
				i2c_write_status=3;
				}
			break;
		case 3:
			i2c_write_status=0;
			return __n;
	}
	return 0;
}
#endif

// since the data is modified, there is data length in the first byte of the data, so the read function. First determine the length of the data and modified to the data into orginial data.

unsigned char i2c_read(unsigned char *__buf, unsigned char __n
#if (I2C_MASTER==1)
, unsigned char slave_addr
#endif
)
{
	unsigned char i = 0;
	int i_delay;
	
#if (I2C_MASTER==1)
	#if (SWITCH_CASE==0)
	switch (i2c_read_status) {
		case 0:
			I2CHW_1_fReadBytes(slave_addr, i2c_tmp_buffer, MAX_I2C_BUF, 0);
		#if (I2C_MULTI_MASTER==1)
			while (!(I2CHW_1_bReadMasterStatus() & I2CHW_RD_COMPLETE))
		#else
			while (!(I2CHW_1_bReadI2CStatus() & I2CHW_RD_COMPLETE))
		#endif
				{
		#if (AUTO_SW_MOD==0)
					sched_yield()
		#endif
					;
		#if (I2C_MULTI_MASTER==1)
					I2CHW_1_ClrMasterRdStatus();								// Clear the Read Status
		#else
					I2CHW_1_ClrRdStatus();								// Clear the Read Status
		#endif
					i2c_data_length=i2c_tmp_buffer[0];
					i2c_read_status = 1;
				}
			break;
		case 1:
			for (;i<__n;i++) {
				*(__buf+i)=i2c_tmp_buffer[k];						//read from the tmp_buffer
				k++;		
				if (k>=i2c_data_length)	{
					i2c_read_status = 0;
					k=1;
					return i+1;
				}
			}
		return __n;
	}
	return 0;
#else	// else of #if (I2C_MASTER==1)
	switch (i2c_read_status) {
		case 0:	
			i2c_read_status=1;
		case 1:
			switch (i2c_status)	{
					case 0:
			 			I2CHW_1_fReadBytes(slave_addr, i2c_rx_buf, MAX_I2C_BUF, 0);
			 			i2c_status = 1;
			 			break;
			 		case 1:
			 			#if (I2C_MULTI_MASTER==1)
							if (I2CHW_1_bReadMasterStatus() & I2CHW_RD_COMPLETE) {
						#else
							if (I2CHW_1_bReadI2CStatus() & I2CHW_RD_COMPLETE) {
						#endif
			 					if (i2c_rx_buf[0]>0)	{
			 						i2c_data_length=i2c_rx_buf[0];
			 						for (i=0;i<i2c_data_length; i++)	{
				 						i2c_tmp_buffer[i] = i2c_rx_buf[i];
										}
									i2c_read_status = 2;
								}
							#if (I2C_MULTI_MASTER==1)
								I2CHW_1_ClrMasterRdStatus();
							#else
								I2CHW_1_ClrRdStatus();							// Clear the Read Status
							#endif
							i2c_status = 0;
				 			}
						 break;
					}
		break;
case 2:
	for (;i<__n;i++)	{
		*(__buf+i)=i2c_tmp_buffer[k];						//read from the tmp_buffer
		k++;
		if (k>=i2c_data_length)	{
		i2c_read_status = 0;
		k=1;
		return i+1;
		}		
	}
	return __n;
}
return 0;
#endif
	#endif
	#if (I2C_SLAVE==1)
switch (i2c_read_status)	{
case 0:
	if (I2CHW_1_bReadI2CStatus() & I2CHW_WR_COMPLETE) {
		i2c_data_length=i2c_rx_buf[0];
		for (;i<i2c_data_length; i++)	{
		i2c_tmp_buffer[i] = i2c_rx_buf[i];
		}
		I2CHW_1_ClrRdStatus();								// Clear the Read Status
		I2CHW_1_InitWrite(i2c_rx_buf,MAX_I2C_BUF);					// ReInitialize Write Buffer for next write from Master
		i2c_read_status=1;
	}
	break;
case 1:		
	for (;i<__n;i++)	{
		*(__buf+i)=i2c_tmp_buffer[k];						//read from the tmp_buffer
		k++;
		if (k>=i2c_data_length)	{
		i2c_read_status = 0;
		k=1;
		return i+1;
		}		
	}
	return __n;
}
return 0;
#endif	
}

#if (SWITCH_CASE==0)
void i2c_write_complete(unsigned char *__buf, unsigned char __n
	#if (I2C_MASTER==1)
, unsigned char slave_addr
	#endif
}
{
	// non-tested
	unsigned char i = 0;
	do {
		i += i2c_write(__buf + i, __n - i
	#if (I2C_MASTER==1)
		, unsigned char slave_addr
	#endif);
		if (i==__n) break;
		else msleep(10);
	} while (1);
}
#else
unsigned char i2c_write_complete(unsigned char *__buf, unsigned char __n
	#if (I2C_MASTER==1)
, unsigned char slave_addr
	#endif
)
{
	switch (compl_var.state) {
		case 0 :
			 compl_var.tmp = 0;
			 compl_var.state = 1;
			break;
		case 1 :
			 compl_var.tmp += i2c_write(__buf + compl_var.tmp, __n - compl_var.tmp
			 #if (I2C_MASTER==1)
			,slave_addr
			#endif
			);
			 if (compl_var.tmp>=__n){
			 compl_var.state = 0;
			 return compl_var.tmp;
			 }
			break;
	}
	return 0;
}
#endif

#if (SWITCH_CASE==0)
void i2c_raed_complete(unsigned char *__buf, unsigned char __n
	#if (I2C_MASTER==1)
, unsigned char slave_addr
	#endif
}
{
	// non-tested
	unsigned char i = 0;
	do {
		i += i2c_read(__buf + i, __n - i
	#if (I2C_MASTER==1)
		,slave_addr
	#endif);
		if (i==__n) break;
		else msleep(10);
	} while (1);
}
#else	// (SWITCH_CASE==0)
unsigned char i2c_read_complete(unsigned char *__buf, unsigned char __n
	#if (I2C_MASTER==1)
, unsigned char slave_addr
	#endif
)
{
	switch (compl_var.state) {
		case 0 :
			 compl_var.tmp = 0;
			 compl_var.state = 1;
			break;
		case 1 :
			 compl_var.tmp += i2c_read(__buf + compl_var.tmp, __n - compl_var.tmp
			 #if (I2C_MASTER==1)
			,slave_addr
			#endif
			);
			 if (compl_var.tmp>=__n) {
			 compl_var.state = 0;
			 return __n;
			 }
			break;
	}
	return 0;
}
#endif	// (SWITCH_CASE==0)
#endif
