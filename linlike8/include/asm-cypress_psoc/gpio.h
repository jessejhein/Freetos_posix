/*
 * Copyright (c) 12-05-2004 cheng chung yan yan@amonics.com> / <chungyan5@hkem.com for 8-bit system
 *
 * the header of gpio in cypress psoc, driver specification. So each arch. has different code here
 * 
 * using cypress psoc gpio
 * for keyboard scanning and ... 
 *
 * header of gpio.h
 *
 *13-10-2004	Hei		add one more parameter "key_deb_f_1" for the PUSH_KEY application
 *
 */

struct gpio_data {
	unsigned timer_10msec_f : 1;
//	unsigned timer_semop : 1;							// semophore for adding timer
#if (KB_MOD==1)
	unsigned key_deb_f : 1;								// deboucing flag of keyboard
	unsigned key_deb_f_1 : 1;								// deboucing flag of keyboard
	#if (PUSH_KEY==1)
	unsigned key_fr_hi_lo : 3;							// bit 0, 1, 2 corres. to enter, up, down
											//	1 as originally high value
											//	0 as originally low value
	#else
	unsigned key_fr_hi_lo : 1;
	#endif
	unsigned scan_key_event : 2;							// keep the events after detect the key
											//	0 -- push button event
											//	1 -- rotary key anti-clockwise
											//	2 -- rotary key clockwise
	unsigned scan_key_timeout : 3;
#endif
};
extern struct gpio_data gpio_var;
#if (KB_MOD==1)
#define	FR_HI_LO_ENTER		0x01
	#if (PUSH_KEY==1)
#define	FR_HI_LO_DOWN		0x02
#define	FR_HI_LO_UP		0x04
	#endif
#endif

