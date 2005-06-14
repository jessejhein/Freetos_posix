/*
 * header of configuration for mwlike8
 * i.e. Parameters
 *
 */

// events, MUST >=2 and <16 ==============================================
#define NR_EVENT		2

// I/O ctrl module, such as LED(s), Buzzer(s) ... ========================
#define	IO_MOD			1
#if (IO_MOD>0)
	#define	IO_NUM		2
#endif

// LCD display ===========================================================
#define LCD_MOD         1
