	.module time.c
	.area text(rom, con, rel)
	.dbfile ./interrupt.h
	.dbfunc s mark_bh _mark_bh fV
;             nr -> X-4
_mark_bh:
	.dbline -1
	push X
	mov X,SP
	.dbline 22
; /*
;  *	arch/cypress_psoc/kernel/time.c
;  *
;  *	23-04-2004	yan	create this following linux arch but using cypress psoc hardware related codes
;  *
;  */
; 
; #include <m8c.h>									// part specific constants and macros
; #include "PSoCAPI.h"									// PSoC API definitions for all User Modules
; 
; #include "app.h"
; #include "sched.h"
; #include "interrupt.h"									// do_softirq
; #include "gpio.h"									// gpio_var.timer_10msec_f
; 
; extern void do_IRQ(void);
; 
; #if (TIMER_MOD==1)
; void time_init(void)
; {
;         Timer8_1_EnableInt();								// start 8-bit Timer, 5mS for one interrupt
;         Timer8_1_Start();
	.dbline 23
; }
	mov [__r0],1
	mov A,[X-4]
	and A,7
	jz X0
X1:
	asl [__r0]
	dec A
	jnz X1
X0:
	mov A,[_softirq_vec]
	or A,[__r0]
	mov [_softirq_vec],A
	.dbline -2
	.dbline 24
; #endif
L2:
	pop X
	.dbline 0 ; func end
	ret
	.dbsym l nr -4 c
	.dbend
	.dbfile ./time.c
	.dbfunc e time_init _time_init fV
_time_init::
	.dbline -1
	.dbline 20
	.dbline 21
	push X
	xcall _Timer8_1_EnableInt
	.dbline 22
	xcall _Timer8_1_Start
	pop X
	.dbline -2
	.dbline 23
L3:
	.dbline 0 ; func end
	ret
	.dbend
	.dbfunc e timer_interrupt _timer_interrupt fV
_timer_interrupt::
	.dbline -1
	push A
	mov A,[__r0]
	push A
	mov A,[__r1]
	push A
	mov A,[__r2]
	push A
	mov A,[__r3]
	push A
	mov A,[__r4]
	push A
	mov A,[__r5]
	push A
	mov A,[__r6]
	push A
	mov A,[__r7]
	push A
	mov A,[__r8]
	push A
	mov A,[__r9]
	push A
	mov A,[__r10]
	push A
	mov A,[__r11]
	push A
	mov A,[__rX]
	push A
	mov A,[__rY]
	push A
	mov A,[__rZ]
	push A
	.dbline 28
; 
; #pragma	interrupt_handler	timer_interrupt						// shouald add this isr name at boot.asm from PSoC Designer
; void timer_interrupt(void)								// timer interrupt
; {											//	this code must under disable interrupt state in cypress_psoc
	.dbline 30
; #if (TIMER_MOD==1)
; 	if (gpio_var.timer_10msec_f) {							//	since reduce usage of cypress digital block, using a timer8, that it only support to 5mSec each interrupt, but i need 10mSec
	mov A,[_gpio_var]
	and A,1
	jz L5
	.dbline 30
	.dbline 33
; 	//if (chk_timer_int_ctrl) {
; 		//inv_timer_int_ctrl;							
; 		gpio_var.timer_10msec_f ^= 1;						//	must do it 1st, since it will switch to other process
	mov [__r1],[_gpio_var]
	mov [__r0],0
	mov [__r3],<_gpio_var
	mov A,[__r1]
	and A,-2
	mov [__r5],A
	mov A,0
	and A,-1
	mov [__r4],A
	and [__r1],1
	and [__r0],0
	xor [__r1],1
	xor [__r0],0
	mov [__r0],0
	and [__r1],1
	and [__r0],0
	and [__r1],1
	and [__r0],0
	mov A,[__r5]
	or A,[__r1]
	mov [__r1],A
	mov A,[__r4]
	or A,[__r0]
	mov A,[__r1]
	mvi [__r3],A
	.dbline 34
; 		do_timer();								//	should in do_timer_interrupt(), timer interrupt code, need to complete in short time
	xcall _do_timer
	.dbline 35
; 		do_softirq();								//	botton half of interrupt, if needed, enable interrupt in 2nd-half
	xcall _do_softirq
	.dbline 36
	xjmp L6
L5:
	.dbline 36
	mov [__r1],[_gpio_var]
	mov [__r0],0
	mov [__r3],<_gpio_var
	mov A,[__r1]
	and A,-2
	mov [__r5],A
	mov A,0
	and A,-1
	mov [__r4],A
	and [__r1],1
	and [__r0],0
	xor [__r1],1
	xor [__r0],0
	mov [__r0],0
	and [__r1],1
	and [__r0],0
	and [__r1],1
	and [__r0],0
	mov A,[__r5]
	or A,[__r1]
	mov [__r1],A
	mov A,[__r4]
	or A,[__r0]
	mov A,[__r1]
	mvi [__r3],A
L6:
	.dbline -2
	.dbline 39
; 	} else gpio_var.timer_10msec_f ^= 1;
; 	//} else inv_timer_int_ctrl;
; #endif
; }											//	auto enable interrupt again after isr in cypress_psoc
L4:
	pop A
	mov [__rZ],A
	pop A
	mov [__rY],A
	pop A
	mov [__rX],A
	pop A
	mov [__r11],A
	pop A
	mov [__r10],A
	pop A
	mov [__r9],A
	pop A
	mov [__r8],A
	pop A
	mov [__r7],A
	pop A
	mov [__r6],A
	pop A
	mov [__r5],A
	pop A
	mov [__r4],A
	pop A
	mov [__r3],A
	pop A
	mov [__r2],A
	pop A
	mov [__r1],A
	pop A
	mov [__r0],A
	pop A
	.dbline 0 ; func end
	reti
	.dbend
