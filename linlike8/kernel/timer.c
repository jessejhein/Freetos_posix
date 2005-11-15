/*
 *	kernel/timer.c
 *
 *	23-04-2004	yan	create this following linux
 *	17-06-2004	yan	modify timer structure from 6 bytes to 4 bytes only to reduce RAM usage, then more timer variables can reduce more RAM
 *	27-07-2004	yan	max. timer delay or sleep is 255*10mSec = 2.55 Sec, then reuce more RAM : timer counter from 16-bit to 8-bit
 *	13-10-2004	Hei	modify for the push key function
 */

// including, get data linking from others ===============================================
//	this appl. layer
#include <pin_define.h>
//	linlike8 configuration
#include <linlike8/config.h>
//	mwlike8

//#include "app.h"
//	os header
#include <linlike8/sched.h>							// schedule_timeout()
//#include "current.h"								// current
#if (TIMER_MOD==1)
#include <linlike8/interrupt.h>							// mark_bh()
#endif
#include <linlike8/timer.h>								// timer_list
//#include <asm/system.h>								// sti
#if (KB_MOD==1)
	#include "gpio_kb_app.h"
	#include "gpio.h"
#endif

#if (TIMER_MOD==1)

#if (AUTO_ZERO==1)
unsigned char jiffies;
#else
unsigned char jiffies = 0;								// 8-bit, so can handle 2^8 * 10mSec =  days; if type is be modified, change MAX_JIFFY_OFFSET also in include/linlike8/time.h
#endif

#if (NR_TIMER_OUT>0)
struct timer_list* timer_vector[NR_TIMER_OUT];
#endif

#if (AUTO_ZERO==0)
void init_timer(void)
{
	unsigned char i;
	for (i=0;i<NR_TIMER_OUT;i++)							// clr all to zero, indicating empty timer.
		timer_vector[i] = 0;
}
#endif

void do_timer(void)
{
	jiffies++;									// make sure it is running in atomic, so make sure no other interrupt here
	mark_bh(TIMER_BH);								// enable bh timer task
}

void timer_softirq(void)
{
#if (NR_TIMER_OUT>0)
	unsigned char i;								// common variabe
	
		// this timer_softirq source code should do not longer than 10mSec, since each timer interrupt event is each 10mSec
		//******************************************************************************
		
		// run_timer_list();	=> chk timer event
		//******************************************************************************
	{
//	if (gpio_var.timer_semop==0) {
//		gpio_var.timer_semop = 1;
	for (i=0;i<NR_TIMER_OUT;i++)							// find out non-empty place
		if (timer_vector[i]!=0) {
			struct timer_list timer_event = *timer_vector[i];
			if (timer_event.expires==jiffies) {
#if (CONTEXT_SW==1)
	#if (CYPRESS_PSOC_RAM_UNDER_256==1)
				if ((timer_event.data.timer_data&0xff00)==0xff00) wake_up_process(((struct task_struct*) (&task[(timer_event.data.timer_data&0x00ff)]) ));// related to sched.c only
	#else
				if ((struct timer_list timer_vector[i])->data!=0) wake_up_process(((struct task_struct*) ((struct timer_list timer_vector[i])->data)));
	#endif
				else {
#endif	// (CONTEXT_SW==1)
					del_timer(i);							// must del this timer 1st, since may be user code will re-add this timer again
					timer_event.data.timer_function();
#if (CONTEXT_SW==1)
				}
#endif	// (CONTEXT_SW==1)
				//after timer interrupt, should have schedule again for updated process to run; do it in future; now just let programmer to yield other process, then run into this process again
			}
		}
		
//		gpio_var.timer_semop = 0;
//	}
	}
#endif	// (NR_TIMER_OUT>0)
		
#if (KB_MOD==1)
#if (ROTARY_KEY==1)
	if ((++gpio_var.scan_key_timeout)%7) {
		
	//	if ((gpio_var.key_fr_hi_lo&((unsigned)FR_HI_LO_ENTER))&&(!ENTER_KEY_DET)) {
		if ((gpio_var.key_deb_f==0)&&(!ENTER_KEY_DET)) {
	//		gpio_var.key_fr_hi_lo &= (unsigned) ~FR_HI_LO_ENTER;			//	in low state
			gpio_var.key_deb_f = 1;
			// appl. level code here for ENTER key in DOWN case
			APP_ENTER_KEY_DOWN_CASE;
		} else if (ENTER_KEY_DET) {
			gpio_var.key_deb_f = 0;
		}
		ENTER_KEY_SET_HIGH;								//	restore to high level regularly
		DOWN_KEY_SET_HIGH;								//	since other process also use this gpio port for other function,
		UPPER_KEY_SET_HIGH;								//	it read this pin and write back same data.
												//	However, this pin may be at active state when user click-in
												//	then this active state will be always keep, so we need to clean regulary
		//gpio_var.scan_key_timeout = 0;
	}
#endif

#if (PUSH_KEY==1)
	if ((++gpio_var.scan_key_timeout)%7) {
		
	//	if ((gpio_var.key_fr_hi_lo&((unsigned)FR_HI_LO_ENTER))&&(!ENTER_KEY_DET)) {
		if ((gpio_var.key_deb_f==0)&&(!DOWN_KEY_DET)) {
		//	gpio_var.key_fr_hi_lo &= (unsigned) ~FR_HI_LO_ENTER;			//	in low state
			gpio_var.key_deb_f = 1;
			// appl. level code here for ENTER key in DOWN case
			APP_DOWN_KEY_DOWN_CASE;
		} else if (DOWN_KEY_DET) {
			gpio_var.key_deb_f = 0;
		}							
		//	if ((gpio_var.key_fr_hi_lo&((unsigned)FR_HI_LO_ENTER))&&(!ENTER_KEY_DET)) {
		if ((gpio_var.key_deb_f_1==0)&&(!ENTER_KEY_DET)) {
	//		gpio_var.key_fr_hi_lo &= (unsigned) ~FR_HI_LO_ENTER;			//	in low state
			gpio_var.key_deb_f_1 = 1;
			// appl. level code here for ENTER key in DOWN case
			APP_ENTER_KEY_DOWN_CASE;
		} else if (ENTER_KEY_DET) {
			gpio_var.key_deb_f_1 = 0;
		}
		ENTER_KEY_SET_HIGH;								//	restore to high level regularly
		DOWN_KEY_SET_HIGH;								//	since other process also use this gpio port for other function,
		//UPPER_KEY_SET_HIGH;	
		}			

#endif
#endif	// (KB_MOD==1)

}

