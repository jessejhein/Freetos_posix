/*
 * Copyright (c) 1999, 2000, 2001, 2003 Greg Haerr <greg@censoft.com>
 * Portions Copyright (c) 2002 by Koninklijke Philips Electronics N.V.
 * Copyright (c) 1991 David I. Bell
 * Copyright (c) 2004 cheng chung yan <chungyan5@hkem.com / yan@amonics.com> for 8-bit system running in linlike8
 * Permission is granted to use, distribute, or modify this source,
 * provided that this copyright notice remains intact.
 *
 * Main module of graphics server.
 */

#include "app.h"									// linlike8 os

#include "sched.h"
#include "nano-X.h"									// NR_EVENT
#include "device.h"									// scr_open()
#include "timer.h"

extern void io_open(void);

void mwlike8_server(void)
{
	
#if (KB_MOD==1)
	// keyboard events
	//=========================================================
	unsigned char i;
#endif

		// init this process
#if (GUI_LCD==1)
	scr_open();									// hardware init.
#endif
	io_open();
	
#if (AUTO_ZERO==0)
	unsigned char k;								// mwlike8 server init.
	wr_ptr_events_vect = 0;
	rd_ptr_events_vect = 0;
	for (k=0;i<NR_EVENT;k++) events_vect[i].type = GR_EVENT_TYPE_NONE;
#endif

		// main job of this process
		//	detect user input
	while (1) {
		
		// linlike8 system
		//******************************************
		
		// appl.
		//******************************************
		
		// system context switch
		//******************************************
		sched_yield();								// any time should switch away to let other process to run, unless somethign need to run in this process0
	}
}


/*
#ifdef CYPRESS_PSOC
#endif
#ifdef I386_LINUX
	#include <sched.h>
#endif


*/

/*
#include "nano-X.h"
//#include "timer.h"
//#include <unistd.h>
#define MAX_NUM_EVENT_QUEUE	1
unsigned char event_queue[MAX_NUM_EVENT_QUEUE];
unsigned char p_wr_event_queue = 0;
unsigned char p_rd_event_queue = 0;

// later to remove this code to lib general. then if other code need this get and wr again, we can reduce code
// change to
//	unsigned char wr_event_queue(unsigned char event_type, unsigned char wr_ptr, unsigned char rd_ptr, unsigned char* ptr_buf)
//	unsigned char rd_event_queue(unsigned char* ptr_event_type, unsigned char wr_ptr, unsigned char rd_ptr, unsigned char* ptr_buf))
unsigned char wr_event_queue(unsigned char event_type)
{
	unsigned char tmp = (p_wr_event_queue == MAX_NUM_EVENT_QUEUE) ? 0 : p_wr_event_queue + 1;
	if ( tmp == p_rd_event_queue ) return 0;
	event_queue[p_wr_event_queue] = event_type;
	p_wr_event_queue = tmp;
	return 1;
}

unsigned char rd_event_queue(unsigned char* ptr_event_type)
{
	if (p_wr_event_queue==p_rd_event_queue) return 0;
	*ptr_event_type = event_queue[p_rd_event_queue];
	p_rd_event_queue = (p_rd_event_queue == MAX_NUM_EVENT_QUEUE) ? 0 : p_rd_event_queue + 1;
	return 1;
}*/

// timeout - in millisecond
/*void
GsSelect(GR_TIMEOUT timeout)
{
	
	// looping for events for this client, such as mouse, keyboard ... 
	// if timeout, place a timeout event into event queue
	//	timeout function call its internal timer, but temporary using linlike8 one
	//do {
//		msleep(timeout);
	//	timeout /= 2;
	//} while (!wr_event_queue(GR_EVENT_TYPE_TIMEOUT));
}

extern void init_srvmain_post(void);
extern void srvmain_post(void);								// a non-looping subr., aims to do not waste this process area, but no timer

*/

