	.module gpio.c
	.area text(rom, con, rel)
	.dbfile ./interrupt.h
	.dbfunc s mark_bh _mark_bh fV
;             nr -> X-4
_mark_bh:
	.dbline -1
	push X
	mov X,SP
	.dbline 22
; /*
;  * Copyright (c) 24-05-2004 cheng chung yan yan@amonics.com> for 8-bit system
;  *
;  * this module is gpio in cypress psoc, driver specification. So each arch. has different codes here
;  * 
;  * date		author		reason
;  * 26-07-2004	yan		not user friendly : when rotary key running in circular, ENTER key can't detected by interrupt
;  *						    it is because rotary pin mode set as low level interrupt trigger in psoc. 
;  *						    when circular running of rotary key, it is level interrupt until completed rotary one key.
;  *						    so, change the interrupt mode of rotary pin as any changing in psoc
;  * 27-07-2004	yan		gpio interrupt of psoc detect one interrupt from rotary key, but can't detect another enter key at same time
;  *				so, remove the enter key from interrupt mode, change to scan mode in each timer isr
;  * 03-08-2004	yan		improve the rotary encoder
;  */
; 
; #include "app.h"									// this linlike8
; #include <m8c.h>									// M8C_EnableIntMask()
; #include "interrupt.h"									// mark_bh()
; #include "gpio.h"									//
; #if (KB_MOD==1)
; 	#include "kb.h"									// kb hw driver(application dependent, so diff. appl. has diff. kb.h)
; 	#include "gpio_kb_app.h"							// kb app.
	.dbline 23
; 	#include "sched.h"									// schedule_timeout()
	mov [__r0],1
	mov A,[X-4]
	and A,7
	jz X0
X1:
	asl [__r0]
	dec A
	jnz X1
X0:
	mov A,[_softirq_vec]
	or A,[__r0]
	mov [_softirq_vec],A
	.dbline -2
	.dbline 24
; 	#include "timer.h"									// timer_list
L2:
	pop X
	.dbline 0 ; func end
	ret
	.dbsym l nr -4 c
	.dbend
	.dbfile ./gpio.c
	.dbfunc e gpio_open _gpio_open fV
_gpio_open::
	.dbline -1
	.dbline 36
; #endif
; 
; //#include "psoc_gpio_bit.h"								// led_hw_off()
; 
; struct gpio_data gpio_var;
; //#if (KB_MOD==1)
; //struct timer_list deboucing_timer;
; //void kb_deboudcing(void);
; //#endif
; 
; void gpio_open(void)
; {
	.dbline 38
; #if (KB_MOD==1)
;         UPPER_KEY_SET_HIGH;								// init. all pins -- logic high since pull high internal
	or REG[20],8
	.dbline 39
;         ENTER_KEY_SET_HIGH;
	or REG[12],2
	.dbline 40
; 	DOWN_KEY_SET_HIGH;
	or REG[12],8
	.dbline 41
; 	gpio_var.key_deb_f = 0;								// 0 as clr key flag
	and [_gpio_var],-3
	.dbline 42
; 	gpio_var.key_fr_hi_lo = 1;							// init. in pull-high for scan key
	or [_gpio_var],4
	.dbline 47
; 	#if (PUSH_KEY==1)
; 	gpio_var.key_fr_hi_lo |= 0x06;
; 	#endif
; #endif
;         M8C_EnableIntMask( INT_MSK0, INT_MSK0_GPIO );					// prepare GPIO
	or REG[-32],32
	.dbline -2
	.dbline 48
; }
L6:
	.dbline 0 ; func end
	ret
	.dbend
	.dbfunc e gpio_isr _gpio_isr fV
_gpio_isr::
	.dbline -1
	push A
	mov A,[__r0]
	push A
	mov A,[__r1]
	push A
	mov A,[__r2]
	push A
	mov A,[__r3]
	push A
	mov A,[__r4]
	push A
	mov A,[__r5]
	push A
	mov A,[__r6]
	push A
	mov A,[__r7]
	push A
	mov A,[__r8]
	push A
	mov A,[__r9]
	push A
	mov A,[__r10]
	push A
	mov A,[__r11]
	push A
	mov A,[__rX]
	push A
	mov A,[__rY]
	push A
	mov A,[__rZ]
	push A
	.dbline 52
