/*
 *	arch/cypress_psoc/kernel/time.c
 *
 *	23-04-2004	yan	create this following linux arch but using cypress psoc hardware related codes
 *
 */

// including, get data linking from others
//==============================================================
//	os config.
#include <linlike8/config.h>

#include <m8c.h>									// part specific constants and macros
#include "PSoCAPI.h"									// PSoC API definitions for all User Modules

#include "app.h"
#include <linlike8/sched.h>
#include <linlike8/interrupt.h>						// do_softirq
#include "gpio.h"									// gpio_var.timer_10msec_f

extern void do_IRQ(void);

#if (TIMER_MOD==1)
void time_init(void)
{
        Timer8_1_EnableInt();								// start 8-bit Timer, 5mS for one interrupt
        Timer8_1_Start();
}
#endif

#pragma	interrupt_handler	timer_interrupt						// shouald add this isr name at boot.asm from PSoC Designer
void timer_interrupt(void)								// timer interrupt
{											//	this code must under disable interrupt state in cypress_psoc
#if (TIMER_MOD==1)
	if (gpio_var.timer_10msec_f) {							//	since reduce usage of cypress digital block, using a timer8, that it only support to 5mSec each interrupt, but i need 10mSec
	//if (chk_timer_int_ctrl) {
		//inv_timer_int_ctrl;							
		gpio_var.timer_10msec_f ^= 1;						//	must do it 1st, since it will switch to other process
		do_timer();								//	should in do_timer_interrupt(), timer interrupt code, need to complete in short time
		do_softirq();								//	botton half of interrupt, if needed, enable interrupt in 2nd-half
	} else gpio_var.timer_10msec_f ^= 1;
	//} else inv_timer_int_ctrl;
#endif
}											//	auto enable interrupt again after isr in cypress_psoc

