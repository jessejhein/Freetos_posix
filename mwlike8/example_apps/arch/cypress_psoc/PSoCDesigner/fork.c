/*
 *	linlike8/kernel/fork.c
 *
 *	10-04-2004	yan	create this following linux
 *
 *	fork new process
 */

#include "app.h"
#include "sched.h"
#include "current.h"									// MUST be placed after sched.h
#include "system.h"									// MUST be placed after sched.h

#if (CONTEXT_SW==1)
		// process_num - hardcode number, beware to not double
		// get the previous process stack size, then cal. the new process stack starting point
void do_fork(unsigned char process_num, unsigned char prev_stack_size, p_func process)
{
	//task[process_num] = *current;							// allocate memory for new process, then copy the existing process content to new one
	task[process_num].stack_ptr = task[process_num-1].stack_ptr + prev_stack_size;
	place_process_stack();
	wake_up_process((&task[process_num]));
	//return ;	return 0 as parent, return pid as child
}
#endif

