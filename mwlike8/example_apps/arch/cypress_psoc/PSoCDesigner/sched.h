/*
 *	header of schedule
 *	
 *	include/linlike8/sched.h
 *
 *	Kernel scheduler and related syscalls
 *
 *	09-04-2004	yan	create this following linux
 *
 */

 											// state of process
#define	TASK_RUNNING		0							//	ready for running / already in running
#define	TASK_INTERRUPTIBLE	1							//	sleep mode, but can be wake up by signal
#define	TASK_STOPPED		8
struct task_struct {
	unsigned char stack_ptr;							// task context data, must follow order, since hardcode to save each data
//	unsigned char acc;
//	unsigned char index;
//	unsigned char flag;
//	unsigned int PC;
	unsigned char state;
	//unsigned char processor;							// cpus_runnable is ~0 if the process is not running on any
											// CPU. It's (1 << cpu) if it's running on a CPU. This mask
											// is updated under the runqueue lock.
											// To determine whether a process might run on a CPU, this
											// mask is AND-ed with cpus_allowed.
};
extern struct task_struct task[];

#define	set_current_state(state_value)	do { current->state = (state_value); } while (0)

//extern void wake_up_process(struct task_struct* p);
#define	wake_up_process(p)	p->state = TASK_RUNNING
typedef void const (*p_func)(void);				// this specific for psoc, later to change to general
		// Description : fork a new process 
		// Parameters : process_num - process number, manually to create this
		//		prev_stack_size - stack size of previous process
		//		process - process starting address
extern void do_fork(unsigned char process_num, unsigned char prev_stack_size, p_func process);	// this specific for psoc, later to change to general
//extern void sched_yield(void);
#define	sched_yield()	schedule()
extern void schedule(void);

#if (TIMER_MOD==1)
extern unsigned char jiffies;
extern void schedule_timeout(unsigned char millisec_time_10);	//extern void schedule_timeout(unsigned char sec);
extern void do_timer(void);
#endif

