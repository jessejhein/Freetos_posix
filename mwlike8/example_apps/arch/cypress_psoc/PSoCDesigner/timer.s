	.module timer.c
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
;  *	kernel/timer.c
;  *
;  *	23-04-2004	yan	create this following linux
;  *	17-06-2004	yan	modify timer structure from 6 bytes to 4 bytes only to reduce RAM usage, then more timer variables can reduce more RAM
;  *	27-07-2004	yan	max. timer delay or sleep is 255*10mSec = 2.55 Sec, then reuce more RAM : timer counter from 16-bit to 8-bit
;  *
;  */
; 
; #include "app.h"
; #include "sched.h"									// schedule_timeout()
; #include "current.h"									// current
; #include "interrupt.h"									// mark_bh()
; #include "timer.h"									// timer_list
; #include "system.h"									// sti()             
; #if (KB_MOD==1)
; 	#include "kb.h"									// kb hw driver(application dependent, so diff. appl. has diff. kb.h)
; 	#include "gpio.h"
; 	#include "gpio_kb_app.h"							// kb app.
; #endif
; 
; #if (TIMER_MOD==1)
	.dbline 23
; 
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
; #if (AUTO_ZERO==1)
L2:
	pop X
	.dbline 0 ; func end
	ret
	.dbsym l nr -4 c
	.dbend
	.dbfile ./timer.c
	.dbfunc e do_timer _do_timer fV
_do_timer::
	.dbline -1
	.dbline 44
; unsigned char jiffies;
; #else
; unsigned char jiffies = 0;								// 9-bit, so can handle 2^8 * 10mSec =  days; if type is be modified, change MAX_JIFFY_OFFSET also in include/linlike8/time.h
; #endif
; 
; #if (NR_TIMER_OUT>0)
; struct timer_list* timer_vector[NR_TIMER_OUT];
; #endif
; 
; #if (AUTO_ZERO==0)
; void init_timer(void)
; {
; 	unsigned char i;
; 	for (i=0;i<NR_TIMER_OUT;i++)							// clr all to zero, indicating empty timer.
; 		timer_vector[i] = 0;
; }
; #endif
; 
; void do_timer(void)
; {
	.dbline 45
; 	jiffies++;									// make sure it is running in atomic, so make sure no other interrupt here
	inc [_jiffies]
	.dbline 46
; 	mark_bh(TIMER_BH);								// enable bh timer task
	mov A,0
	push A
	xcall _mark_bh
	add SP,-1
	.dbline -2
	.dbline 47
; }
L6:
	.dbline 0 ; func end
	ret
	.dbend
	.dbfunc e timer_softirq _timer_softirq fV
;              i -> X+1
;    timer_event -> X+1
;              i -> X+0
_timer_softirq::
	.dbline -1
	push X
	mov X,SP
	add SP,4
	.dbline 50
