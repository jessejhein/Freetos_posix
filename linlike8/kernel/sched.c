/*
 *	kernel/sched.c
 *
 *	Kernel scheduler and related syscalls
 *
 *	09-04-2004	yan	create this following linux
 */

// including, get data linking from others ===============================================
//	appl. declaration
#include <pin_define.h>

#include "app.h"
#include "sched.h"
#include "current.h"									// MUST be placed after sched.h
#include <asm/system.h>
#include "timer.h"									// timer_list

#if (CONTEXT_SW==1)
void sched_init(void);
struct task_struct task[NR_TASKS];
unsigned char current_ptr;
unsigned char next_ptr;
#endif

#if (AUTO_ZERO==0)
extern void init_timer(void);
#endif

#if (CONTEXT_SW==1)
void sched_init(void)
{
	//unsigned char cpu = smp_processor_id();					// get existing process id
	//init_task.processor = cpu;

#if (AUTO_ZERO==0)
	init_timer();
#endif

				// the following does not same to linux
				// init. process 0
#if (AUTO_ZERO==0)
	current_ptr = 0;
	next_ptr = 0;
#endif
	get_process0_sp();								// task[0].stack_ptr = SP, since Process0 is also starting from this layer, see main.c
	wake_up_process(current);
}
#endif	// (CONTEXT_SW==1)

//void wake_up_process(struct task_struct* p)
//{
//	p->state = TASK_RUNNING;
//}

#if (CONTEXT_SW==1)
		// Description : each time kernel arrange different processes to run, this schedule must be called, so try to min. the code and RAM
		//			global variables declaration, since reduce time to declar in local and save memory in each time stack
struct task_struct *prev;						
void schedule(void)
{
	cli();
	prev = current;
	do {										// seek next running state process
		if (next_ptr==0) {								//	just normal sched for next
			current_ptr++;							//		method A => choose this method, since it is more save RAM and ROM code studied from assembly code, at least in cypress PSoC, verify other mcus, and think about what is the good implement method from C
			if (current_ptr==NR_TASKS) current_ptr = 0;
			//current_ptr = ((current_ptr++) == NR_TASKS) ? 0 :;		//		method B
		} else {								//	any urge process
			current_ptr = next_ptr;
			next_ptr = 0;
		}
	} while (current->state!=TASK_RUNNING);
	if (prev!=current) switch_to();
	sti();
}

//void sys_sched_yield(void)
//{
//
//}
//void sched_yield(void)
//{
	//set_current_state(TASK_RUNNING);						// already in running state
	//sys_sched_yield();
//	schedule();
//}

#if (TIMER_MOD==1)                    
		// Remarks 
		//	* max. of time for sleep is 256Sec
extern char add_timer(struct timer_list* ptimer_addr);
//extern void del_timer(unsigned char timer_num);
/*void schedule_timeout(unsigned char sec)
{
	unsigned char i;
	struct timer_list timer;
	current->state = TASK_INTERRUPTIBLE;
	timer.expires = sec * 100 + jiffies;						// since 10mSec for 1 tick, so X100
	timer.data = (unsigned int) current;
	//timer.function = process_timeout;
	i = add_timer(&timer);
	schedule();
	del_timer(i);
}*/
		// must in times of 10mSec
void schedule_timeout(unsigned char millisec_time_10)
{
	unsigned char i;
	struct timer_list timer;
	timer.expires = millisec_time_10 + jiffies;					// since 10mSec for 1 tick, so X100
	timer.data.timer_data = (0xff00 | ((unsigned int) current_ptr));		// related to timer.c only
	//timer.data = (unsigned int) current;
	if ((i = add_timer(&timer))>=0) current->state = TASK_INTERRUPTIBLE;
	schedule();									// when switching, struct timer_list timer staying be kept
	if (i>=0) del_timer(i);
}

#endif
#endif

