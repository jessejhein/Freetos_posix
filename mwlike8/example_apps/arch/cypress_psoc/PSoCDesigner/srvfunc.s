	.module srvfunc.c
	.area text(rom, con, rel)
	.dbfile ./srvfunc.c
	.dbfunc e io_open _io_open fV
_io_open::
	.dbline -1
	.dbline 34
; /*
;  * Copyright (c) 1999, 2000, 2001, 2002, 2003 Greg Haerr <greg@censoft.com>
;  * Portions Copyright (c) 2002 by Koninklijke Philips Electronics N.V.
;  * Copyright (c) 2000 Alex Holden <alex@linuxhacker.org>
;  * Copyright (c) 1991 David I. Bell
;  * Copyright (c) 2004 cheng chung yan <chungyan5@hkem.com / yan@amonics.com> for 8-bit system
;  * Permission is granted to use, distribute, or modify this source,
;  * provided that this copyright notice remains intact.
;  */
; 
; #include "app.h"
; #include "nano-X.h"
; #include "sched.h"									// timer.h
; #include "timer.h"									// msleep()
; #include "psoc_gpio_bit.h"								// led_hw_off()
; #include "cirbuf.h"									// pre_wr_cir254buf()
; 
; //#include "serial.h"
; 
; 		// LED and Buzzer
; //struct io_bit_struct buzzer_ctrl;
; //struct io_bit_struct led_ctrl;
; struct io_bit_struct io_bit_ctrl;
; #if (ADD_LED>0)
; struct ADD_LED_STRUCT add_led;
; #endif
; 		// events
; GR_EVENT events_vect[NR_EVENT];
; struct EVENT_PTR ptr_events_vect;
; //unsigned char wr_ptr_events_vect;
; //unsigned char rd_ptr_events_vect;
; 
; void io_open(void)
; {
	.dbline 35
; 	buz_hw_off;									// hw init.
	and REG[8],-9
	.dbline 36
; 	led_hw_off;
	or REG[8],-128
	.dbline 37
; 	buz_off();
	and [_io_bit_ctrl],-32
	.dbline 38
; 	led_off();
	.dbline 49
; #if (ADD_LED>0)
; 	led_1st_off();
; 	#if (ADD_LED>1)
; 	led_2nd_off();
; 	#endif
; 	#if (ADD_LED>2)
; 	led_3rd_off();
; 	#endif
; #endif
; 	#if (SWITCH_CASE==1)
; 	io_bit_ctrl.GrGetNextEventTimeout_status = 0;
	.dbline -2
	.dbline 51
; 	#endif
; }
L4:
	.dbline 0 ; func end
	ret
	.dbend
	.area bss(ram, con, rel)
	.dbfile ./srvfunc.c
L6:
	.blkb 1
L7:
	.blkb 2
	.area text(rom, con, rel)
	.dbfile ./srvfunc.c
	.dbfunc e GrGetNextEventTimeout _GrGetNextEventTimeout fc
	.dbsym s ggnet_timer L7 i
	.dbsym s ggnet_i L6 c
;              k -> X+0
;        timeout -> X-7
;             ep -> X-5
_GrGetNextEventTimeout::
	.dbline -1
	push X
	mov X,SP
	add SP,7
	.dbline 187