; 
; void timer_softirq(void)
; {
	.dbline 59
; #if (NR_TIMER_OUT>0)
; 	unsigned char i;								// common variabe
; 	
; 		// this timer_softirq source code should do not longer than 10mSec, since each timer interrupt event is each 10mSec
; 		//******************************************************************************
; 		
; 		// run_timer_list();	=> chk timer event
; 		//******************************************************************************
; 	{
	.dbline 62
; //	if (gpio_var.timer_semop==0) {
; //		gpio_var.timer_semop = 1;
; 	for (i=0;i<NR_TIMER_OUT;i++)							// find out non-empty place
	mov [X+0],0
	xjmp L11
L8:
	.dbline 63
; 		if (timer_vector[i]!=0) {
	mov A,[X+0]
	mov [__r1],A
	mov [__r0],0
	asl [__r1]
	rlc [__r0]
	add [__r1],<_timer_vector
	adc [__r0],>_timer_vector
	mvi A,[__r1]
	mov [__r0],A
	mvi A,[__r1]
	cmp [__r0],0
	jnz X2
	cmp A,0
	jz L12
X2:
	.dbline 63
	.dbline 64
; 			struct timer_list timer_event = *timer_vector[i];
	mov [__r1],X
	add [__r1],1
	mov A,[X+0]
	mov [__r3],A
	mov [__r2],0
	asl [__r3]
	rlc [__r2]
	add [__r3],<_timer_vector
	adc [__r2],>_timer_vector
	mvi A,[__r3]
	mvi A,[__r3]
	mov [__r3],A
	mvi A,[__r3]
	mvi [__r1],A
	mvi A,[__r3]
	mvi [__r1],A
	mvi A,[__r3]
	mvi [__r1],A
	.dbline 65
; 			if (timer_event.expires==jiffies) {
	mov A,[X+1]
	cmp A,[_jiffies]
	jnz L14
	.dbline 65
	.dbline 66
; 				if ((timer_event.data.timer_data&0xff00)==0xff00) wake_up_process(((struct task_struct*) (&task[(timer_event.data.timer_data&0x00ff)]) ));// related to sched.c only
	mov A,[X+3]
	and A,0
	mov [__r1],A
	mov A,[X+2]
	and A,-1
	cmp A,-1
	jnz L16
	cmp [__r1],0
	jnz L16
X3:
	.dbline 66
	mov A,[X+3]
	and A,-1
	mov [__r1],A
	mov A,[X+2]
	and A,0
	mov [__r0],A
	asl [__r1]
	rlc [__r0]
	add [__r1],<_task+1
	adc [__r0],>_task+1
	mov A,0
	mvi [__r1],A
	xjmp L17
L16:
	.dbline 68
; //				if ((struct timer_list timer_vector[i])->data!=0) wake_up_process(((struct task_struct*) ((struct timer_list timer_vector[i])->data)));
; 				else {
	.dbline 69
; 					timer_event.data.timer_function();
	mov A,[X+3]
	mov [__r1],A
	mov A,[X+2]
	mov [__r0],A
	xcall __plcall
	.dbline 70
; 					del_timer(i);
	mov A,[X+0]
	mov [__r1],A
	mov [__r0],0
	asl [__r1]
	rlc [__r0]
	add [__r1],<_timer_vector
	adc [__r0],>_timer_vector
	inc [__r1]
	mov A,0
	mvi [__r1],A
	.dbline 71
; 				}
L17:
	.dbline 73
; 				//after timer interrupt, should have schedule again for updated process to run; do it in future; now just let programmer to yield other process, then run into this process again
; 			}
L14:
	.dbline 74
; 		}
L12:
L9:
	.dbline 62
	inc [X+0]
L11:
	.dbline 62
	cmp [X+0],3
	jz X4
	jc L8
X4:
	.dbline 78
; 		
; //		gpio_var.timer_semop = 0;
; //	}
; 	}
	.dbline 83
; #endif
; 		
; #if (KB_MOD==1)
; 
; 	if ((++gpio_var.scan_key_timeout)%7) {
	mov A,[_gpio_var]
	asr A
	asr A
	asr A
	asr A
	asr A
	and A,7
	mov [__r0],A
	and [__r0],7
	add [__r0],1
	mov [__r1],[__r0]
	mov [__r0],0
	and [__r1],7
	and [__r0],0
	mov [__r3],<_gpio_var
	mov [__r5],[_gpio_var]
	mov [__r4],0
	and [__r5],31
	and [__r4],-1
	mov [__r7],[__r1]
	mov [__r6],[__r0]
	asl [__r7]
	rlc [__r6]
	asl [__r7]
	rlc [__r6]
	asl [__r7]
	rlc [__r6]
	asl [__r7]
	rlc [__r6]
	asl [__r7]
	rlc [__r6]
	and [__r7],-32
	and [__r6],0
	mov A,[__r7]
	or [__r5],A
	mov A,[__r6]
	or [__r4],A
	mov A,[__r5]
	mvi [__r3],A
	mov A,0
	push A
	mov A,7
	push A
	mov A,[__r0]
	push A
	mov A,[__r1]
	push A
	xcall __divmod_16X16_16
	add SP,-2
	pop A
	mov [__r1],A
	pop A
	cmp A,0
	jnz X5
	cmp [__r1],0
	jz L22
X5:
	.dbline 83
	.dbline 86
; 		
; 	//	if ((gpio_var.key_fr_hi_lo&((unsigned)FR_HI_LO_ENTER))&&(!ENTER_KEY_DET)) {
; 		if ((gpio_var.key_deb_f==0)&&(!ENTER_KEY_DET)) {
	mov [__r0],[_gpio_var]
	and F,-5
	rrc [__r0]
	and [__r0],1
	jnz L24
	tst REG[12],2
	jnz L24
	.dbline 86
	.dbline 88
; 	//		gpio_var.key_fr_hi_lo &= (unsigned) ~FR_HI_LO_ENTER;			//	in low state
; 			gpio_var.key_deb_f = 1;
	or [_gpio_var],2
	mov A,[_gpio_var]
L26:
	.dbline 90
; 			// appl. level code here for ENTER key in DOWN case
; 			APP_ENTER_KEY_DOWN_CASE;
	.dbline 90
	mov A,2
	push A
	mov A,[_ptr_events_vect]
	asr A
	asr A
	asr A
	asr A
	and A,15
	push A
	mov A,[_ptr_events_vect]
	and A,15
	push A
	xcall _pre_wr_cir254buf
	add SP,-3
	mov [X+1],A
	cmp A,-1
	jz L25
	.dbline 90
	.dbline 90
	mov [__r1],[_ptr_events_vect]
	mov [__r0],0
	and [__r1],15
	and [__r0],0
	add [__r1],<_events_vect
	adc [__r0],>_events_vect
	mvi A,[__r1]
	dec [__r1]
	mov [__r2],A
	and [__r2],-8
	or [__r2],2
	mov A,[__r2]
	mvi [__r1],A
	.dbline 90
	mov [__r1],[_ptr_events_vect]
	mov [__r0],0
	and [__r1],15
	and [__r0],0
	add [__r1],<_events_vect
	adc [__r0],>_events_vect
	mvi A,[__r1]
	dec [__r1]
	and A,-25
	mvi [__r1],A
	.dbline 90
	mov A,[_ptr_events_vect]
	and A,-16
	mov [__r0],A
	mov A,[X+1]
	and A,15
	or [__r0],A
	mov [_ptr_events_vect],[__r0]
	.dbline 90
	.dbline 90
	.dbline 90
	.dbline 91
	xjmp L25
L24:
	.dbline 91
; 		} else if (ENTER_KEY_DET) {
	tst REG[12],2
	jz L31
	.dbline 91
	.dbline 92
; 			gpio_var.key_deb_f = 0;
	and [_gpio_var],-3
	.dbline 93
; 		}
L31:
L25:
	.dbline 94
	or REG[12],2
	.dbline 95
	or REG[12],8
	.dbline 96
	or REG[20],8
	.dbline 100
L22:
	.dbline -2
	.dbline 103
; 		ENTER_KEY_SET_HIGH;								//	restore to high level regularly
; 		DOWN_KEY_SET_HIGH;								//	since other process also use this gpio port for other function,
; 		UPPER_KEY_SET_HIGH;								//	it read this pin and write back same data.
; 												//	However, this pin may be at active state when user click-in
; 												//	then this active state will be always keep, so we need to clean regulary
; 		//gpio_var.scan_key_timeout = 0;
; 	}
; #endif
; 
; }
L7:
	add SP,-4
	pop X
	.dbline 0 ; func end
	ret
	.dbsym l i 1 c
	.dbsym l timer_event 1 X
	.dbsym l i 0 c
	.dbend
	.dbfunc e add_timer _add_timer fc
