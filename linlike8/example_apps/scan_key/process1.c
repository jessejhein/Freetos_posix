/*
 *	linlike8/test_apps/process1.c
 *
 *	linlike8 init task, it does to create others task
 *	but this task also can be one of task which will always running
 *
 *	16-04-2004	yan	just for test, real application should be have some job to do
 *	03-05-2004	yan	for General testing 
 *	04-05-2004	yan	key scanning, ONE key pin is pull-high, and active in GND, P3.3 
 *
 *	ToDo
 *		may be better to change to library, just modify some parameters 
 */

#define CYPRESS_PSOC

#include "app.h"
#ifdef CYPRESS_PSOC
	#include <m8c.h>        // part specific constants and macros
	#include "PSoCAPI.h"    // PSoC API definitions for all User Modules
	#include "sched.h"
	#include "timer.h"
#endif

#ifdef I386_LINUX
#endif

//	o Vcc
//	|
//	R
//	|	-----
//  o---+-------o   o---+
// cpu port		|
//			V Gnd
//
//  ----+++		+++---------------------+++----------
//	|||		|||			|||
//	+++-------------+++ - - - - - - - - - - +++
//	A  B		   D			C
//	return
//		1 - as key valid
//		0 - no key
#define KEY_RESTORE		PRT3DR |= 0x08
#define KEY_DETECT		!(PRT3DR & 0x08)

#define MAX_KEY_BUF	10
unsigned char key_buf[MAX_KEY_BUF];
unsigned char p_wr_key_buf = 0;
unsigned char p_rd_key_buf = 0;

// this will over-written the old value
void put_key_buf(unsigned char key_data)
{
	key_buf[p_wr_key_buf] = key_data;
	p_wr_key_buf = (p_wr_key_buf == MAX_KEY_BUF) ? 0 : p_wr_key_buf + 1;
}

unsigned char get_key_buf(unsigned char* p_key_data)
{
	if (p_rd_key_buf!=p_wr_key_buf) {
		*p_key_data = key_buf[p_rd_key_buf];
		p_rd_key_buf = (p_rd_key_buf == MAX_KEY_BUF) ? 0 : p_rd_key_buf + 1;
		return 1;
	}
	return 0;
}

void scan_key(void)
{
	unsigned char i;
	while (1) {
		if (KEY_DETECT) {								// point A
			msleep(10);
			KEY_RESTORE;
			if (KEY_DETECT) {							// point B
				put_key_buf(1);
				for (i=0;i<10;i++) {
					msleep(50);
					KEY_RESTORE;
					if (!KEY_DETECT) break;					// point D
				}
				// if (i == 10), it is point C, loop back to start point and detect key for repeated key
			}
		}
	}
}

