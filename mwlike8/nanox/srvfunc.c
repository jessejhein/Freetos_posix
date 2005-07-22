/*
 * Copyright (c) 1999, 2000, 2001, 2002, 2003 Greg Haerr <greg@censoft.com>
 * Portions Copyright (c) 2002 by Koninklijke Philips Electronics N.V.
 * Copyright (c) 2000 Alex Holden <alex@linuxhacker.org>
 * Copyright (c) 1991 David I. Bell
 * Copyright (c) 2004 cheng chung yan <chungyan5@hkem.com / yan@amonics.com> for 8-bit system
 * Permission is granted to use, distribute, or modify this source,
 * provided that this copyright notice remains intact.
 *
 * History
 * Date		author		Description
 * 12-08-2004	yan		creation
 * 17-08-2004	yan		branch out a 200408101 product from this main root
 * 13-10-2004	Hei		modify for the screen display so that the screen would update for a regular time
 *				work only in SWITCH_CASE == 1
 */

// including, get data linking from others ===============================================
//	this appl. layer configuration
#include <pin_define.h>
//	os config.
#include <linlike8/config.h>
//	gui mwlike config.
#include "config_mwlike8.h"
#include "app.h"
//	local declaration
#include "nano-X.h"
//#include "gpio_kb_app.h"
//
#include <linlike8\sched.h>									// timer.h
#include <linlike8\timer.h>									// msleep()
//#include "psoc_gpio_bit.h"								// led_hw_off()
#include "cirbuf.h"									// pre_wr_cir254buf()

// data
//==============================================================

//	IO, such as LED(s), Buzzer(s) ... 
p_funct_nano leds_on_drv[IO_NUM];
p_funct_nano leds_off_drv[IO_NUM];
p_funct_nano funct_ptr;
unsigned char led_status[IO_NUM];

#define	CHK_EVENT_CYCLE_TIME	512


		// LED and Buzzer
//struct io_bit_struct buzzer_ctrl;
//struct io_bit_struct led_ctrl;
struct io_bit_struct io_bit_ctrl;
		// events
GR_EVENT events_vect[NR_EVENT];
struct EVENT_PTR ptr_events_vect;
//unsigned char wr_ptr_events_vect;
//unsigned char rd_ptr_events_vect;

// methods ===============================================================================

//	init =================================================================================
void io_open(void)
{
	unsigned char tmp;

//		init. hw connection
	init_io();
	
//		io ctrl to off state
	for (tmp=0;tmp<IO_NUM;tmp++) {
		funct_ptr = leds_off_drv[tmp];
		funct_ptr();
		led_off(tmp);
	}

//		LCD module hw and boot-up seq. 
#if (LCD_MOD==1)
	scr_open();
#endif


	#if (SWITCH_CASE==1)
	io_bit_ctrl.GrGetNextEventTimeout_status = 0;
	#endif
}

// return -1, event buf. full; all others as completed in this number index
/*char wr_event(char event_type)
{
	if ((i=pre_wr_cir254buf(wr_ptr_events_vect,rd_ptr_events_vect,NR_EVENT))!=255) {
		events_vect[wr_ptr_events_vect].type = event_type;
		wr_ptr_events_vect = i;
	}
	//
	unsigned char tmp1 = wr_ptr_events_vect;
	unsigned char tmp = (wr_ptr_events_vect==NR_EVENT) ? 0 : wr_ptr_events_vect + 1;
	if (tmp==rd_ptr_events_vect) return ((char) -1);
	events_vect[wr_ptr_events_vect].type = event_type;
	wr_ptr_events_vect = tmp;
	return tmp1;
}*/

/*
 * Map the window to make it (and possibly its children) visible on the screen.
 * (this subr. is placed in server side, but be called from client, i.e. coding running in client side)
 */
//void
//GrMapWindow(void)
//{
//	unsigned char i;
//	if ((i=pre_wr_cir254buf(ptr_events_vect.wr,ptr_events_vect.rd,NR_EVENT))!=255) {
//		events_vect[ptr_events_vect.wr].type = GR_EVENT_TYPE_EXPOSURE;
//		ptr_events_vect.wr = i;
//	}
//}

/*
 * Return the next event from the event queue, or
 * wait for a new one if one is not ready.  If timeout
 * is nonzero, return timeout event if time elapsed.
 * timeout in millisecond
 * Assume the ep event as zero content
 * (this subr. is placed in server side, but be called from client, i.e. coding running in client side)
 * max. timeout is 12500mSec
 */
