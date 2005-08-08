/*
 *	app.h
 *
 *	header file for this application definition
 *
 *	History
 *	0.01.03	31-08-2004	yan	modules from os, gui, appl.
 *					multi mode
 *	1-11-2004		Hei 	modify for the current feedback and the driving current application
 * 	28-4-2005		yan		modify for uClinux platform
 */	

	#define CYPRESS_PSOC_RAM_UNDER_256	1


		// os 
		//=========================================================================

			// max. available task
#define	NR_TASKS		1							// at least 2 Tasks; if 1 task need only, you can add looping in process1 as
											// while (1) sched_yield(); ==> always release cpu, but process stack size is at least
extern unsigned char next_ptr;
			// process0
			//	this process as the idle task, must always in running State
#define	PROCESS0_STACK_SIZE	177
//>85
#define	let_idle_process_to_run()	next_ptr = NR_TASKS - 1

			// process1
extern void mwlike8_process1(void);
#define	init_process	mwlike8_process1
#define	PROCESS1_STACK_SIZE	6
#define	let_init_process_to_run()	next_ptr = 0

			// AUTO schedule modulars
			//	auto. context switch(done by timer) between process
#define	AUTO_SW_MOD	0
			// Timer modulars
			//	you must modify boot.asm : "ljmp	_timer_interrupt"
			//		;ljmp	_Timer8_1_ISR
			//		ljmp	_timer_interrupt
			//	set the number of TIMER_OUT
			//		//KB_MOD
			//		on/off step timer
			// mode
#define MODE_NUM		1							// how many modes are there?
			// analog interface
			//	any modification of these number, must modify the code for accessing
			//		adc_status.which size
#define MAX_DRV_VAL0		200
#define MAX_DRV_VAL1		200
#define MAX_DRV_VAL2		200
#define MAX_DRV_VAL3		200
			// ui
//#define	LOGO			" Renka Corp."
			// laser 
//#define	ERR_FLAG_LASER_NUM_DIFF	12
#define direct_drv	1
#define current_feedback	0
#if (direct_drv>0)
#define	LASER_ON_OFF_VAL	6
#define	on_off_status()	(drvVal[laser.num]>LASER_ON_OFF_VAL)
#endif
#if (current_feedback>0)
#define	LASER_ON_OFF_VAL	290				//cur_det dac value 
extern unsigned int cur_det[MAX_CUR];
#define	on_off_status()	(cur_det[laser.num]>LASER_ON_OFF_VAL)
#endif
