	.module fork.c
	.area text(rom, con, rel)
	.dbfile ./fork.c
	.dbfunc e do_fork _do_fork fV
;        process -> X-7
; prev_stack_size -> X-5
;    process_num -> X-4
_do_fork::
	.dbline -1
	push X
	mov X,SP
	.dbline 18
; /*
;  *	linlike8/kernel/fork.c
;  *
;  *	10-04-2004	yan	create this following linux
;  *
;  *	fork new process
;  */
; 
; #include "app.h"
; #include "sched.h"
; #include "current.h"									// MUST be placed after sched.h
; #include "system.h"									// MUST be placed after sched.h
; 
; #if (CONTEXT_SW==1)
; 		// process_num - hardcode number, beware to not double
; 		// get the previous process stack size, then cal. the new process stack starting point
; void do_fork(unsigned char process_num, unsigned char prev_stack_size, p_func process)
; {
	.dbline 20
; 	//task[process_num] = *current;							// allocate memory for new process, then copy the existing process content to new one
; 	task[process_num].stack_ptr = task[process_num-1].stack_ptr + prev_stack_size;
	mov A,[X-4]
	mov [__r1],A
	mov [__r0],0
	asl [__r1]
	rlc [__r0]
	add [__r1],<_task
	adc [__r0],>_task
	mov A,[X-4]
	mov [__r3],A
	mov [__r2],0
	sub [__r3],1
	sbb [__r2],0
	asl [__r3]
	rlc [__r2]
	add [__r3],<_task
	adc [__r2],>_task
	mvi A,[__r3]
	mov [__r2],A
	mov A,[X-5]
	add [__r2],A
	mov A,[__r2]
	mvi [__r1],A
L2:
	.dbline 21
	.dbline 21
	mov A,[X-4]
	mov [__r1],A
	mov [__r0],0
	asl [__r1]
	rlc [__r0]
	add [__r1],<_task
	adc [__r0],>_task
	mvi A,[__r1]
	mov [__r1],A
	mov [__r0],0
	mov A,[X-6]
	mov [__r3],A
	mov A,[X-7]
	push X
	push A
	mov X,[__r3]
	romx
	mov [__r2],A
	pop A
	inc X
	adc A,0
	romx
	mov [__r3],A
	pop X
	mov A,[__r2]
	mvi [__r1],A
	mov A,[__r3]
	mvi [__r1],A
	.dbline 21
	mov A,[X-4]
	mov [__r1],A
	asl [__r1]
	rlc [__r0]
	add [__r1],<_task
	adc [__r0],>_task
	mvi A,[__r1]
	dec [__r1]
	add A,3
	mvi [__r1],A
	.dbline 21
L3:
	.dbline 21
; 	place_process_stack();
	.dbline 22
; 	wake_up_process((&task[process_num]));
	mov A,[X-4]
	mov [__r1],A
	mov [__r0],0
	asl [__r1]
	rlc [__r0]
	add [__r1],<_task+1
	adc [__r0],>_task+1
	mov A,0
	mvi [__r1],A
	.dbline -2
	.dbline 24
; 	//return ;	return 0 as parent, return pid as child
; }
L1:
	pop X
	.dbline 0 ; func end
	ret
	.dbsym l process -7 pfX
	.dbsym l prev_stack_size -5 c
	.dbsym l process_num -4 c
	.dbend