#define TIME_OUT_INTERVAL	50							// in mill. second
#if (SWITCH_CASE==0)
void
GrGetNextEventTimeout(GR_EVENT *ep, GR_TIMEOUT timeout)
{
	unsigned char max_time_out = timeout/TIME_OUT_INTERVAL;
	unsigned char i = 0;
	//unsigned char buz_j;
	//unsigned char led_j;
	unsigned char k;								// for circular buffer
	ep->type = GR_EVENT_TYPE_NONE;
	
	do {
		// sleep regular time as TIME_OUT_INTERVAL
		msleep(TIME_OUT_INTERVAL);
		
		// check event job 
		if ((k=pre_rd_cir254buf((unsigned char) ptr_events_vect.wr,(unsigned char) ptr_events_vect.rd,NR_EVENT))==255) {	// no event
			if (i==max_time_out) ep->type = GR_EVENT_TYPE_TIMEOUT;
			else i++;
		} else {										// event occurs
			*ep = events_vect[(unsigned char) ptr_events_vect.rd];
			//ep->type = events_vect[rd_ptr_events_vect].type;
			ptr_events_vect.rd = k;
		}
		
		// buzzer control job
		switch (io_bit_ctrl.buzzer_stat) {
			case BUZZER_OFF :
				 buz_hw_off;
				break;
			case BUZZER_POS_PULSE :
				 buz_hw_on;
				 //io_bit_ctrl.buzzer_stat = BUZZER_POS_PULSE_1;
				 io_bit_ctrl.buzzer_stat = BUZZER_OFF;
				break;
			/*case BUZZER_POS_PULSE_1 :
				 if (max_time_out>4) {					// 4 as 4 X 50mSec = 200mSec
					 if (i>=4) io_bit_ctrl.buzzer_stat = BUZZER_OFF;
				 } else {
					 if (i>=max_time_out) io_bit_ctrl.buzzer_stat = BUZZER_OFF;
				 }
				break;
			case BUZZER_REP_PULSE :
				 buz_j = buzzer_ctrl.cnt;
				 buzzer_ctrl.stat = BUZZER_REP_PULSE_1;
				break;
			case BUZZER_REP_PULSE_1 :
				 if ((buz_j--)>(buzzer_ctrl.cnt/2)) {
					buz_hw_on;
				 } else {
					if (buz_j==0) buz_j = buzzer_ctrl.cnt;
					buz_hw_off;
				 }
				break;*/
		}
		
		// led control job
		switch (io_bit_ctrl.led_stat) {
			case LED_OFF :
				 led_hw_off;
				break;
			case LED_POS_PULSE :
				 led_hw_on;
				 io_bit_ctrl.led_stat = LED_OFF;
				break;
			case LED_ON :
				 led_hw_on;
				break;
			case LED_NEG_PULSE :
				 led_hw_off;
				 io_bit_ctrl.led_stat = LED_ON;
				break;
			/*case LED_REP_PULSE :
				 led_j = led_ctrl.cnt;
				 led_ctrl.stat = LED_REP_PULSE_1;
				break;
			case LED_REP_PULSE_1 :
				 if ((led_j--)>(led_ctrl.cnt/2)) {
					led_hw_on;
				 } else {
					if (led_j==0) led_j = led_ctrl.cnt;
					led_hw_off;
				 }
				break;*/
		}
		
	} while (ep->type==GR_EVENT_TYPE_NONE);
}
#else
//void GrGetNextEventTimeout_timeout_alarm(void);
// return 
//	GR_EVENT_TYPE_NONE -- not completed
//	NOT GR_EVENT_TYPE_NONE -- valid event
GR_EVENT_TYPE GrGetNextEventTimeout(GR_EVENT *ep, GR_TIMEOUT timeout)
{
	unsigned char k;								// for circular buffer
	static unsigned char ggnet_i;
	//static struct timer_list ggnet_timer;
	static unsigned int ggnet_timer;
	ep->type = GR_EVENT_TYPE_NONE;
	switch (io_bit_ctrl.GrGetNextEventTimeout_status) {
		case 0 :
			 ggnet_i = 0;
			 io_bit_ctrl.GrGetNextEventTimeout_status = 1;
			break;
		case 1 :
			 //ggnet_timer.data.timer_function = (p_func) GrGetNextEventTimeout_timeout_alarm;//after solve the problem, put back into alarm()
			 //if (malarm((p_func) GrGetNextEventTimeout_timeout_alarm, TIME_OUT_INTERVAL, &ggnet_timer)>=0)// start timer, sleep regular time as TIME_OUT_INTERVAL
			 //	io_bit_ctrl.GrGetNextEventTimeout_status = 2;
			 ggnet_timer = 0;
			 io_bit_ctrl.GrGetNextEventTimeout_status = 2;
			break;
		case 2 :								// wait alarm for each timeout
			 if ((++ggnet_timer)>=CHK_EVENT_CYCLE_TIME) io_bit_ctrl.GrGetNextEventTimeout_status = 3;
			break;
		case 3 :
			if ((k=pre_rd_cir254buf((unsigned char) ptr_events_vect.wr,(unsigned char) ptr_events_vect.rd,NR_EVENT))!=255) {// event occurs
		//		 if (ggnet_i==timeout/TIME_OUT_INTERVAL) {
		//			 ep->type = GR_EVENT_TYPE_TIMEOUT;
		//		 }
		//		 else {
		//			 ggnet_i++;
		//			 io_bit_ctrl.GrGetNextEventTimeout_status = 1;
		//		 }
		//43	 } else {	
				*ep = events_vect[(unsigned char) ptr_events_vect.rd];
				ptr_events_vect.rd = k;
				//io_bit_ctrl.GrGetNextEventTimeout_status = 3;			// others event, check for timeout event again next time
			} else if (ggnet_i==timeout/TIME_OUT_INTERVAL) {
				ep->type = GR_EVENT_TYPE_TIMEOUT;
				io_bit_ctrl.GrGetNextEventTimeout_status = 0;			// after timeout, goto very beginning
			} else {
				ggnet_i++;
				io_bit_ctrl.GrGetNextEventTimeout_status = 1;			// nothing, prapare for next timeout
			}

#if (IO_MOD>0)
			 // i/o control job
			for (k=0;k<IO_NUM;k++) {
				switch (led_status[k]) {
					case LED_OFF :
						funct_ptr = leds_off_drv[k];
						funct_ptr();
						break;
					case LED_POS_PULSE :
						funct_ptr = leds_on_drv[k];
						funct_ptr();
						led_status[k] = LED_OFF;
						break;
					case LED_ON :
						funct_ptr = leds_on_drv[k];
						funct_ptr();
						break;
					case LED_NEG_PULSE :
						funct_ptr = leds_off_drv[k];
						funct_ptr();
						led_status[k] = LED_ON;
						break;
				}
			}
#endif	// end of #if (IO_MOD>0)
			break;
	}
	return ep->type;
}
//void GrGetNextEventTimeout_timeout_alarm(void)
//{
//	io_bit_ctrl.GrGetNextEventTimeout_status = 3;
//}
#endif

