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
//		1MHz(Sys. clk) / 1024(prescaler, TCCR = 101) ~= 1024; 1024/10 ~= 100
//		so compare value is 10
//			set for 1024 prescaler
//			CTC mode
	TCCR0 = (1<<CS02)|(1<<CS00)|(1<<WGM01);
	OCR0 = 9;									// should be 10mSec, but not so accuracy, so use 9
	//10;
	TIMSK |= 1<<OCIE0;							// compare match enable
	TIFR |= 1<<OCF0;							// clr pending interrupt
	/////////////////////////////////// enable for overflow interrupt
	//TIMSK |= 1<<TOIE0;
	//TIFR |= 1<<TOV0;
}

//INTERRUPT(SIG_OVERFLOW0)
INTERRUPT(SIG_OUTPUT_COMPARE0)
{
	do_timer();									//	should in do_timer_interrupt(), timer interrupt code, need to complete in short time
	do_softirq();								//	botton half of interrupt, if needed, enable interrupt in 2nd-half
}
#endif // (TIMER_MOD==1)