; 
; #pragma	interrupt_handler	gpio_isr						// shouald add this isr name at boot.asm from PSoC Designer
; void gpio_isr(void)
; {
	.dbline 66
; 	
; 	// fast interrupt
; 	
; #if (KB_MOD==1)
; //unsigned char i;
; //buz_hw_on;
; //led_hw_off;
; 	// scan key (this source change back to compatible all platform later)
; 	//******************************************************************************
; //	if (gpio_var.key_fr_hi_lo!=(ENTER_KEY_DET&&1)) {
; //	if (!ENTER_KEY_DET) {
; //		gpio_var.scan_key_event = 0;// interrupt from push button
; 	#if (ROTARY_KEY==1)
; 	if (UPPER_KEY_DET) {								// after interrupt(rising) from pin A
	tst REG[20],8
	jz L8
	.dbline 66
	.dbline 67
; 		if (DOWN_KEY_DET) {							//	if pin B is high, anti-clockwise
	tst REG[12],8
	jz L10
	.dbline 67
	.dbline 68
; 			gpio_var.scan_key_event = 1;
	mov A,[_gpio_var]
	and A,-25
	mov [__r0],A
	or [__r0],8
	mov [_gpio_var],[__r0]
	.dbline 69
	xjmp L11
L10:
	.dbline 69
; 		} else {								//	if pin B is low, clockwise
	.dbline 70
; 			gpio_var.scan_key_event = 2;
	mov A,[_gpio_var]
	and A,-25
	mov [__r0],A
	or [__r0],16
	mov [_gpio_var],[__r0]
	.dbline 71
; 			DOWN_KEY_SET_HIGH;
	or REG[12],8
	.dbline 72
; 		}
L11:
L12:
	.dbline 73
; 		UPPER_KEY_INTR_FALLING;							//	enable pin A interrupt(falling)
	.dbline 73
	or F,0x10  ; iopage = 1
	or REG[22],8
	.dbline 73
	and REG[23],-9
	and F,0xEF ; iopage = 0
	.dbline 73
	.dbline 73
	.dbline 74
	xjmp L9
L8:
	.dbline 74
; 	} else {									// after interrupt(falling) from pin A
L15:
	.dbline 75
	.dbline 75
	or F,0x10  ; iopage = 1
	and REG[22],-9
	.dbline 75
	or REG[23],8
	and F,0xEF ; iopage = 0
	.dbline 75
L16:
	.dbline 75
; 		UPPER_KEY_INTR_RISING;							//	enable pin A interrupt(rising)
	.dbline 76
; 		UPPER_KEY_SET_HIGH;
	or REG[20],8
	.dbline 77
; 	}
L9:
	.dbline 79
; //	UPPER_KEY_SET_HIGH;
; 	ENTER_KEY_SET_HIGH;							//	restore to high level
	or REG[12],2
	.dbline 80
; 	DOWN_KEY_SET_HIGH;
	or REG[12],8
	.dbline 85
; 	#endif
; 
; #endif
; 	
; 	mark_bh(GPIO_BH);								// enable bh gpio task
	mov A,1
	push A
	xcall _mark_bh
	add SP,-1
	.dbline 86
; 	do_softirq();									// botton half of interrupt, if needed, enable interrupt in 2nd-half
	xcall _do_softirq
	.dbline -2
	.dbline 88
; //led_hw_on;
; }
L7:
	pop A
	mov [__rZ],A
	pop A
	mov [__rY],A
	pop A
	mov [__rX],A
	pop A
	mov [__r11],A
	pop A
	mov [__r10],A
	pop A
	mov [__r9],A
	pop A
	mov [__r8],A
	pop A
	mov [__r7],A
	pop A
	mov [__r6],A
	pop A
	mov [__r5],A
	pop A
	mov [__r4],A
	pop A
	mov [__r3],A
	pop A
	mov [__r2],A
	pop A
	mov [__r1],A
	pop A
	mov [__r0],A
	pop A
	.dbline 0 ; func end
	reti
	.dbend
	.dbfunc e gpio_softirq _gpio_softirq fV
;              i -> X+2
;              i -> X+2
_gpio_softirq::
	.dbline -1
	push X
	mov X,SP
	add SP,3
	.dbline 91
