	.module srvmain.c
	.area text(rom, con, rel)
	.dbfile ./srvmain.c
	.dbfunc e mwlike8_server _mwlike8_server fV
;              i -> X+0
_mwlike8_server::
	.dbline -1
	push X
	mov X,SP
	add SP,1
	.dbline 22
; /*
;  * Copyright (c) 1999, 2000, 2001, 2003 Greg Haerr <greg@censoft.com>
;  * Portions Copyright (c) 2002 by Koninklijke Philips Electronics N.V.
;  * Copyright (c) 1991 David I. Bell
;  * Copyright (c) 2004 cheng chung yan <chungyan5@hkem.com / yan@amonics.com> for 8-bit system running in linlike8
;  * Permission is granted to use, distribute, or modify this source,
;  * provided that this copyright notice remains intact.
;  *
;  * Main module of graphics server.
;  */
; 
; #include "app.h"									// linlike8 os
; 
; #include "sched.h"
; #include "nano-X.h"									// NR_EVENT
; #include "device.h"									// scr_open()
; #include "timer.h"
; 
; extern void io_open(void);
; 
; void mwlike8_server(void)
; {
	.dbline 32
; 	
; #if (KB_MOD==1)
; 	// keyboard events
; 	//=========================================================
; 	unsigned char i;
; #endif
; 
; 		// init this process
; #if (GUI_LCD==1)
; 	scr_open();									// hardware init.
	xcall _scr_open
	.dbline 34
; #endif
; 	io_open();
	xcall _io_open
	xjmp L6
L5:
	.dbline 45
	.dbline 55
	xcall _schedule
	.dbline 56
L6:
	.dbline 45
	xjmp L5
X0:
	.dbline -2
	.dbline 57
; 	
; #if (AUTO_ZERO==0)
; 	unsigned char k;								// mwlike8 server init.
; 	wr_ptr_events_vect = 0;
; 	rd_ptr_events_vect = 0;
; 	for (k=0;i<NR_EVENT;k++) events_vect[i].type = GR_EVENT_TYPE_NONE;
; #endif
; 
; 		// main job of this process
; 		//	detect user input
; 	while (1) {
; 		
; 		// linlike8 system
; 		//******************************************
; 		
; 		// appl.
; 		//******************************************
; 		
; 		// system context switch
; 		//******************************************
; 		sched_yield();								// any time should switch away to let other process to run, unless somethign need to run in this process0
; 	}
; }
L4:
	add SP,-1
	pop X
	.dbline 0 ; func end
	ret
	.dbsym l i 0 c
	.dbend