#if (LCD_MOD==1)
/*
 * Draw a text string in the specified drawable using the
 * specified graphics context.
 *
 * flags
 *	bit 0 - '0' as high light off, '1' as high light on
 */
void
GrText(GR_COORD x, GR_COORD y, const unsigned char* str_ROM, unsigned char* str_RAM, GR_COUNT count, GR_TEXTFLAGS flags)
{
	unsigned char i;
	i = count + x;
	
	if (str_ROM==0) {								// if future has more better solution, throw aways this method
		for (;x<i;x++,str_RAM++)
			GrTextDrv(x, y, *str_RAM, flags);
	} else {
		for (;x<i;x++,str_ROM++)
			GrTextDrv(x, y, *str_ROM, flags);
	}
}
#endif

void insert_event_timeout_manual(void)
{
	unsigned char i;
	if ((i=pre_wr_cir254buf(ptr_events_vect.wr,ptr_events_vect.rd,NR_EVENT))!=255) {	// insert event
		events_vect[ptr_events_vect.wr].type = GR_EVENT_TYPE_TIMEOUT_MANUAL;
		ptr_events_vect.wr = i;
		//io_bit_ctrl.GrGetNextEventTimeout_status = 3;									// may be add this, see the performance
	}
}

/*extern unsigned char rd_event_queue(unsigned char* ptr_event_type);
static void
CheckNextEvent(GR_EVENT *ep, GR_BOOL doCheckEvent)
{
	// from client data, get the event list
	// from this event list, get a event
	rd_event_queue(&(ep->type));
}*/

