	.module sched.c
	.area text(rom, con, rel)
	.dbfile ./sched.c
	.dbfunc e sched_init _sched_init fV
_sched_init::
	.dbline -1
	.dbline 27
; /*
;  *	kernel/sched.c
;  *
;  *	Kernel scheduler and related syscalls
;  *
;  *	09-04-2004	yan	create this following linux
;  */
; 
; #include "app.h"
; #include "sched.h"
; #include "current.h"									// MUST be placed after sched.h
; #include "system.h"
; #include "timer.h"									// timer_list
; 
; void sched_init(void);
; struct task_struct task[NR_TASKS];
; #if (CONTEXT_SW==1)
; unsigned char current_ptr;
; unsigned char next_ptr;
; #endif
; 
; #if (AUTO_ZERO==0)
; extern void init_timer(void);
; #endif
; 
; void sched_init(void)
; {
L3:
	.dbline 43
	.dbline 43
		mov [__r1],<_task						; 	_task+0, so stack ptr
	;mov [__r0],>_task 
	swap A, SP 
	mvi [__r1], A 
	swap A, SP 
	

	.dbline 43
L4:
	.dbline 43
; 	//unsigned char cpu = smp_processor_id();					// get existing process id
; 	//init_task.processor = cpu;
; 
; #if (AUTO_ZERO==0)
; 	init_timer();
; #endif
; 
; 				// the following does not same to linux
; 				// init. process 0
; #if (AUTO_ZERO==0)
; #if (CONTEXT_SW==1)
; 	current_ptr = 0;
; 	next_ptr = 0;
; #endif
; #endif
; 	get_process0_sp();								// task[0].stack_ptr = SP, since Process0 is also starting from this layer, see main.c
	.dbline 44
; 	wake_up_process(current);
	mov [__r1],[_current_ptr]
	mov [__r0],0
	asl [__r1]
	rlc [__r0]
	add [__r1],<_task+1
	adc [__r0],>_task+1
	mov A,0
	mvi [__r1],A
	.dbline -2
	.dbline 45
; }
L2:
	.dbline 0 ; func end
	ret
	.dbend
	.dbfunc e schedule _schedule fV
_schedule::
	.dbline -1
	.dbline 57
; 
; //void wake_up_process(struct task_struct* p)
; //{
; //	p->state = TASK_RUNNING;
; //}
; 
; #if (CONTEXT_SW==1)
; 		// Description : each time kernel arrange different processes to run, this schedule must be called, so try to min. the code and RAM
; 		//			global variables declaration, since reduce time to declar in local and save memory in each time stack
; struct task_struct *prev;						
; void schedule(void)
; {
	.dbline 58
; 	cli();
		and F, FEh

	.dbline 59
; 	prev = current;
	mov [__r1],[_current_ptr]
	mov [__r0],0
	asl [__r1]
	rlc [__r0]
	mov A,[__r1]
	add A,<_task
	mov [_prev+1],A
	mov A,[__r0]
	adc A,>_task
	mov [_prev],A
L8:
	.dbline 60
; 	do {										// seek next running state process
	.dbline 61
; 		if (next_ptr==0) {								//	just normal sched for next
	cmp [_next_ptr],0
	jnz L11
	.dbline 61
	.dbline 62
; 			current_ptr++;							//		method A => choose this method, since it is more save RAM and ROM code studied from assembly code, at least in cypress PSoC, verify other mcus, and think about what is the good implement method from C
	inc [_current_ptr]
	.dbline 63
; 			if (current_ptr==NR_TASKS) current_ptr = 0;
	cmp [_current_ptr],2
	jnz L12
	.dbline 63
	mov [_current_ptr],0
	.dbline 65
	xjmp L12
L11:
	.dbline 65
; 			//current_ptr = ((current_ptr++) == NR_TASKS) ? 0 :;		//		method B
; 		} else {								//	any urge process
	.dbline 66
; 			current_ptr = next_ptr;
	mov [_current_ptr],[_next_ptr]
	.dbline 67
; 			next_ptr = 0;
	mov [_next_ptr],0
	.dbline 68
; 		}
L12:
	.dbline 69
L9:
	.dbline 69
; 	} while (current->state!=TASK_RUNNING);
	mov [__r1],[_current_ptr]
	mov [__r0],0
	asl [__r1]
	rlc [__r0]
	add [__r1],<_task+1
	adc [__r0],>_task+1
	mvi A,[__r1]
	jnz L8
	.dbline 70
	mov [__r1],[_current_ptr]
	mov [__r0],0
	asl [__r1]
	rlc [__r0]
	add [__r1],<_task
	adc [__r0],>_task
	mov A,[_prev]
	cmp A,[__r0]
	jnz X0
	mov A,[_prev+1]
	cmp A,[__r1]
	jz L16
X0:
L18:
	.dbline 70
	.dbline 70
		;add SP,-2								; remove yield() return address 
	push X								; keep X index register
	swap A, SP 
								; save previous stack pointer, mov p->SP, SP
	mov [__r1], [_prev+1]
	mvi [__r1], A							; 	__r1 + 0, so stack ptr
	mov [__r1],[_current_ptr]					; get next stack pointer, mov SP, n->SP
	mov [__r0],0 
	asl [__r1] 
	rlc [__r0] 
	add [__r1],<_task						; 	_task+0, so stack ptr
	adc [__r0],>_task 
	mvi A,[__r1] 
	swap A, SP 
	pop X
	

	.dbline 70
L19:
	.dbline 70
; 	if (prev!=current) switch_to();
L16:
	.dbline 71
; 	sti();
		or  F, 01h

	.dbline -2
	.dbline 72
; }
L7:
	.dbline 0 ; func end
	ret
	.dbend
	.dbfunc e schedule_timeout _schedule_timeout fV