; 
; void gpio_softirq(void)
; {
	.dbline 94
	mov [__r1],[_gpio_var]
	mov [__r0],0
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	and [__r1],3
	and [__r0],0
	mov A,[__r1]
	mov [X+1],A
	mov A,[__r0]
	mov [X+0],A
	cmp [X+0],0
	jnz X3
	cmp [X+1],1
	jz L22
X3:
	cmp [X+0],0
	jnz X4
	cmp [X+1],2
	jz L28
X4:
	xjmp L19
X2:
	.dbline 94
; 	
; #if (KB_MOD==1) 
; 	switch (gpio_var.scan_key_event) {
L22:
L23:
	.dbline 106
	.dbline 106
	mov A,2
	push A
	mov A,[_ptr_events_vect]
	asr A
	asr A
	asr A
	asr A
	and A,15
	push A
	mov A,[_ptr_events_vect]
	and A,15
	push A
	xcall _pre_wr_cir254buf
	add SP,-3
	mov [X+2],A
	cmp A,-1
	jz L26
	.dbline 106
	.dbline 106
	mov [__r1],[_ptr_events_vect]
	mov [__r0],0
	and [__r1],15
	and [__r0],0
	add [__r1],<_events_vect
	adc [__r0],>_events_vect
	mvi A,[__r1]
	dec [__r1]
	mov [__r2],A
	and [__r2],-8
	or [__r2],2
	mov A,[__r2]
	mvi [__r1],A
	.dbline 106
	mov [__r1],[_ptr_events_vect]
	mov [__r0],0
	and [__r1],15
	and [__r0],0
	add [__r1],<_events_vect
	adc [__r0],>_events_vect
	mvi A,[__r1]
	dec [__r1]
	mov [__r2],A
	and [__r2],-25
	or [__r2],16
	mov A,[__r2]
	mvi [__r1],A
	.dbline 106
	mov A,[_ptr_events_vect]
	and A,-16
	mov [__r0],A
	mov A,[X+2]
	and A,15
	or [__r0],A
	mov [_ptr_events_vect],[__r0]
	.dbline 106
L26:
	.dbline 106
L24:
	.dbline 106
; //		case 0 : 
; //			 if (gpio_var.key_deb_f==0) {
; 					// setting deboucing timer
; //				deboucing_timer.data.timer_function = (p_func) kb_deboudcing;//after solve the problem, put back into alarm()
; //				if (malarm((p_func) kb_deboudcing, 10, &deboucing_timer)>=0)		// 10mSec is enough for push key deboucing
; //					gpio_var.key_deb_f = 1; 
; //			 }
; //			break;
; 	#if (ROTARY_KEY==1)
; 		case 1 : 
; 			// appl. level code here for DOWN key in DOWN case
; 			 APP_DOWN_KEY_DOWN_CASE;
	.dbline 107
; 			 gpio_var.scan_key_event = 0;					// clr the previous key-in
	and [_gpio_var],-25
	mov A,[_gpio_var]
	.dbline 108
; 			break;
	xjmp L20
L28:
L29:
	.dbline 111
	.dbline 111
	mov A,2
	push A
	mov A,[_ptr_events_vect]
	asr A
	asr A
	asr A
	asr A
	and A,15
	push A
	mov A,[_ptr_events_vect]
	and A,15
	push A
	xcall _pre_wr_cir254buf
	add SP,-3
	mov [X+2],A
	cmp A,-1
	jz L32
	.dbline 111
	.dbline 111
	mov [__r1],[_ptr_events_vect]
	mov [__r0],0
	and [__r1],15
	and [__r0],0
	add [__r1],<_events_vect
	adc [__r0],>_events_vect
	mvi A,[__r1]
	dec [__r1]
	mov [__r2],A
	and [__r2],-8
	or [__r2],2
	mov A,[__r2]
	mvi [__r1],A
	.dbline 111
	mov [__r1],[_ptr_events_vect]
	mov [__r0],0
	and [__r1],15
	and [__r0],0
	add [__r1],<_events_vect
	adc [__r0],>_events_vect
	mvi A,[__r1]
	dec [__r1]
	mov [__r2],A
	and [__r2],-25
	or [__r2],8
	mov A,[__r2]
	mvi [__r1],A
	.dbline 111
	mov A,[_ptr_events_vect]
	and A,-16
	mov [__r0],A
	mov A,[X+2]
	and A,15
	or [__r0],A
	mov [_ptr_events_vect],[__r0]
	.dbline 111
L32:
	.dbline 111
L30:
	.dbline 111
; 		case 2 : 
; 			// appl. level code here for UP key in DOWN case
; 			 APP_UP_KEY_DOWN_CASE;
	.dbline 112
; 			 gpio_var.scan_key_event = 0;					// clr the previous key-in
	and [_gpio_var],-25
	.dbline 113
; 			break;
L19:
L20:
	.dbline -2
	.dbline 118
; 	#endif
; 	}
; #endif
; 
; }
L18:
	add SP,-3
	pop X
	.dbline 0 ; func end
	ret
	.dbsym l i 2 c
	.dbsym l i 2 c
	.dbend
	.area bss(ram, con, rel)
	.dbfile ./gpio.c
_gpio_var::
	.blkb 1
	.dbsym e gpio_var _gpio_var X
