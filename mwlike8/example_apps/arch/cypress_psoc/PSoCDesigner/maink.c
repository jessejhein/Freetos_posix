/*
 *	init/main.c
 *
 *	09-04-2004	yan	create this following linux
 *
 */

#include "app.h"
#include "sched.h"
#include "system.h"

void start_kernel(void);
extern void setup_arch(void);
extern void sched_init(void);
extern void softirq_init(void);
#if (TIMER_MOD==1)
extern void time_init(void);
#endif
#if (CONTEXT_SW==1)
void kernel_thread(void);
#endif

void start_kernel(void)
{
	setup_arch();
	sched_init();									// <-+ this subr. must place as same layer of process0()
	//fork_init();									//   |
	softirq_init();									//   |
	#if (TIMER_MOD==1)
	time_init();									//   |		// must place after sched_int(), since after timer init, sched can be start to work
	#endif
#if (CONTEXT_SW==1)
	kernel_thread();								//   |
#endif
	sti();										//   |		// start interrupt, kernel start ??? diff. to linux
	idle_process();//for (;;);//cpu_idle();						   <-+ this subr. must place as same layer of sched_init()
}

		// should place at arch/XXX/process.c
#if (CONTEXT_SW==1)
void kernel_thread(void)
{
	do_fork(1, PROCESS0_SYS_APP_STACK_SIZE, (p_func) &init_process);				// create init process
}
#endif

/*
void init_process(void)
{
	
	//do_fork(0);									// create task(s)
	do_fork(1);
	do_fork(2);
	
	while (1) {									// Forever Loop for this parent task
		change_cpu_sleep();
		// yield
	}
}

//#include "../include/linux/main.h"

		// Description : init process which is the super-father of all processes
//void init_process(void);
*/
