/*
 * Copyright (c) 24-05-2004 cheng chung yan yan@amonics.com> for 8-bit system
 *
 * this module is gpio in cypress psoc, driver specification. So each arch. has different codes here
 * 
 * date		author		reason
 * 26-07-2004	yan		not user friendly : when rotary key running in circular, ENTER key can't detected by interrupt
 *						    it is because rotary pin mode set as low level interrupt trigger in psoc. 
 *						    when circular running of rotary key, it is level interrupt until completed rotary one key.
 *						    so, change the interrupt mode of rotary pin as any changing in psoc
 * 27-07-2004	yan		gpio interrupt of psoc detect one interrupt from rotary key, but can't detect another enter key at same time
 *				so, remove the enter key from interrupt mode, change to scan mode in each timer isr
 * 03-08-2004	yan		improve the rotary encoder
 * 13-10-2004	Hei		modify for the PUSH_KEY function
 */

#include "app.h"									// this linlike8
#include <m8c.h>									// M8C_EnableIntMask()
#include "interrupt.h"									// mark_bh()
#include "gpio.h"									//
#if (KB_MOD==1)
	#include "kb.h"									// kb hw driver(application dependent, so diff. appl. has diff. kb.h)
	#include "gpio_kb_app.h"							// kb app.
	#include "sched.h"									// schedule_timeout()
	#include "timer.h"									// timer_list
#endif

//#include "psoc_gpio_bit.h"								// led_hw_off()

struct gpio_data gpio_var;
//#if (KB_MOD==1)
//struct timer_list deboucing_timer;
//void kb_deboudcing(void);
//#endif

void gpio_open(void)
{
#if (KB_MOD==1)
        UPPER_KEY_SET_HIGH;								// init. all pins -- logic high since pull high internal
        ENTER_KEY_SET_HIGH;
	DOWN_KEY_SET_HIGH;
	gpio_var.key_deb_f = 0;								// 0 as clr key flag
	gpio_var.key_deb_f_1 = 0;							// for the PUSH_KEY function
	gpio_var.key_fr_hi_lo = 1;							// init. in pull-high for scan key
	#if (PUSH_KEY==1)
	gpio_var.key_fr_hi_lo |= 0x06;
	#endif
#endif
        M8C_EnableIntMask( INT_MSK0, INT_MSK0_GPIO );					// prepare GPIO
}

#pragma	interrupt_handler	gpio_isr						// shouald add this isr name at boot.asm from PSoC Designer
void gpio_isr(void)
{
	
	// fast interrupt
	
#if (KB_MOD==1)
//unsigned char i;
//buz_hw_on;
//led_hw_off;
	// scan key (this source change back to compatible all platform later)
	//******************************************************************************
//	if (gpio_var.key_fr_hi_lo!=(ENTER_KEY_DET&&1)) {
//	if (!ENTER_KEY_DET) {
//		gpio_var.scan_key_event = 0;// interrupt from push button
	#if (ROTARY_KEY==1)
	if (UPPER_KEY_DET) {								// after interrupt(rising) from pin A
		if (DOWN_KEY_DET) {							//	if pin B is high, anti-clockwise
			gpio_var.scan_key_event = 1;
		} else {								//	if pin B is low, clockwise
			gpio_var.scan_key_event = 2;
			DOWN_KEY_SET_HIGH;
		}
		UPPER_KEY_INTR_FALLING;							//	enable pin A interrupt(falling)
	} else {									// after interrupt(falling) from pin A
		UPPER_KEY_INTR_RISING;							//	enable pin A interrupt(rising)
		UPPER_KEY_SET_HIGH;
	}
//	UPPER_KEY_SET_HIGH;
	ENTER_KEY_SET_HIGH;							//	restore to high level
	DOWN_KEY_SET_HIGH;
	#endif

#endif
	
	mark_bh(GPIO_BH);								// enable bh gpio task
	do_softirq();									// botton half of interrupt, if needed, enable interrupt in 2nd-half
//led_hw_on;
}

