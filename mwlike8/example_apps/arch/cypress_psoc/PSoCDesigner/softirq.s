	.module softirq.c
	.area text(rom, con, rel)
	.dbfile ./softirq.c
	.dbfunc e softirq_init _softirq_init fV
_softirq_init::
	.dbline -1
	.dbline 24
; /*
;  *	kernel/softirq.c
;  *
;  *	29-04-2004	yan	create this following linux
;  *
;  */
; 
; #include "app.h"
; #include "system.h"									// sti             
; #include "hardirq.h"									// irq_cpustat.h, in_interrupt
; #include "irq_cpustat.h"								// irq_cpustat_t
; #include "softirq.h"									// local_bh_disable
; #include "sched.h"									// schedule()
; 
; irq_cpustat_t irq_stat[1];								// must be '1', since linlike8 just support single MCU; where to init. in linux ???
; unsigned char softirq_vec;
; 
; #if (TIMER_MOD==1)                    
; extern void timer_softirq(void);
; #endif                                                                   
; extern void gpio_softirq(void);
; 
; void softirq_init(void)
; {
	.dbline -2
	.dbline 29
; #if (AUTO_ZERO==0)
; 	softirq_vec = 0;
; 	irq_stat[0].__local_softirq_count = 0;
; #endif
; }
L2:
	.dbline 0 ; func end
	ret
	.dbend
	.dbfunc e do_softirq _do_softirq fV
_do_softirq::
	.dbline -1
	.dbline 32
; 			
; void do_softirq(void)
; {
	.dbline 33
; 	if (in_interrupt()) return;							// if any previous soft irq is running, avoid nesting, return to let it to run cont., current soft irq will be run after previous by looping
	cmp [_irq_stat],0
	jz L4
	.dbline 33
	xjmp L3
L4:
L6:
	.dbline 34
	.dbline 34
	inc [_irq_stat]
	.dbline 34
L7:
	.dbline 34
; 	local_softirq_disable();
	.dbline 35
; 	sti();										// start to enable interrupt again
		or  F, 01h

	xjmp L10
L9:
	.dbline 36
; 	while (softirq_vec) {
	.dbline 38
; #if (TIMER_MOD==1)                    
; 		if (softirq_vec&0x01) {timer_softirq();softirq_vec&=~0x01;}		// timer is always has interrupt, so let it to chk and do 1st to reduce each interrupt time consuming
	tst [_softirq_vec],1
	jz L12
	.dbline 38
	.dbline 38
	xcall _timer_softirq
	.dbline 38
	and [_softirq_vec],-2
	.dbline 38
	xjmp L13
L12:
	.dbline 39
; 		else if (softirq_vec&0x02) {gpio_softirq();softirq_vec&=~0x02;}
	tst [_softirq_vec],2
	jz L14
	.dbline 39
	.dbline 39
	xcall _gpio_softirq
	.dbline 39
	and [_softirq_vec],-3
	.dbline 39
L14:
L13:
	.dbline 46
L10:
	.dbline 36
	cmp [_softirq_vec],0
	jnz L9
L16:
	.dbline 47
	.dbline 47
	dec [_irq_stat]
	.dbline 47
L17:
	.dbline 47
; #else
; 		if (softirq_vec&0x02) {gpio_softirq();softirq_vec&=~0x02;}
; #endif
; 		//else if (softirq_vec&0x04) ;						// after complete one softirq, directly go to while loop goto chk and out
; 		// ... 
; 		
; 	}
; 	local_softirq_enable();
	.dbline -2
	.dbline 51
; #if (AUTO_SW_MOD==1)
; 	schedule();
; #endif
; }
L3:
	.dbline 0 ; func end
	ret
	.dbend
	.area bss(ram, con, rel)
	.dbfile ./softirq.c
_softirq_vec::
	.blkb 1
	.dbsym e softirq_vec _softirq_vec c
_irq_stat::
	.blkb 1
	.dbsym e irq_stat _irq_stat A[1:1]X
