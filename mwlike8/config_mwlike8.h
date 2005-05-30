/*
 * header of configuration for mwlike8
 * i.e. Parameters
 *
 */

// events, MUST >=2 and <16 ==============================================
#define NR_EVENT		2

// LED module ============================================================
#define	LED_MOD			1
#if (LED_MOD>0)
	#define	LED_NUM		3
#endif

                        //      LCD display
#define GUI_LCD         0
