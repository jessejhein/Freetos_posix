/*
 *	include/linlike8/irq_cpustat.h
 *
 *	29-04-2004	yan	create this following linux
 *
 */

extern irq_cpustat_t irq_stat[];
#define	local_softirq_count()	irq_stat[0].__local_softirq_count				// irq counting in this MCU