;              i -> X+3
;          timer -> X+0
; millisec_time_10 -> X-4
_schedule_timeout::
	.dbline -1
	push X
	mov X,SP
	add SP,4
	.dbline 104
; 
; //void sys_sched_yield(void)
; //{
; //
; //}
; //void sched_yield(void)
; //{
; 	//set_current_state(TASK_RUNNING);						// already in running state
; 	//sys_sched_yield();
; //	schedule();
; //}
; 
; #if (TIMER_MOD==1)                    
; 		// Remarks 
; 		//	* max. of time for sleep is 256Sec
; extern char add_timer(struct timer_list* ptimer_addr);
; //extern void del_timer(unsigned char timer_num);
; /*void schedule_timeout(unsigned char sec)
; {
; 	unsigned char i;
; 	struct timer_list timer;
; 	current->state = TASK_INTERRUPTIBLE;
; 	timer.expires = sec * 100 + jiffies;						// since 10mSec for 1 tick, so X100
; 	timer.data = (unsigned int) current;
; 	//timer.function = process_timeout;
; 	i = add_timer(&timer);
; 	schedule();
; 	del_timer(i);
; }*/
; 		// must in times of 10mSec
; void schedule_timeout(unsigned char millisec_time_10)
; {
	.dbline 107
; 	unsigned char i;
; 	struct timer_list timer;
; 	timer.expires = millisec_time_10 + jiffies;					// since 10mSec for 1 tick, so X100
	mov A,[X-4]
	add A,[_jiffies]
	mov [X+0],A
	.dbline 108
; 	timer.data.timer_data = (0xff00 | ((unsigned int) current_ptr));		// related to timer.c only
	mov A,[_current_ptr]
	or A,0
	mov [X+2],A
	mov A,0
	or A,-1
	mov [X+1],A
	.dbline 110
; 	//timer.data = (unsigned int) current;
; 	if ((i = add_timer(&timer))>=0) current->state = TASK_INTERRUPTIBLE;
	mov A,0
	push A
	push X
	xcall _add_timer
	add SP,-2
	mov [X+3],A
	cmp A,0
	jz X1
	jc L23
X1:
	.dbline 110
	mov [__r1],[_current_ptr]
	mov [__r0],0
	asl [__r1]
	rlc [__r0]
	add [__r1],<_task+1
	adc [__r0],>_task+1
	mov A,1
	mvi [__r1],A
L23:
	.dbline 111
	xcall _schedule
	.dbline 112
	cmp [X+3],0
	jz X2
	jc L26
X2:
	.dbline 112
	mov A,[X+3]
	mov [__r1],A
	mov [__r0],0
	asl [__r1]
	rlc [__r0]
	add [__r1],<_timer_vector
	adc [__r0],>_timer_vector
	inc [__r1]
	mov A,0
	mvi [__r1],A
L26:
	.dbline -2
	.dbline 113
; 	schedule();									// when switching, struct timer_list timer staying be kept
; 	if (i>=0) del_timer(i);
; }
L21:
	add SP,-4
	pop X
	.dbline 0 ; func end
	ret
	.dbsym l i 3 c
	.dbsym l timer 0 X
	.dbsym l millisec_time_10 -4 c
	.dbend
	.area bss(ram, con, rel)
	.dbfile ./sched.c
_prev::
	.blkb 2
	.dbsym e prev _prev pX
_current_ptr::
	.blkb 1
	.dbsym e current_ptr _current_ptr c
_task::
	.blkb 4
	.dbsym e task _task A[4:2]X
_next_ptr::
	.blkb 1
	.dbsym e next_ptr _next_ptr c