void gpio_softirq(void)
{
	
#if (KB_MOD==1) 
	switch (gpio_var.scan_key_event) {
//		case 0 : 
//			 if (gpio_var.key_deb_f==0) {
					// setting deboucing timer
//				deboucing_timer.data.timer_function = (p_func) kb_deboudcing;//after solve the problem, put back into alarm()
//				if (malarm((p_func) kb_deboudcing, 10, &deboucing_timer)>=0)		// 10mSec is enough for push key deboucing
//					gpio_var.key_deb_f = 1; 
//			 }
//			break;
	#if (ROTARY_KEY==1)
		case 1 : 
			// appl. level code here for DOWN key in DOWN case
			 APP_DOWN_KEY_DOWN_CASE;
			 gpio_var.scan_key_event = 0;					// clr the previous key-in
			break;
		case 2 : 
			// appl. level code here for UP key in DOWN case
			 APP_UP_KEY_DOWN_CASE;
			 gpio_var.scan_key_event = 0;					// clr the previous key-in
			break;
	#endif
	}
#endif

}


/*#if (KB_MOD==1) 
	// do it must in order DOWN, UP and ENTER, highest priority is at top
	// since they are put into event buffer, then if full, later key cannot put into event
void kb_deboudcing(void)
{
	#if (PUSH_KEY==1)
	if ((!(gpio_var.key_fr_hi_lo&FR_HI_LO_DOWN))&&(DOWN_KEY_DET_HIGH)) {		// chk previous '0'  
		gpio_var.key_fr_hi_lo |= (unsigned) FR_HI_LO_DOWN;
		// appl. level code here for DOWN key in UP case
		APP_DOWN_KEY_UP_CASE;
	}
	if ((!(gpio_var.key_fr_hi_lo&((unsigned)FR_HI_LO_UP)))&&(UPPER_KEY_DET_HIGH)) {
		gpio_var.key_fr_hi_lo |= (unsigned) FR_HI_LO_UP;
		// appl. level code here for UP key in UP case
		APP_UP_KEY_UP_CASE;
	}
	#endif
//	if ((!(gpio_var.key_fr_hi_lo&((unsigned)FR_HI_LO_ENTER)))&&(ENTER_KEY_DET)) {  
//		gpio_var.key_fr_hi_lo |= (unsigned) FR_HI_LO_ENTER;
//		// appl. level code here for ENTER key in UP case
//		APP_ENTER_KEY_UP_CASE;
//	}
	#if (PUSH_KEY==1)
	if ((gpio_var.key_fr_hi_lo&FR_HI_LO_DOWN)&&(DOWN_KEY_DET_LOW)) {		// chk '1' key flag and hw 
		gpio_var.key_fr_hi_lo &= (unsigned) ~FR_HI_LO_DOWN;
		DOWN_KEY_SET_HIGH;							//	restore to high level
		// appl. level code here for DOWN key in DOWN case
		APP_DOWN_KEY_DOWN_CASE;
	}
	if ((gpio_var.key_fr_hi_lo&((unsigned)FR_HI_LO_UP))&&(UPPER_KEY_DET_LOW)) {
		gpio_var.key_fr_hi_lo &= (unsigned) ~FR_HI_LO_UP;
		UPPER_KEY_SET_HIGH;							//	restore to high level
		// appl. level code here for UP key in DOWN case
		APP_UP_KEY_DOWN_CASE;
	}
	#endif
//	if ((gpio_var.key_fr_hi_lo&((unsigned)FR_HI_LO_ENTER))&&(!ENTER_KEY_DET)) {
	if (!ENTER_KEY_DET) {
//		gpio_var.key_fr_hi_lo &= (unsigned) ~FR_HI_LO_ENTER;			//	in low state
		ENTER_KEY_SET_HIGH;							//	restore to high level
		// appl. level code here for ENTER key in DOWN case
		APP_ENTER_KEY_DOWN_CASE;
	}
		
	gpio_var.key_deb_f = 0;								// premit another interrupt 
}
#endif*/

