	.module maink.c
	.area text(rom, con, rel)
	.dbfile ./maink.c
	.dbfunc e start_kernel _start_kernel fV
_start_kernel::
	.dbline -1
	.dbline 24
; /*
;  *	init/main.c
;  *
;  *	09-04-2004	yan	create this following linux
;  *
;  */
; 
; #include "app.h"
; #include "sched.h"
; #include "system.h"
; 
; void start_kernel(void);
; extern void setup_arch(void);
; extern void sched_init(void);
; extern void softirq_init(void);
; #if (TIMER_MOD==1)
; extern void time_init(void);
; #endif
; #if (CONTEXT_SW==1)
; void kernel_thread(void);
; #endif
; 
; void start_kernel(void)
; {
	.dbline 25
; 	setup_arch();
	xcall _setup_arch
	.dbline 26
; 	sched_init();									// <-+ this subr. must place as same layer of process0()
	xcall _sched_init
	.dbline 28
; 	//fork_init();									//   |
; 	softirq_init();									//   |
	xcall _softirq_init
	.dbline 30
; 	#if (TIMER_MOD==1)
; 	time_init();									//   |		// must place after sched_int(), since after timer init, sched can be start to work
	xcall _time_init
	.dbline 33
; 	#endif
; #if (CONTEXT_SW==1)
; 	kernel_thread();								//   |
	xcall _kernel_thread
	.dbline 35
; #endif
; 	sti();										//   |		// start interrupt, kernel start ??? diff. to linux
		or  F, 01h

	.dbline 36
; 	idle_process();//for (;;);//cpu_idle();						   <-+ this subr. must place as same layer of sched_init()
	xcall _mwlike8_server
	.dbline -2
	.dbline 37
; }
L1:
	.dbline 0 ; func end
	ret
	.dbend
	.dbfunc e kernel_thread _kernel_thread fV
_kernel_thread::
	.dbline -1
	.dbline 42
; 
; 		// should place at arch/XXX/process.c
; #if (CONTEXT_SW==1)
; void kernel_thread(void)
; {
	.dbline 43
; 	do_fork(1, PROCESS0_SYS_APP_STACK_SIZE, (p_func) &init_process);				// create init process
	mov A,>PL_mwlike8_process1
	push A
	mov A,<PL_mwlike8_process1
	push A
	mov A,83
	push A
	mov A,1
	push A
	xcall _do_fork
	add SP,-4
	.dbline -2
	.dbline 44
; }
L2:
	.dbline 0 ; func end
	ret
	.dbend
	.area func_lit
PL_mwlike8_process1:	.word _mwlike8_process1
