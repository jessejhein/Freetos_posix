/*
 *	linlike8/test_apps/process0.c
 *
 *	originally in linux as idle task with cpu sleep to save power.
 *	However, we don't want to waste the cpu power and stack size for this process in this small system(8-bit system is resource limitation, especially memory in rom and ram), 
 *	This should have something to do for system, such as monitoring in embbeded system.
 *	This process is always running, can't have sleep(means must always active), but lowest priority, so should do the task for always working but not significant
 *
 *	14-04-2004	yan	just for test, real application should be have some job to do
 *	25-05-2004	yan	change process0 as a system process
 *				adding the scan key procedures here combined with gpio module
 *	10-06-2004	yan	process0 still as a system process, it cannot be sleep, must always running.
 *				this process can do some time delay by alarm()
 *				remove the scan key from this process, since moving to gpio module
 *
 */

#include "app.h"

#ifdef CYPRESS_PSOC
	#include "sched.h"								// linlike8 system
	#include "timer.h"
	#include <m8c.h>								// application 
	#include "PSoCAPI.h"
#if (SERIAL_MOD>0)
	#include "serial.h"								// read()
#endif
#endif

#ifdef I386_LINUX
	#include <sched.h>
	#include <stdio.h>
	#include <unistd.h>
#endif

#ifdef CYPRESS_PSOC
unsigned char a_glo = 0;
#endif
#ifdef I386_LINUX
extern unsigned char* shmaddr;
#define	a_glo *shmaddr
#endif

unsigned char test_alarm_i;
void test_alarm(void)
{
	test_alarm_i = 1;
}

void process0(void)
{
	unsigned char i = 0;
	struct timer_list proc0_timer;
	
	//Counter8_1_WritePeriod(0x07);
	//Counter8_1_WriteCompareValue(0x03);
	//Counter8_1_EnableInt();
	Counter8_1_Start();
	
	while (1) {									// must always looping, DO NOT have sleep()
		
		// appl.
		//******************************************
		i ^= 0xff;
		
		if (i) {
			a_glo += 1;
#ifdef CYPRESS_PSOC
			//PRT4DR &= ~0x80;
			PRT2DR &= ~0x08;
#endif
#ifdef I386_LINUX
			printf("1\n");
			printf("(from P0) a_glo=%d \n", a_glo);
#endif
		} else {
#ifdef CYPRESS_PSOC
			PRT2DR |= 0x08;							// LED off
			//PRT4DR |= 0x80;
			//PRT2DR |= 0x08;
#endif
#ifdef I386_LINUX
			printf("2\n");
#endif
		}
		
		//PRT1DR &= ~0xfc;
		
		
		test_alarm_i = 0;
		proc0_timer.data.timer_function = (p_func) test_alarm;//after solve the problem, put back into alarm()
		alarm((p_func) &test_alarm, 1, &proc0_timer);
		while (test_alarm_i == 0) {
			PRT1DR &= ~0xff;
			sched_yield();
			/*PRT1DR |= 0x01;			// 0.34uSec
			PRT1DR |= 0x01;
			PRT1DR |= 0x01;
			PRT1DR |= 0x01;
			PRT1DR |= 0x01;
			PRT1DR |= 0x01;
			PRT1DR |= 0x01;
			*/
		}
	}
}

