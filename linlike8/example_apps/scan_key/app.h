/*
 *	app.h
 *
 *	header file fot his application
 *
 *	03-05-2004	yan	created for testing analog multiplexer in PSoC
 *
 */

 		// user parameters
			// max. available task
#define	NR_TASKS		2							// at least 2 Tasks; if 1 task need only, you can add looping in process1 as
											// while (1) sched_yield(); ==> always release cpu, but process stack size is at least 
			// process0
			//	you can change process0() and process1() to other
extern void process0(void);
#define	idle_process()	process0()
#define	PROCESS0_STACK_SIZE	60
			// process1
extern void scan_key(void);
#define	init_process1	scan_key
#define	PROCESS1_STACK_SIZE	30
			// init. data to zero
			//	Some mcu system will clean all init. RAM to zero auto., however, some they don't
			//	if already done, we can cut the init. code, so define as 0, otherwise 1
#define	AUTO_ZERO	1
			// modulars
#define	TIMER_MOD	1
	#if (TIMER_MOD>0)
	#define	NR_TIMER_OUT	1							// how many sleep() you can call
	#endif

