/*
 *	header of interrupt
 *	
 *	include/linlike8/interrupt.h
 *
 *	29-04-2004	yan	create this following linux
 *
 *	Remarks
 *		must be placed after hardirq.h
 */

extern unsigned char softirq_vec;

enum {
	TIMER_BH = 0,									// timer is always has interrupt, so let it to chk and do 1st to reduce each interrupt time consuming
	GPIO_BH
};

extern void do_softirq(void);

static void mark_bh(unsigned char nr)
{
	softirq_vec |= (0x01 << nr);
}

