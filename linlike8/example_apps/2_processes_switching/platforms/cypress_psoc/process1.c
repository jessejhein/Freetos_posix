/*
 *	linlike8/test_apps/process1.c
 *
 *	linlike8 init task, it does to create others task
 *	but this task also can be one of task which will always running
 *
 *	16-04-2004	yan	just for test, real application should be have some job to do
 *
 */

#include "app.h"

#ifdef CYPRESS_PSOC
	#include <m8c.h>        // part specific constants and macros
	#include "PSoCAPI.h"    // PSoC API definitions for all User Modules
	#include "sched.h"
	#include "timer.h"
#if (SERIAL_MOD>0)
	#include "serial.h"								// read()
#endif
#if (I2C_MOD>0)
	#include "i2c.h"								// read()
#endif
#endif

#ifdef I386_LINUX
	#include <sched.h>
	#include <stdio.h>
	#include <unistd.h>
#endif

#ifdef CYPRESS_PSOC
extern unsigned char a_glo;
#endif
#ifdef I386_LINUX
extern unsigned char* shmaddr;
#define	a_glo *shmaddr
#endif


void process1(void)
{
	//while (1) sched_yield();
	unsigned char i = 0;
	unsigned char buf[3];
	//unsigned char buf_b[5];
	//unsigned char j = 0;

	while (1) {
#if (SERIAL_MOD>0)
		if (read(buf)) {
			buf[0] += 1;
			write(buf, 1);
			msleep(10);
		}
		
#endif
		
		i ^= 0xff;
		
		if (i) {
			a_glo += 2;
#ifdef CYPRESS_PSOC
			PRT2DR &= ~0x80;
#endif
#ifdef I386_LINUX
			printf("A\n");
			printf("(from P1) a_glo=%d \n", a_glo);
#endif
		} else {
#ifdef CYPRESS_PSOC
			PRT2DR |= 0x80;							// LED off
#endif
#ifdef I386_LINUX
			printf("B\n");
#endif
		}
		
#if ((SERIAL_MOD>0) && (I2C_MOD>0))
	
		for (j=0;j<4;j++)
			buf[j] = j+4;
		i2c_write(buf, 4, 0x01);
		
		sleep(1);
	
		if (i2c_read(buf_b, 4, 0x01)>0) write(buf_b, 4);
		
		sleep(1);
	
//	I2CHW_1_fReadBytes(1, buf_b, 4, 0);
//	while (!(I2CHW_1_bReadI2CStatus() & I2CHW_RD_COMPLETE));
//if (verify_buf(buf, buf_b, 4)==0) PRT0DR |= 0x80;
//else PRT0DR &= ~0x80;
//	I2CHW_1_ClrRdStatus();								// Clear the Read Status
		
#endif

		//sched_yield();
	}
}
