/*
 *	include/asm-cypress_psoc/hardirq.h
 *
 *	29-04-2004	yan	create this following linux but using cypress psoc hardware related codes
 *
 */

typedef struct {
	unsigned char __local_softirq_count;
} irq_cpustat_t;

#define in_interrupt() (local_softirq_count())
