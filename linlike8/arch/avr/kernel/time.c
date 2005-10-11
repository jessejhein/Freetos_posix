/*
 *	arch/arv/kernel/time.c
 *
 */

// including, get data linking from others
//==============================================================
//	os config.
#include <linlike8/config.h>
//	avr platform
#include <avr/io.h>									// io ctrl
#include <avr/signal.h>								// interrupt signal
//	os header
#include <linlike8/sched.h>
#if (TIMER_MOD==1)
#define	TIME	1
#include <linlike8/interrupt.h>
#endif

// data
//==============================================================

//	avr reg. setting
#define OCF0					1
#define OCIE0					1
#define OCR0					_SFR_IO8(0x3C)
//#define SIG_OUTPUT_COMPARE0		_VECTOR(19)

// methods
//==============================================================

#if (TIMER_MOD==1)
void time_init(void)
{
//	i need 10mSec to interrupt once
//		i.e. 100Hz, 100 times overflow per second inside timer
//		8MHz(Sys. clk) / 1024(prescaler, TCCR = 101) ~= 8x1024Hz, 1/(8x1024)Hz~=0.1mSec; 0.1mSecx100=10mSec
//		so compare value is 100
//			set for 1024 prescaler
//			CTC mode
	TCCR0 = (1<<CS02)|(1<<CS00)|(1<<WGM01);
	OCR0 = 80;//82;								// should be 10mSec, but not so accuracy, so use 82 by tuning
												//	tuning with electric power meter fr. ringo lee
	TIMSK |= 1<<OCIE0;							// compare match enable
	TIFR |= 1<<OCF0;							// clr pending interrupt
	/////////////////////////////////// enable for overflow interrupt
	//TIMSK |= 1<<TOIE0;
	//TIFR |= 1<<TOV0;
}

//INTERRUPT(SIG_OVERFLOW0)
SIGNAL(SIG_OUTPUT_COMPARE0)
{
	do_timer();									//	should in do_timer_interrupt(), timer interrupt code, need to complete in short time
	do_softirq();								//	botton half of interrupt, if needed, enable interrupt in 2nd-half
}
#endif // (TIMER_MOD==1)