; 
; // return -1, event buf. full; all others as completed in this number index
; /*char wr_event(char event_type)
; {
; 	if ((i=pre_wr_cir254buf(wr_ptr_events_vect,rd_ptr_events_vect,NR_EVENT))!=255) {
; 		events_vect[wr_ptr_events_vect].type = event_type;
; 		wr_ptr_events_vect = i;
; 	}
; 	//
; 	unsigned char tmp1 = wr_ptr_events_vect;
; 	unsigned char tmp = (wr_ptr_events_vect==NR_EVENT) ? 0 : wr_ptr_events_vect + 1;
; 	if (tmp==rd_ptr_events_vect) return ((char) -1);
; 	events_vect[wr_ptr_events_vect].type = event_type;
; 	wr_ptr_events_vect = tmp;
; 	return tmp1;
; }*/
; 
; /*
;  * Map the window to make it (and possibly its children) visible on the screen.
;  * (this subr. is placed in server side, but be called from client, i.e. coding running in client side)
;  */
; //void
; //GrMapWindow(void)
; //{
; //	unsigned char i;
; //	if ((i=pre_wr_cir254buf(ptr_events_vect.wr,ptr_events_vect.rd,NR_EVENT))!=255) {
; //		events_vect[ptr_events_vect.wr].type = GR_EVENT_TYPE_EXPOSURE;
; //		ptr_events_vect.wr = i;
; //	}
; //}
; 
; /*
;  * Return the next event from the event queue, or
;  * wait for a new one if one is not ready.  If timeout
;  * is nonzero, return timeout event if time elapsed.
;  * timeout in millisecond
;  * Assume the ep event as zero content
;  * (this subr. is placed in server side, but be called from client, i.e. coding running in client side)
;  * max. timeout is 12500mSec
;  */
; #define TIME_OUT_INTERVAL	50							// in mill. second
; #if (SWITCH_CASE==0)
; void
; GrGetNextEventTimeout(GR_EVENT *ep, GR_TIMEOUT timeout)
; {
; 	unsigned char max_time_out = timeout/TIME_OUT_INTERVAL;
; 	unsigned char i = 0;
; 	//unsigned char buz_j;
; 	//unsigned char led_j;
; 	unsigned char k;								// for circular buffer
; 	ep->type = GR_EVENT_TYPE_NONE;
; 	
; 	do {
; 		// sleep regular time as TIME_OUT_INTERVAL
; 		msleep(TIME_OUT_INTERVAL);
; 		
; 		// check event job 
; 		if ((k=pre_rd_cir254buf((unsigned char) ptr_events_vect.wr,(unsigned char) ptr_events_vect.rd,NR_EVENT))==255) {	// no event
; 			if (i==max_time_out) ep->type = GR_EVENT_TYPE_TIMEOUT;
; 			else i++;
; 		} else {										// event occurs
; 			*ep = events_vect[(unsigned char) ptr_events_vect.rd];
; 			//ep->type = events_vect[rd_ptr_events_vect].type;
; 			ptr_events_vect.rd = k;
; 		}
; 		
; 		// buzzer control job
; 		switch (io_bit_ctrl.buzzer_stat) {
; 			case BUZZER_OFF :
; 				 buz_hw_off;
; 				break;
; 			case BUZZER_POS_PULSE :
; 				 buz_hw_on;
; 				 //io_bit_ctrl.buzzer_stat = BUZZER_POS_PULSE_1;
; 				 io_bit_ctrl.buzzer_stat = BUZZER_OFF;
; 				break;
; 			/*case BUZZER_POS_PULSE_1 :
; 				 if (max_time_out>4) {					// 4 as 4 X 50mSec = 200mSec
; 					 if (i>=4) io_bit_ctrl.buzzer_stat = BUZZER_OFF;
; 				 } else {
; 					 if (i>=max_time_out) io_bit_ctrl.buzzer_stat = BUZZER_OFF;
; 				 }
; 				break;
; 			case BUZZER_REP_PULSE :
; 				 buz_j = buzzer_ctrl.cnt;
; 				 buzzer_ctrl.stat = BUZZER_REP_PULSE_1;
; 				break;
; 			case BUZZER_REP_PULSE_1 :
; 				 if ((buz_j--)>(buzzer_ctrl.cnt/2)) {
; 					buz_hw_on;
; 				 } else {
; 					if (buz_j==0) buz_j = buzzer_ctrl.cnt;
; 					buz_hw_off;
; 				 }
; 				break;*/
; 		}
; 		
; 		// led control job
; 		switch (io_bit_ctrl.led_stat) {
; 			case LED_OFF :
; 				 led_hw_off;
; 				break;
; 			case LED_POS_PULSE :
; 				 led_hw_on;
; 				 io_bit_ctrl.led_stat = LED_OFF;
; 				break;
; 			case LED_ON :
; 				 led_hw_on;
; 				break;
; 			case LED_NEG_PULSE :
; 				 led_hw_off;
; 				 io_bit_ctrl.led_stat = LED_ON;
; 				break;
; 			/*case LED_REP_PULSE :
; 				 led_j = led_ctrl.cnt;
; 				 led_ctrl.stat = LED_REP_PULSE_1;
; 				break;
; 			case LED_REP_PULSE_1 :
; 				 if ((led_j--)>(led_ctrl.cnt/2)) {
; 					led_hw_on;
; 				 } else {
; 					if (led_j==0) led_j = led_ctrl.cnt;
; 					led_hw_off;
; 				 }
; 				break;*/
; 		}
; 		
; 	} while (ep->type==GR_EVENT_TYPE_NONE);
; }
; #else
; //void GrGetNextEventTimeout_timeout_alarm(void);
; // return 
; //	GR_EVENT_TYPE_NONE -- not completed
; //	NOT GR_EVENT_TYPE_NONE -- valid event
; GR_EVENT_TYPE GrGetNextEventTimeout(GR_EVENT *ep, GR_TIMEOUT timeout)
; {
	.dbline 192
; 	unsigned char k;								// for circular buffer
; 	static unsigned char ggnet_i;
; 	//static struct timer_list ggnet_timer;
; 	static unsigned int ggnet_timer;
; 	ep->type = GR_EVENT_TYPE_NONE;
	mov A,[X-4]
	mov [__r1],A
	mov A,[X-4]
	mov [__r3],A
	mvi A,[__r3]
	and A,-8
	mvi [__r1],A
	.dbline 193
	mov [__r1],[_io_bit_ctrl]
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
	mov [X+2],A
	mov A,[__r0]
	mov [X+1],A
	cmp [X+1],0
	jnz X3
	cmp [X+2],0
	jz L11
X3:
	cmp [X+1],0
	jnz X4
	cmp [X+2],1
	jz L12
X4:
	cmp [X+1],0
	jnz X5
	cmp [X+2],2
	jz L13
X5:
	cmp [X+1],0
	jnz X6
	cmp [X+2],3
	jz L16
X6:
	xjmp L8
X0:
	.dbline 193
; 	switch (io_bit_ctrl.GrGetNextEventTimeout_status) {
L11:
	.dbline 195
; 		case 0 :
; 			 ggnet_i = 0;
	mov [L6],0
	.dbline 196
; 			 io_bit_ctrl.GrGetNextEventTimeout_status = 1;
	mov A,[_io_bit_ctrl]
	and A,-25
	mov [__r0],A
	or [__r0],8
	mov [_io_bit_ctrl],[__r0]
	.dbline 197
; 			break;
	xjmp L9
L12:
	.dbline 202
; 		case 1 :
; 			 //ggnet_timer.data.timer_function = (p_func) GrGetNextEventTimeout_timeout_alarm;//after solve the problem, put back into alarm()
; 			 //if (malarm((p_func) GrGetNextEventTimeout_timeout_alarm, TIME_OUT_INTERVAL, &ggnet_timer)>=0)// start timer, sleep regular time as TIME_OUT_INTERVAL
; 			 //	io_bit_ctrl.GrGetNextEventTimeout_status = 2;
; 			 ggnet_timer = 0;
	mov [L7+1],0
	mov [L7],0
	.dbline 203
; 			 io_bit_ctrl.GrGetNextEventTimeout_status = 2;
	mov A,[_io_bit_ctrl]
	and A,-25
	mov [__r0],A
	or [__r0],16
	mov [_io_bit_ctrl],[__r0]
	.dbline 204
; 			break;
	xjmp L9
L13:
	.dbline 206
; 		case 2 :								// wait alarm for each timeout
; 			 if ((++ggnet_timer)==1024) io_bit_ctrl.GrGetNextEventTimeout_status = 3;
	mov A,[L7+1]
	add A,1
	mov [__r1],A
	mov A,[L7]
	adc A,0
	mov [__r0],A
	mov [L7+1],[__r1]
	mov [L7],A
	cmp [L7],4
	jnz L9
	cmp [__r1],0
	jnz L9
X7:
	.dbline 206
	or [_io_bit_ctrl],24
	mov A,[_io_bit_ctrl]
	.dbline 207
; 			break;
	xjmp L9
L16:
	.dbline 209
; 		case 3 :
; 			 io_bit_ctrl.GrGetNextEventTimeout_status = 0;			// check event job
	and [_io_bit_ctrl],-25
	.dbline 210
; 			 if ((k=pre_rd_cir254buf((unsigned char) ptr_events_vect.wr,(unsigned char) ptr_events_vect.rd,NR_EVENT))==255) {// no event
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
	xcall _pre_rd_cir254buf
	add SP,-3
	mov [X+0],A
	cmp A,-1
	jnz L17
	.dbline 210
	.dbline 211
; 				 if (ggnet_i==timeout/TIME_OUT_INTERVAL) {
	mov [__r1],[L6]
	mov A,0
	push A
	mov A,50
	push A
	mov A,[X-7]
	push A
	mov A,[X-6]
	push A
	xcall __divmodu_16X16_16
	pop A
	mov [__r3],A
	pop A
	mov [__r2],A
	add SP,-2
	mov A,0
	cmp A,[__r2]
	jnz L19
	mov A,[__r1]
	cmp A,[__r3]
	jnz L19
X8:
	.dbline 211
	.dbline 212
; 					 ep->type = GR_EVENT_TYPE_TIMEOUT;
	mov A,[X-4]
	mov [__r1],A
	mov A,[X-4]
	mov [__r3],A
	mvi A,[__r3]
	mov [__r2],A
	and [__r2],-8
	or [__r2],4
	mov A,[__r2]
	mvi [__r1],A
	.dbline 213
; 				 }
	xjmp L18
L19:
	.dbline 214
; 				 else {
	.dbline 215
; 					 ggnet_i++;
	inc [L6]
	.dbline 216
; 					 io_bit_ctrl.GrGetNextEventTimeout_status = 1;
	mov A,[_io_bit_ctrl]
	and A,-25
	mov [__r0],A
	or [__r0],8
	mov [_io_bit_ctrl],[__r0]
	.dbline 217
; 				 }
	.dbline 218
	xjmp L18
L17:
	.dbline 218
; 			 } else {							//	event occurs
	.dbline 219
; 				*ep = events_vect[(unsigned char) ptr_events_vect.rd];
	mov A,[X-4]
	mov [__r1],A
	mov [__r3],[_ptr_events_vect]
	mov [__r2],0
	asr [__r2]
	rrc [__r3]
	asr [__r2]
	rrc [__r3]
	asr [__r2]
	rrc [__r3]
	asr [__r2]
	rrc [__r3]
	and [__r3],15
	and [__r2],0
	add [__r3],<_events_vect
	adc [__r2],>_events_vect
	mvi A,[__r3]
	mvi [__r1],A
	.dbline 220
; 				ptr_events_vect.rd = k;
	mov A,[_ptr_events_vect]
	and A,15
	mov [__r0],A
	mov A,[X+0]
	and A,15
	asl A
	asl A
	asl A
	asl A
	and A,-16
	or [__r0],A
	mov [_ptr_events_vect],[__r0]
	.dbline 221
; 			 }
L18:
	.dbline 224
	mov [__r1],[_io_bit_ctrl]
	mov [__r0],0
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	and [__r1],1
	and [__r0],0
	mov A,[__r1]
	mov [X+4],A
	mov A,[__r0]
	mov [X+3],A
	cmp [X+3],0
	jnz X9
	cmp [X+4],0
	jz L24
X9:
	cmp [X+3],0
	jnz X10
	cmp [X+4],1
	jz L25
X10:
	xjmp L21
X1:
	.dbline 224
; 
; 			 // buzzer control job
; 			 switch (io_bit_ctrl.buzzer_stat) {
L24:
	.dbline 226
; 				case BUZZER_OFF :
; 					 buz_hw_off;
	and REG[8],-9
	.dbline 227
; 					break;
	xjmp L22
L25:
	.dbline 229
; 				case BUZZER_POS_PULSE :
; 					 buz_hw_on;
	or REG[8],8
	.dbline 231
; 					 //io_bit_ctrl.buzzer_stat = BUZZER_POS_PULSE_1;
; 					 io_bit_ctrl.buzzer_stat = BUZZER_OFF;
	and [_io_bit_ctrl],-5
	.dbline 232
; 					break;
L21:
L22:
	.dbline 256
	mov [__r1],[_io_bit_ctrl]
	mov [__r0],0
	and [__r1],3
	and [__r0],0
	mov A,[__r1]
	mov [X+6],A
	mov A,[__r0]
	mov [X+5],A
	cmp [X+5],0
	jnz X11
	cmp [X+6],0
	jz L29
X11:
	cmp [X+5],0
	jnz X12
	cmp [X+6],1
	jz L30
X12:
	cmp [X+5],0
	jnz X13
	cmp [X+6],2
	jz L31
X13:
	cmp [X+5],0
	jnz X14
	cmp [X+6],3
	jz L32
X14:
	xjmp L26
X2:
	.dbline 256
; /*				case BUZZER_POS_PULSE_1 :
; 					 k = timeout/TIME_OUT_INTERVAL;
; 					 if (k>2) {					// 4 as 4 X 50mSec = 200mSec
; 						 if (ggnet_i>=2) io_bit_ctrl.buzzer_stat = BUZZER_OFF;
; 					 } else {
; 						 if (ggnet_i>=k) io_bit_ctrl.buzzer_stat = BUZZER_OFF;
; 					 }
; 					break;*/
; 				/*case BUZZER_REP_PULSE :
; 					 buz_j = buzzer_ctrl.cnt;
; 					 buzzer_ctrl.stat = BUZZER_REP_PULSE_1;
; 					break;
; 				case BUZZER_REP_PULSE_1 :
; 					 if ((buz_j--)>(buzzer_ctrl.cnt/2)) {
; 						buz_hw_on;
; 					 } else {
; 						if (buz_j==0) buz_j = buzzer_ctrl.cnt;
; 						buz_hw_off;
; 					 }
; 					break;*/
; 			 }
; 			
; 			 // led control job
; 			 switch (io_bit_ctrl.led_stat) {
L29:
	.dbline 258
; 				case LED_OFF :
; 					 led_hw_off;
	or REG[8],-128
	.dbline 259
; 					break;
	xjmp L27
L30:
	.dbline 261
; 				case LED_POS_PULSE :
; 					 led_hw_on;
	and REG[8],127
	.dbline 262
; 					 io_bit_ctrl.led_stat = LED_OFF;
	and [_io_bit_ctrl],-4
	mov A,[_io_bit_ctrl]
	.dbline 263
; 					break;
	xjmp L27
L31:
	.dbline 265
; 				case LED_ON :
; 					 led_hw_on;
	and REG[8],127
	.dbline 266
; 					break;
	xjmp L27
L32:
	.dbline 268
; 				case LED_NEG_PULSE :
; 					 led_hw_off;
	or REG[8],-128
	.dbline 269
; 					 io_bit_ctrl.led_stat = LED_ON;
	mov A,[_io_bit_ctrl]
	and A,-4
	mov [__r0],A
	or [__r0],2
	mov [_io_bit_ctrl],[__r0]
	.dbline 270
; 					break;
L26:
L27:
	.dbline 341
; 				/*case LED_REP_PULSE :
; 					 ggnet_led_j = led_ctrl.cnt;
; 					 led_ctrl.stat = LED_REP_PULSE_1;
; 					break;
; 				case LED_REP_PULSE_1 :
; 					 if ((ggnet_led_j--)>(led_ctrl.cnt/2)) {
; 						led_hw_on;
; 					 } else {
; 						if (ggnet_led_j==0) ggnet_led_j = led_ctrl.cnt;
; 						led_hw_off;
; 					 }
; 					break;*/
; 			 }
; 			
; #if (ADD_LED>0)
; 			 // additional led control job
; 			 switch (add_led.first) {
; 				case LED_OFF :
; 					 led_1st_hw_off;
; 					break;
; 				case LED_POS_PULSE :
; 					 led_1st_hw_on;
; 					 add_led.first = LED_OFF;
; 					break;
; 				case LED_ON :
; 					 led_1st_hw_on;
; 					break;
; 				case LED_NEG_PULSE :
; 					 led_1st_hw_off;
; 					 add_led.first = LED_ON;
; 					break;
; 			 }
; 	#if (ADD_LED>1)
; 			 switch (add_led.second) {
; 				case LED_OFF :
; 					 led_2nd_hw_off;
; 					break;
; 				case LED_POS_PULSE :
; 					 led_2nd_hw_on;
; 					 add_led.second = LED_OFF;
; 					break;
; 				case LED_ON :
; 					 led_2nd_hw_on;
; 					break;
; 				case LED_NEG_PULSE :
; 					 led_2nd_hw_off;
; 					 add_led.second = LED_ON;
; 					break;
; 			 }
; 	#endif
; 	#if (ADD_LED>2)
; 			 switch (add_led.third) {
; 				case LED_OFF :
; 					 led_3rd_hw_off;
; 					break;
; 				case LED_POS_PULSE :
; 					 led_3rd_hw_on;
; 					 add_led.third = LED_OFF;
; 					break;
; 				case LED_ON :
; 					 led_3rd_hw_on;
; 					break;
; 				case LED_NEG_PULSE :
; 					 led_3rd_hw_off;
; 					 add_led.third = LED_ON;
; 					break;
; 			 }
; 	#endif
; 			break;
; #endif
; 	}
L8:
L9:
	.dbline 342
; 	return ep->type;
	mov A,[X-4]
	mov [__r1],A
	mvi A,[__r1]
	and A,7
	.dbline -2
L5:
	add SP,-7
	pop X
	.dbline 0 ; func end
	ret
	.dbsym l k 0 c
	.dbsym l timeout -7 i
	.dbsym l ep -5 pX
	.dbend
	.dbfunc e GrText _GrText fV
;              i -> X+0
;          flags -> X-11
;          count -> X-10
;        str_RAM -> X-9
;        str_ROM -> X-7
;              y -> X-5
;              x -> X-4
_GrText::
	.dbline -1
	push X
	mov X,SP
	add SP,1
	.dbline 360
; }
; //void GrGetNextEventTimeout_timeout_alarm(void)
; //{
; //	io_bit_ctrl.GrGetNextEventTimeout_status = 3;
; //}
; #endif
; 
; #if (GUI_LCD==1)
; /*
;  * Draw a text string in the specified drawable using the
;  * specified graphics context.
;  *
;  * flags
;  *	bit 0 - '0' as high light off, '1' as high light on
;  */
; void
; GrText(GR_COORD x, GR_COORD y, const unsigned char* str_ROM, unsigned char* str_RAM, GR_COUNT count, GR_TEXTFLAGS flags)
; {
	.dbline 362
; 	unsigned char i;
; 	i = count + x;
	mov A,[X-10]
	add A,[X-4]
	mov [X+0],A
	.dbline 364
; 	
; 	if (str_ROM==0) {								// if future has more better solution, throw aways this method
	cmp [X-7],0
	jnz L43
	cmp [X-6],0
	jnz L43
X16:
	.dbline 364
	.dbline 365
; 		for (;x<i;x++,str_RAM++)
	xjmp L39
L36:
	.dbline 366
	mov A,[X-11]
	push A
	mov A,[X-8]
	mov [__r1],A
	mvi A,[__r1]
	push A
	mov A,[X-5]
	push A
	mov A,[X-4]
	push A
	xcall _GrTextDrv
	add SP,-4
L37:
	.dbline 365
	inc [X-4]
	inc [X-8]
	adc [X-9],0
L39:
	.dbline 365
	mov A,[X-4]
	cmp A,[X+0]
	jz X17
	jc L36
X17:
	.dbline 367
	xjmp L35
X15:
	.dbline 367
; 			GrTextDrv(x, y, *str_RAM, flags);
; 	} else {
	.dbline 368
; 		for (;x<i;x++,str_ROM++)
L40:
	.dbline 369
	mov A,[X-11]
	push A
	mov A,[X-6]
	mov [__r1],A
	mov A,[X-7]
	push X
	mov X,[__r1]
	romx
	pop X
	push A
	mov A,[X-5]
	push A
	mov A,[X-4]
	push A
	xcall _GrTextDrv
	add SP,-4
L41:
	.dbline 368
	inc [X-4]
	inc [X-6]
	adc [X-7],0
L43:
	.dbline 368
	mov A,[X-4]
	cmp A,[X+0]
	jz X18
	jc L40
X18:
	.dbline 370
L35:
	.dbline -2
	.dbline 371
; 			GrTextDrv(x, y, *str_ROM, flags);
; 	}
; }
L33:
	add SP,-1
	pop X
	.dbline 0 ; func end
	ret
	.dbsym l i 0 c
	.dbsym l flags -11 c
	.dbsym l count -10 c
	.dbsym l str_RAM -9 pc
	.dbsym l str_ROM -7 pc
	.dbsym l y -5 c
	.dbsym l x -4 c
	.dbend
	.area bss(ram, con, rel)
	.dbfile ./srvfunc.c
_io_bit_ctrl::
	.blkb 1
	.dbsym e io_bit_ctrl _io_bit_ctrl X
_ptr_events_vect::
	.blkb 1
	.dbsym e ptr_events_vect _ptr_events_vect X
_events_vect::
	.blkb 2
	.dbsym e events_vect _events_vect A[2:2]X
