/*
 *	include/asm-cypress_psoc/softirq.h
 *
 *	29-04-2004	yan	create this following linux but using cypress psoc hardware related codes
 *
 */

#define	local_softirq_disable()	do { local_softirq_count()++;} while (0)
#define	local_softirq_enable()	do { local_softirq_count()--;} while (0)

