/*
 *	arch/cypress_psoc/kernel/irq.c
 *
 *	23-04-2004	yan	create this following linux arch but using cypress psoc hardware related codes
 *
 */

#include "system.h"									// sti
#include "irq_cpustat.h"								// softirq_pending
#include "interrupt.h"									// do_softirq

 		// Description : this do_IRQ does not exactly same to linux
		//		ISR
		//		diff. ISR code by diff. interrupt sources
		//		go into this global ISR coding, but by diff. platform ???
		//		go back to ISR, pop all and reti
void do_IRQ(void)
{
	sti();										// enable interrupt again for bh
	if (softirq_pending())								// check software irq status for this MCU
		do_softirq();
}