;              i -> X+0
;    ptimer_addr -> X-5
_add_timer::
	.dbline -1
	push X
	mov X,SP
	add SP,1
	.dbline 111
; 
; #if (NR_TIMER_OUT>0)
; // parameters
; // return
; //	<0 as invalid timer
; //	>=0 as valid timer index
; char add_timer(struct timer_list* ptimer_addr)
; {
	.dbline 113
; 	char i;
; 	for (i=0;i<NR_TIMER_OUT;i++)							// chk struct timer vector for old one
	mov [X+0],0
	xjmp L37
L34:
	.dbline 115
; #if (CYPRESS_PSOC_RAM_UNDER_256==1)
; 		if ((((unsigned int)timer_vector[i])&0x00ff)==(((unsigned int)ptimer_addr)&0x00ff))
	mov A,[X+0]
	mov [__r1],A
	mov [__r0],0
	asl [__r1]
	rlc [__r0]
	add [__r1],<_timer_vector
	adc [__r0],>_timer_vector
	mvi A,[__r1]
	mov [__r0],A
	mvi A,[__r1]
	mov [__r1],A
	and [__r1],-1
	and [__r0],0
	mov A,[X-4]
	and A,-1
	mov [__r3],A
	mov A,[X-5]
	and A,0
	mov [__r2],A
	mov A,[__r0]
	cmp A,[__r2]
	jnz L38
	mov A,[__r1]
	cmp A,[__r3]
	jnz L38
X6:
	.dbline 123
; 		// compiler problem in :
; 		//	CY27CXXXX has 256 bytes RAM only, so the higher byte of 16-bit data is don't care. 
; 		//	however, compiler of psoc is don't care the higher byte in parameter input(RAM address data) in subr.
; 		//	this compiler is still handle the higher byte when comparing RAM address data
; #else
; 		if (timer_vector[i]==ptimer_addr)
; #endif
; 			return i;
	mov A,[X+0]
	xjmp L33
L38:
L35:
	.dbline 113
	inc [X+0]
L37:
	.dbline 113
	cmp [X+0],3
	jz X7
	jc L34
X7:
	.dbline 124
; 	for (i=0;i<NR_TIMER_OUT;i++)							//		find out the empty place for new timer
	mov [X+0],0
	xjmp L43
L40:
	.dbline 125
; 		if (timer_vector[i]==0)
	mov A,[X+0]
	mov [__r1],A
	mov [__r0],0
	asl [__r1]
	rlc [__r0]
	add [__r1],<_timer_vector
	adc [__r0],>_timer_vector
	mvi A,[__r1]
	mov [__r0],A
	mvi A,[__r1]
	cmp [__r0],0
	jnz L44
	cmp A,0
	jnz L44
X8:
	.dbline 126
; 			break;
	xjmp L42
L44:
L41:
	.dbline 124
	inc [X+0]
L43:
	.dbline 124
	cmp [X+0],3
	jz X9
	jc L40
X9:
L42:
	.dbline 127
; 	if (i==NR_TIMER_OUT) {
	cmp [X+0],3
	jnz L46
	.dbline 127
	.dbline 128
; 		return (char) -1;
	mov A,-1
	xjmp L33
L46:
	.dbline 130
; 	}
; 	timer_vector[i] = ptimer_addr;
	mov A,[X+0]
	mov [__r1],A
	mov [__r0],0
	asl [__r1]
	rlc [__r0]
	add [__r1],<_timer_vector
	adc [__r0],>_timer_vector
	inc [__r1]
	mov A,[X-4]
	mvi [__r1],A
	.dbline 131
; 	return i;
	mov A,[X+0]
	.dbline -2
L33:
	add SP,-1
	pop X
	.dbline 0 ; func end
	ret
	.dbsym l i 0 c
	.dbsym l ptimer_addr -5 pX
	.dbend
	.dbfunc e _malarm __malarm fc
;          timer -> X-8
;    jiffies_cnt -> X-6
;       function -> X-5
__malarm::
	.dbline -1
	push X
	mov X,SP
	.dbline 159
; }
; 
; /*void del_timer(unsigned char timer_num)
; {
; 	timer_vector[timer_num] = 0; 
; }
; 
; void sleep(unsigned char sec)
; {
; 	schedule_timeout(sec * 100);
; }*/
; 
; /*void alarm(p_func function, unsigned char sec)
; {
; 	struct timer_list timer;
; 	timer.expires = sec*100 + jiffies;
; 	timer.data = 0;
; 	timer.function = function;
; 	add_timer(&timer);
; }*/
; 
; // parameters
; //	jiffies_cnt -- should not be ZERO
; // return
; //	<0 as invalid timer
; //	>=0 as valid timer index
; char _malarm(p_func function, unsigned char jiffies_cnt, struct timer_list* timer)
; {
	.dbline 163
; //	char i = (char) -1;
; //	if (gpio_var.timer_semop==0) {
; //		gpio_var.timer_semop = 1;
; 		timer->expires = jiffies_cnt + jiffies;
	mov A,[X-7]
	mov [__r1],A
	mov A,[X-6]
	add A,[_jiffies]
	mvi [__r1],A
	.dbline 165
; //		timer->data.timer_function = (p_func) function;// outside to do it, after solve this problem, enable this
; 		return add_timer(timer);
	mov A,[X-8]
	push A
	mov A,[X-7]
	push A
	xcall _add_timer
	add SP,-2
	.dbline -2
L48:
	pop X
	.dbline 0 ; func end
	ret
	.dbsym l timer -8 pX
	.dbsym l jiffies_cnt -6 c
	.dbsym l function -5 pfX
	.dbend
	.area bss(ram, con, rel)
	.dbfile ./timer.c
_timer_vector::
	.blkb 6
	.dbsym e timer_vector _timer_vector A[6:3]pX
_jiffies::
	.blkb 1
	.dbsym e jiffies _jiffies c
