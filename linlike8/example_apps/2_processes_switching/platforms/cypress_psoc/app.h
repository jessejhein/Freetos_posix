/*
 *	app.h
 *
 *	header file for his application
 *
 *	22-04-2004	yan	created
 *
 */

 		// user parameters
		
			// max. available task
#define	NR_TASKS		2							// at least 2 Tasks; if 1 task need only, you can add looping in process1 as
											// while (1) sched_yield(); ==> always release cpu, but process stack size is at least
extern unsigned char next_ptr;
			// process0
			//	this process as the idle task, must always in running State
extern void process0(void);
#define	idle_process()			process0()
#define	PROCESS0_STACK_SIZE		90
#define	let_idle_process_to_run()	next_ptr = NR_TASKS - 1

			// process1
extern void process1(void);
#define	init_process			process1
#define	PROCESS1_STACK_SIZE		90
#define	let_init_process_to_run()	next_ptr = 0

			// platforms 
//#define I386_LINUX
#define CYPRESS_PSOC
//	#define CYPRESS_PSOC_RAM_UNDER_256	1

			// init. data to zero
			//	Some mcu system will clean all init. RAM to zero auto., however, some they don't
			//	if already done, we can cut the init. code, so define as 0, otherwise 1
			//		0 as have not init to zero, must program code to do, i.e. variable = 0;
			//		1 as already done auto zero from system, program code do not need
#define	AUTO_ZERO	1
			// context switch enable
#define	CONTEXT_SW	1
			// AUTO schedule modulars
			//	auto. context switch(done by timer) between process
#define	AUTO_SW_MOD	1
			// Timer modulars
			//	you must modify boot.asm
			//		;ljmp	_Timer8_1_ISR
			//		ljmp	_timer_interrupt
			//	set the number of TIMER_OUT
#define	TIMER_MOD	1
	#if (TIMER_MOD>0)
	#define	NR_TIMER_OUT	3							// how many sleep() you can call
	#endif
			// serial modulars
			//	you must modify boot.asm
			//		;ljmp	_UART_1_RX_ISR
			//		ljmp	_serial_rx_isr
			//		;ljmp	_UART_1_TX_ISR
			//		ljmp	_serial_tx_isr
                        // MAX_SERIAL_BUF must >= 
#define	SERIAL_MOD	1
        #define	MAX_SERIAL_BUF		9
        #define	MAX_SERIAL_TX_BUF	3
			// keyboard modulars
			//	you must modify boot.asm
			//		;ljmp	PSoC_GPIO_ISR
			//		ljmp	_gpio_isr
			//	modify hw configuration depends on kb.h
			//	adding timer in TIMER_MOD
#define	KB_MOD		1
	#if (KB_MOD>0)
			//		enable all pins interrupt
	#define	PUSH_KEY	0
			//		enable 2 pins interrupt only: push key and pin A 
	#define	ROTARY_KEY	1
	#endif
			// i2c modulars
			//	P1.0, P1.1 can not be used, due to hw connected by i2c
#define	I2C_MOD		0
	#if (I2C_MOD>0)
			//	cannot enable both, otherwise system hang
	#define	I2C_MASTER	1
	#define	I2C_SLAVE	0
	#endif
			// switch case mode
#define	SWITCH_CASE	0