#if (NR_TIMER_OUT>0)
// parameters
// return
//	<0 as invalid timer
//	>=0 as valid timer index
char add_timer(struct timer_list* ptimer_addr)
{
	char i;
	for (i=0;i<NR_TIMER_OUT;i++)							// chk struct timer vector for old one
#if (CYPRESS_PSOC_RAM_UNDER_256==1)
		if ((((unsigned int)timer_vector[i])&0x00ff)==(((unsigned int)ptimer_addr)&0x00ff))
		// compiler problem in :
		//	CY27CXXXX has 256 bytes RAM only, so the higher byte of 16-bit data is don't care. 
		//	however, compiler of psoc is don't care the higher byte in parameter input(RAM address data) in subr.
		//	this compiler is still handle the higher byte when comparing RAM address data
#else
		if (timer_vector[(unsigned char)i]==ptimer_addr)
#endif
			return i;
	for (i=0;i<NR_TIMER_OUT;i++)							//		find out the empty place for new timer
		if (timer_vector[(unsigned char)i]==0)
			break;
	if (i==NR_TIMER_OUT) {
		return (char) -1;
	}
	timer_vector[(unsigned char)i] = ptimer_addr;
	return i;
}

/*void del_timer(unsigned char timer_num)
{
	timer_vector[timer_num] = 0; 
}

void sleep(unsigned char sec)
{
	schedule_timeout(sec * 100);
}*/

/*void alarm(p_func function, unsigned char sec)
{
	struct timer_list timer;
	timer.expires = sec*100 + jiffies;
	timer.data = 0;
	timer.function = function;
	add_timer(&timer);
}*/

// parameters
//	jiffies_cnt -- should not be ZERO
// return
//	<0 as invalid timer
//	>=0 as valid timer index
char _malarm(p_func function, unsigned char jiffies_cnt, struct timer_list* timer)
{
//	char i = (char) -1;
//	if (gpio_var.timer_semop==0) {
//		gpio_var.timer_semop = 1;
		timer->expires = jiffies_cnt + jiffies;
//		timer->data.timer_function = (p_func) function;// outside to do it, after solve this problem, enable this
		return add_timer(timer);
//		i = add_timer(timer);
//		gpio_var.timer_semop = 0;
//	}
//	return i;
}
#endif // (NR_TIMER_OUT>0)
#endif

