/*
 * Copyright (c) 24-05-2004 cheng chung yan yan@amonics.com> / <chungyan5@hkem.com for 8-bit system
 *
 * this module is gpio in cypress psoc, driver specification. So each arch. has different code here
 * 
 * using cypress psoc gpio
 * for keyboard scanning and ... 
 *
 */

#include "app.h"									// this linlike8
#include "m8c.h"									// M8C_EnableIntMask()
#include "interrupt.h"									// mark_bh()
#if (KB_MOD==1)
	#include "kb.h"									// kb hw driver(application dependent, so diff. appl. has diff. kb.h)
	#include "sched.h"								// timer.h
	#include "timer.h"								// deboucing_timer
#endif
#include "gpio.h"									// kb hw driver(application dependent, so diff. appl. has diff. kb.h)

#if (KB_MOD==1)
struct gpio_data gpio_var;
struct timer_list deboucing_timer;
void kb_deboudcing(void);
	// gui - mwlike8
#define	APP_UP_KEY_DOWN_CASE	do {									\
	} while (0)
		/*#if (SWITCH_CASE==1)
		if ((i=pre_wr_cir254buf(wr_key_raw_code,rd_key_raw_code,NR_KEY_CODE))!=255) {
			key_raw_code[wr_key_raw_code] = UP_KEY_DOWN;
			wr_key_raw_code = i;
		}
		#else 
		gpio_var.key_raw_code = UP_KEY_DOWN;
		#endif
		let_idle_process_to_run();*/
#define	APP_DOWN_KEY_DOWN_CASE	do {									\
	} while (0)
		/*#if (SWITCH_CASE==1)
		if ((i=pre_wr_cir254buf(wr_key_raw_code,rd_key_raw_code,NR_KEY_CODE))!=255) {
			key_raw_code[wr_key_raw_code] = UP_KEY_DOWN;
			wr_key_raw_code = i;
		}
		#else 
		gpio_var.key_raw_code = UP_KEY_DOWN;
		#endif
		let_idle_process_to_run();*/
#endif

void gpio_open(void)
{
#if (KB_MOD==1)
        UPPER_KEY_SET_HIGH;								// init. all pins -- logic high since pull high internal
        ENTER_KEY_SET_HIGH;
	DOWN_KEY_SET_HIGH;
	gpio_var.key_deb_f = 0;								// 0 as clr key flag
	#if (PUSH_KEY==1)
	gpio_var.key_fr_hi_lo = 0x7;							// init. in pull-high for scan key
	#endif
#endif
        M8C_EnableIntMask( INT_MSK0, INT_MSK0_GPIO );					// prepare GPIO
}

#pragma	interrupt_handler	gpio_isr						// shouald add this isr name at boot.asm from PSoC Designer
void gpio_isr(void)
{
	mark_bh(GPIO_BH);								// enable bh gpio task
	do_softirq();									// botton half of interrupt, if needed, enable interrupt in 2nd-half
}

void gpio_softirq(void)
{

#if (KB_MOD==1)
	if (gpio_var.key_deb_f==0) {
			// setting deboucing timer
		deboucing_timer.function = (p_func) kb_deboudcing;//after solve the problem, put back into alarm()
		malarm((p_func) kb_deboudcing, 10, &deboucing_timer);			// 10mSec is enough for push key deboucing
		gpio_var.key_deb_f = 1;
	}
#endif

}

#if (KB_MOD==1)
void kb_deboudcing(void)
{
	unsigned char i;
	
	#if (ROTARY_KEY==1)
	if (UPPER_KEY_DET_LOW) {
		if (DOWN_KEY_DET_HIGH) {						// clockwise
			// appl. level code here for UP key in DOWN case
			APP_UP_KEY_DOWN_CASE;
		} else {								// anti-clockwise
			DOWN_KEY_SET_HIGH;
			// appl. level code here for DOWN key in DOWN case
			APP_DOWN_KEY_DOWN_CASE;
		}
		UPPER_KEY_SET_HIGH;
	} //else                               // noise only, return to start again
	#endif
	
	#if (PUSH_KEY==1)
		// do it must in order DOWN, UP and ENTER, highest priority is from last one
	if ((!(gpio_var.key_fr_hi_lo&FR_HI_LO_DOWN))&&(DOWN_KEY_DET_HIGH)) {		// chk previous '0'  
		gpio_var.key_fr_hi_lo |= (unsigned) FR_HI_LO_DOWN;
		// appl. level code here for DOWN key in UP case
	}
	if ((!(gpio_var.key_fr_hi_lo&((unsigned)FR_HI_LO_UP)))&&(UPPER_KEY_DET_HIGH)) {
		gpio_var.key_fr_hi_lo |= (unsigned) FR_HI_LO_UP;
		// appl. level code here for UP key in UP case
	}
	#endif
	if ((!(gpio_var.key_fr_hi_lo&((unsigned)FR_HI_LO_ENTER)))&&(ENTER_KEY_DET_HIGH)) {  
		gpio_var.key_fr_hi_lo |= (unsigned) FR_HI_LO_ENTER;
		// appl. level code here for ENTER key in UP case
	}
	#if (PUSH_KEY==1)
	if ((gpio_var.key_fr_hi_lo&FR_HI_LO_DOWN)&&(DOWN_KEY_DET_LOW)) {		// chk '1' key flag and hw 
		gpio_var.key_fr_hi_lo &= (unsigned) ~FR_HI_LO_DOWN;
		// appl. level code here for DOWN key in DOWN case
		APP_DOWN_KEY_DOWN_CASE;
	}
	if ((gpio_var.key_fr_hi_lo&((unsigned)FR_HI_LO_UP))&&(UPPER_KEY_DET_LOW)) {
		gpio_var.key_fr_hi_lo &= (unsigned) ~FR_HI_LO_UP;
		// appl. level code here for UP key in DOWN case
		APP_UP_KEY_DOWN_CASE;
	}
	#endif
	if ((gpio_var.key_fr_hi_lo&((unsigned)FR_HI_LO_ENTER))&&(ENTER_KEY_DET_LOW)) {
		gpio_var.key_fr_hi_lo &= (unsigned) ~FR_HI_LO_ENTER;//	in low state
		// appl. level code here for ENTER key in DOWN case
	}
	
	gpio_var.key_deb_f = 0;								// premit another interrupt 
}
#endif

