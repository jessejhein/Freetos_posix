/*
 *	kernel/softirq.c
 *
 *	29-04-2004	yan	create this following linux
 *
 */

#include "app.h"
#include "system.h"									// sti             
#include "hardirq.h"									// irq_cpustat.h, in_interrupt
#include "irq_cpustat.h"								// irq_cpustat_t
#include "softirq.h"									// local_bh_disable
#include "sched.h"									// schedule()

irq_cpustat_t irq_stat[1];								// must be '1', since linlike8 just support single MCU; where to init. in linux ???
unsigned char softirq_vec;

#if (TIMER_MOD==1)                    
extern void timer_softirq(void);
#endif                                                                   
extern void gpio_softirq(void);

void softirq_init(void)
{
#if (AUTO_ZERO==0)
	softirq_vec = 0;
	irq_stat[0].__local_softirq_count = 0;
#endif
}
			
void do_softirq(void)
{
	if (in_interrupt()) return;							// if any previous soft irq is running, avoid nesting, return to let it to run cont., current soft irq will be run after previous by looping
	local_softirq_disable();
	sti();										// start to enable interrupt again
	while (softirq_vec) {
#if (TIMER_MOD==1)                    
		if (softirq_vec&0x01) {timer_softirq();softirq_vec&=~0x01;}		// timer is always has interrupt, so let it to chk and do 1st to reduce each interrupt time consuming
		else if (softirq_vec&0x02) {gpio_softirq();softirq_vec&=~0x02;}
#else
		if (softirq_vec&0x02) {gpio_softirq();softirq_vec&=~0x02;}
#endif
		//else if (softirq_vec&0x04) ;						// after complete one softirq, directly go to while loop goto chk and out
		// ... 
		
	}
	local_softirq_enable();
#if (AUTO_SW_MOD==1)
	schedule();
#endif
}

