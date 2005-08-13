/*
 * header of configuration for linlike8
 * i.e. Parameters
 *
 */

// scheduling ===========================================================
#define	SWITCH_CASE			1	// 1 as turn ON switch case mode, otherwise 0
#define	CONTEXT_SW			0	// context switch enable

#define	AUTO_ZERO			1	// init. data to zero
								//	Some mcu system will clean all init. RAM to zero auto., however, some they don't
								//	if already done, we can cut the init. code for variable = 0;, so define
								//		0 as have not init to zero, must program code to do, i.e. variable = 0;
								//		1 as already done auto zero from system, program code do not need

// start function for user application ====================================
extern void mwlike8_server(void);
#define	idle_process()	mwlike8_server()

// uart module ===========================================================
#define	UART_MOD						0

#if (UART_MOD>0)
#define MAX_PACKET_DATA_SIZE    6
//		bytes position in package
//		uart buffer size
	#define	MAX_UART_BUF				12	// must >= 9 in laser control application
	#define	MAX_UART_TX_BUF				8	// >= 2
//		RS485 mode
	#define	RS485						0	// enable RS485 or not
//		uart master
#define	UART_MASTER						1 
//		uart slave 
#define UART_SLAVE						0
#if (UART_SLAVE == 1)
		#define LOCAL_SLAVE_ID			1
#endif

#endif

//	I2C module ===========================================================
#define	I2C_MOD							0
//		CAUTION : cannot enable master and slave at same time, otherwise system hang
#if (I2C_MOD==1)
//		master
	#define	I2C_MASTER					1
	#define I2C_MULTI_MASTER			0
	#define I2C_MULTI_MASTER_SLAVE		0
	#if (I2C_MULTI_MASTER_SLAVE==1)
			#define LOCAL_SLAVE_ADDRESS	0
	#endif
//		slave
	#define	I2C_SLAVE					0
	#if (I2C_SLAVE==1)
		#define LOCAL_SLAVE_ID			1
	#endif
#endif

//	keyboard module ========================================================
//		you must modify boot.asm
//			;ljmp	PSoC_GPIO_ISR
//			ljmp	_gpio_isr
//		modify hw configuration depends on kb.h
//		adding timer in TIMER_MOD
#define	KB_MOD							0
#if (KB_MOD>0)
//			enable all pins interrupt
#define	PUSH_KEY	0
//			enable 2 pins interrupt only: push key and pin A 
#define	ROTARY_KEY	1
#endif

//	Timer module ========================================================
//		you must modify boot.asm : "ljmp	_timer_interrupt"
//			;ljmp	_Timer8_1_ISR
//			ljmp	_timer_interrupt
//		set the number of TIMER_OUT
//			//KB_MOD
//			on/off step timer
#define	TIMER_MOD					0
	#if (TIMER_MOD>0)
	#define	NR_TIMER_OUT			0				// how many sleep() you can call
			//	for mwlike8, you must have ONE NR_TIMER_OUT at least
	#endif

//	eeprom module ========================================================
#define	EEPROM_MOD					0