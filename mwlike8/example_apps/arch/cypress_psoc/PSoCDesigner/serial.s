	.module serial.c
	.area text(rom, con, rel)
	.dbfile ./serial.c
	.dbfunc e serial_open _serial_open fV
_serial_open::
	.dbline -1
	.dbline 38
; /*
;  * Copyright (c) 24-05-2004 cheng chung yan yan@amonics.com> / <chungyan5@hkem.com for 8-bit system
;  *
;  * this module is serial in cypress psoc, driver specification. So each arch. has different code here
;  * 
;  * using cypress psoc serial
;  *
;  */
; 
; #include "app.h"									// this linlike8
; #if (SERIAL_MOD==1)
; #if (SWITCH_CASE==0)
; 	#include "sched.h"								// linlike8 system
; 	#include "timer.h"
; #endif
; #include "cirbuf.h"									// pre_wr_cir254buf()
; #include "uart_1.h"
; 
; unsigned char serial_rx_buf[MAX_SERIAL_BUF];
; struct {
; 	unsigned	wr :4;
; 	unsigned	rd :4;
; } serial_rx;
; unsigned char serial_tx_buf[MAX_SERIAL_TX_BUF];
; struct {
; 	unsigned	wr :3;								// max. of tx buf. is 7
; 	unsigned	rd :4;
; 	unsigned	tx_complete_flag :1;						// true as completed, false is non-completed
; } serial_tx;
; #if (SWITCH_CASE==1)
; struct wr_compl_struct {
; 	unsigned tmp	:7;
; 	unsigned state	:1;
; } wr_compl_var;
; #endif
; 
; void serial_open(void)
; {
	.dbline 39
; 	serial_rx.wr = 0;
	and [_serial_rx],0
	.dbline 40
; 	serial_rx.rd = 0;
	.dbline 41
; 	serial_tx.wr = 0;
	and [_serial_tx],-128
	.dbline 42
; 	serial_tx.rd = 0;
	.dbline 43
; 	serial_tx.tx_complete_flag = 1;
	or [_serial_tx],-128
	.dbline 46
; 
; #if (SWITCH_CASE==1)
; 	wr_compl_var.state = 0;
	and [_wr_compl_var],127
	.dbline 48
; #endif
;         UART_1_EnableInt(); 
	push X
	xcall _UART_1_EnableInt
	.dbline 49
;         UART_1_Start(UART_PARITY_NONE);
	mov A,0
	xcall _UART_1_Start
	pop X
	.dbline -2
	.dbline 50
; }
L3:
	.dbline 0 ; func end
	ret
	.dbend
	.dbfunc e serial_tx_isr _serial_tx_isr fV
;              k -> X+0
_serial_tx_isr::
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
	push X
	mov X,SP
	add SP,1
	.dbline 55
; #endif
; 
; #pragma	interrupt_handler	serial_tx_isr						// shouald add this isr name at boot.asm from PSoC Designer
; void serial_tx_isr(void)
; {
	.dbline 58
; #if (SERIAL_MOD==1)
; 	unsigned char k;
; 	if (bUART_1_ReadTxStatus() & UART_TX_BUFFER_EMPTY) {
	push X
	xcall _bUART_1_ReadTxStatus
	pop X
	mov [__r0],A
	tst [__r0],16
	jz L5
	.dbline 58
	.dbline 59
; 		if ((k=pre_rd_cir254buf((unsigned char)serial_tx.wr,(unsigned char)serial_tx.rd,MAX_SERIAL_TX_BUF))!=255) {
	mov A,3
	push A
	mov A,[_serial_tx]
	asr A
	asr A
	asr A
	and A,15
	push A
	mov A,[_serial_tx]
	and A,7
	push A
	xcall _pre_rd_cir254buf
	add SP,-3
	mov [X+0],A
	cmp A,-1
	jz L7
	.dbline 59
	.dbline 60
; 			UART_1_SendData(serial_tx_buf[(unsigned char)serial_tx.rd]);
	mov [__r1],[_serial_tx]
	mov [__r0],0
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	and [__r1],15
	and [__r0],0
	add [__r1],<_serial_tx_buf
	adc [__r0],>_serial_tx_buf
	mvi A,[__r1]
	push X
	xcall _UART_1_SendData
	pop X
	.dbline 61
; 			serial_tx.rd = (unsigned)k;
	mov A,[_serial_tx]
	and A,-121
	mov [__r0],A
	mov A,[X+0]
	and A,15
	asl A
	asl A
	asl A
	and A,120
	or [__r0],A
	mov [_serial_tx],[__r0]
	.dbline 62
	xjmp L8
L7:
	.dbline 62
; 		} else serial_tx.tx_complete_flag = 1;					// change to empty of tx
	or [_serial_tx],-128
	mov A,[_serial_tx]
L8:
	.dbline 63
L5:
	.dbline -2
	.dbline 65
; 	}
; #endif
; }
L4:
	add SP,-1
	pop X
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
	.dbsym l k 0 c
	.dbend
	.dbfunc e serial_rx_isr _serial_rx_isr fV
;              i -> X+1
;      bRxStatus -> X+0
_serial_rx_isr::
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
	push X
	mov X,SP
	add SP,2
	.dbline 69
; 
; #pragma	interrupt_handler	serial_rx_isr						// shouald add this isr name at boot.asm from PSoC Designer
; void serial_rx_isr(void)
; {
	.dbline 73
; #if (SERIAL_MOD==1)
; 	unsigned char i;
; 	unsigned char bRxStatus;
; 	if ( bRxStatus=bUART_1_ReadRxStatus() & UART_RX_COMPLETE ) {
	push X
	xcall _bUART_1_ReadRxStatus
	pop X
	and A,8
	mov [X+0],A
	cmp A,0
	jz L10
	.dbline 73
	.dbline 74
; 		if ( ! (bRxStatus & UART_RX_NO_ERROR) ) {
	tst [X+0],-32
	jnz L12
	.dbline 74
	.dbline 75
; 			if ((i=pre_wr_cir254buf(serial_rx.wr,serial_rx.rd,MAX_SERIAL_BUF))!=255) {
	mov A,9
	push A
	mov A,[_serial_rx]
	asr A
	asr A
	asr A
	asr A
	and A,15
	push A
	mov A,[_serial_rx]
	and A,15
	push A
	xcall _pre_wr_cir254buf
	add SP,-3
	mov [X+1],A
	cmp A,-1
	jz L14
	.dbline 75
	.dbline 76
; 				serial_rx_buf[serial_rx.wr] = bUART_1_ReadRxData();
	push X
	xcall _bUART_1_ReadRxData
	pop X
	mov [__r3],[_serial_rx]
	mov [__r2],0
	and [__r3],15
	and [__r2],0
	add [__r3],<_serial_rx_buf
	adc [__r2],>_serial_rx_buf
	mvi [__r3],A
	.dbline 77
; 				serial_rx.wr = i;
	mov A,[_serial_rx]
	and A,-16
	mov [__r0],A
	mov A,[X+1]
	and A,15
	or [__r0],A
	mov [_serial_rx],[__r0]
	.dbline 78
	xjmp L15
L14:
	.dbline 78
; 			} else bUART_1_ReadRxData();					// any cases, i must need to clear the UART rx register data, then prepare for next interrupt
	push X
	xcall _bUART_1_ReadRxData
	pop X
L15:
	.dbline 79
; 		}
L12:
	.dbline 80
L10:
	.dbline -2
	.dbline 82
; 	}
; #endif
; }
L9:
	add SP,-2
	pop X
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
	.dbsym l i 1 c
	.dbsym l bRxStatus 0 c
	.dbend
	.dbfunc e write _write fc
;              i -> X+1
;            var -> X+0
;            __n -> X-6
;          __buf -> X-5
_write::
	.dbline -1
	push X
	mov X,SP
	add SP,2
	.dbline 86
; 
; #if (SERIAL_MOD==1)
; unsigned char write(unsigned char *__buf, unsigned char __n)
; {
	.dbline 94
; 	unsigned char i;
; //	struct {
; //		unsigned j : 7;
; 		//unsigned k : 1;
; //	} var;
; 	unsigned char var;
; 	
; 	if (serial_tx.tx_complete_flag) {						// empty of tx
	mov A,[_serial_tx]
	asr A
	asr A
	asr A
	asr A
	asr A
	asr A
	asr A
	and A,1
	mov [__r0],A
	and [__r0],1
	jz L17
	.dbline 94
	.dbline 96
; 		//var.k = 1;
; 		var = 1;
	mov [X+0],1
	.dbline 97
	xjmp L22
L17:
	.dbline 97
; 	} else {
	.dbline 99
; 		//var.k = 0;
; 		var = 0;
	mov [X+0],0
	.dbline 100
; 	}
	.dbline 101
	xjmp L22
L19:
	.dbline 101
; 	for (;var<__n;var++) {
	.dbline 102
; 		if ((i=pre_wr_cir254buf((unsigned char)serial_tx.wr,(unsigned char)serial_tx.rd,MAX_SERIAL_TX_BUF))!=255) {
	mov A,3
	push A
	mov A,[_serial_tx]
	asr A
	asr A
	asr A
	and A,15
	push A
	mov A,[_serial_tx]
	and A,7
	push A
	xcall _pre_wr_cir254buf
	add SP,-3
	mov [X+1],A
	cmp A,-1
	jz L21
	.dbline 102
	.dbline 103
; 			serial_tx_buf[serial_tx.wr] = *(__buf+var);
	mov [__r1],[_serial_tx]
	mov [__r0],0
	and [__r1],7
	and [__r0],0
	add [__r1],<_serial_tx_buf
	adc [__r0],>_serial_tx_buf
	mov A,[X+0]
	mov [__r3],A
	mov [__r2],0
	mov A,[X-4]
	add [__r3],A
	mov A,[X-5]
	adc [__r2],A
	mvi A,[__r3]
	mvi [__r1],A
	.dbline 104
; 			serial_tx.wr = i;
	mov A,[_serial_tx]
	and A,-8
	mov [__r0],A
	mov A,[X+1]
	and A,7
	or [__r0],A
	mov [_serial_tx],[__r0]
	.dbline 105
; 		} else break;
	.dbline 105
L24:
	.dbline 106
L20:
	.dbline 101
	inc [X+0]
L22:
	.dbline 101
	mov A,[X+0]
	cmp A,[X-6]
	jz X0
	jc L19
X0:
L21:
	.dbline 108
; 	}
; //	if (var.k==1) UART_1_SendData(*(__buf));
; 	if (serial_tx.tx_complete_flag) {
	mov A,[_serial_tx]
	asr A
	asr A
	asr A
	asr A
	asr A
	asr A
	asr A
	and A,1
	mov [__r0],A
	and [__r0],1
	jz L25
	.dbline 108
	.dbline 109
; 		UART_1_SendData(*(__buf));	// send 1st byte to active the Tx
	mov A,[X-4]
	mov [__r1],A
	mvi A,[__r1]
	push X
	xcall _UART_1_SendData
	pop X
	.dbline 110
; 		serial_tx.tx_complete_flag = 0;						// change to non-empty of tx
	and [_serial_tx],127
	.dbline 111
; 	}
L25:
	.dbline 112
; 	return (unsigned char) var;
	mov A,[X+0]
	.dbline -2
L16:
	add SP,-2
	pop X
	.dbline 0 ; func end
	ret
	.dbsym l i 1 c
	.dbsym l var 0 c
	.dbsym l __n -6 c
	.dbsym l __buf -5 pc
	.dbend
	.dbfunc e read _read fc
;              k -> X+0
;          __buf -> X-5
_read::
	.dbline -1
	push X
	mov X,SP
	add SP,1
	.dbline 117
; }
; 
; //<rec 1 byte,remove>unsigned char read(unsigned char *__buf, unsigned char __n)	// reduce RAM usage
; unsigned char read(unsigned char *__buf)//<rec 1 byte,add>
; {
	.dbline 121
; 	unsigned char k;
; 	//<rec 1 byte,remove>unsigned char j;
; 	//<rec 1 byte,remove>for (j=0;j<__n;j++) {
; 		if ((k=pre_rd_cir254buf(serial_rx.wr,serial_rx.rd,MAX_SERIAL_BUF))!=255) {
	mov A,9
	push A
	mov A,[_serial_rx]
	asr A
	asr A
	asr A
	asr A
	and A,15
	push A
	mov A,[_serial_rx]
	and A,15
	push A
	xcall _pre_rd_cir254buf
	add SP,-3
	mov [X+0],A
	cmp A,-1
	jz L28
	.dbline 121
	.dbline 123
; 			//<rec 1 byte,remove>*(__buf+j) = serial_rx_buf[serial_rx.rd];
; 			*(__buf) = serial_rx_buf[serial_rx.rd];//<rec 1 byte,add>
	mov A,[X-4]
	mov [__r1],A
	mov [__r3],[_serial_rx]
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
	add [__r3],<_serial_rx_buf
	adc [__r2],>_serial_rx_buf
	mvi A,[__r3]
	mvi [__r1],A
	.dbline 124
; 			serial_rx.rd = k;
	mov A,[_serial_rx]
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
	mov [_serial_rx],[__r0]
	.dbline 125
; 			return 1;//<rec 1 byte,add>
	mov A,1
	xjmp L27
L28:
	.dbline 127
; 		}//<rec 1 byte,add>
; 		return 0;//<rec 1 byte,add>
	mov A,0
	.dbline -2
L27:
	add SP,-1
	pop X
	.dbline 0 ; func end
	ret
	.dbsym l k 0 c
	.dbsym l __buf -5 pc
	.dbend
	.dbfunc e serial_write_complete _serial_write_complete fc
;            __n -> X-6
;          __buf -> X-5
_serial_write_complete::
	.dbline -1
	push X
	mov X,SP
	add SP,4
	.dbline 146
; 		//<rec 1 byte,remove>} else break;
; 	//<rec 1 byte,remove>}
; 	//<rec 1 byte,remove>return j;
; }
; 
; #if (SWITCH_CASE==0)
; void serial_write_complete(unsigned char *__buf, unsigned char __n)
; {
; 	// non-tested
; 	unsigned char i = 0;
; 	do {
; 		i += write(__buf + i, __n - i);
; 		if (i==__n) break;
; 		else msleep(10);
; 	} while (1);
; }
; #else
; unsigned char serial_write_complete(unsigned char *__buf, unsigned char __n)
; {
	.dbline 147
	mov [__r1],[_wr_compl_var]
	mov [__r0],0
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	and [__r1],1
	and [__r0],0
	mov A,[__r1]
	mov [X+1],A
	mov A,[__r0]
	mov [X+0],A
	cmp [X+0],0
	jnz X2
	cmp [X+1],0
	jz L34
X2:
	cmp [X+0],0
	jnz X3
	cmp [X+1],1
	jz L35
X3:
	xjmp L31
X1:
	.dbline 147
; 	switch (wr_compl_var.state) {
L34:
	.dbline 149
; 		case 0 :
; 			 wr_compl_var.tmp = 0;
	and [_wr_compl_var],-128
	.dbline 150
; 			 wr_compl_var.state = 1;
	or [_wr_compl_var],-128
	mov A,[_wr_compl_var]
	.dbline 151
; 			break;
	xjmp L32
L35:
	.dbline 153
; 		case 1 :
; 			 wr_compl_var.tmp += write(__buf + wr_compl_var.tmp, __n - wr_compl_var.tmp);
	mov [__r1],[_wr_compl_var]
	mov [__r0],0
	and [__r1],127
	and [__r0],0
	mov A,[__r1]
	mov [X+3],A
	mov A,[__r0]
	mov [X+2],A
	mov A,[X-6]
	mov [__r3],A
	mov [__r2],0
	mov A,[__r1]
	sub [__r3],A
	mov A,[__r0]
	sbb [__r2],A
	mov A,[__r3]
	push A
	mov A,[__r1]
	add A,[X-4]
	mov [__r3],A
	mov A,[__r0]
	adc A,[X-5]
	push A
	mov A,[__r3]
	push A
	xcall _write
	add SP,-3
	mov [__r0],A
	mov A,[_wr_compl_var]
	and A,-128
	mov [__r2],A
	mov [__r1],[__r0]
	mov [__r0],0
	mov A,[X+3]
	add A,[__r1]
	mov [__r1],A
	mov A,[X+2]
	adc A,[__r0]
	mov [__r0],[__r1]
	and [__r0],127
	mov A,[__r2]
	or A,[__r0]
	mov [_wr_compl_var],A
	.dbline 154
; 			 if (wr_compl_var.tmp>=__n) wr_compl_var.state = 0;
	mov A,[_wr_compl_var]
	and A,127
	cmp A,[X-6]
	jz X4
	jc L32
X4:
	.dbline 154
	and [_wr_compl_var],127
	.dbline 155
; 			break;
L31:
L32:
	.dbline 157
; 	}
; 	return (unsigned char) wr_compl_var.state;
	mov A,[_wr_compl_var]
	asr A
	asr A
	asr A
	asr A
	asr A
	asr A
	asr A
	and A,1
	.dbline -2
L30:
	add SP,-4
	pop X
	.dbline 0 ; func end
	ret
	.dbsym l __n -6 c
	.dbsym l __buf -5 pc
	.dbend
	.area bss(ram, con, rel)
	.dbfile ./serial.c
_wr_compl_var::
	.blkb 1
	.dbsym e wr_compl_var _wr_compl_var X
_serial_tx::
	.blkb 1
	.dbsym e serial_tx _serial_tx X
_serial_tx_buf::
	.blkb 3
	.dbsym e serial_tx_buf _serial_tx_buf A[3:3]c
_serial_rx::
	.blkb 1
	.dbsym e serial_rx _serial_rx X
_serial_rx_buf::
	.blkb 9
	.dbsym e serial_rx_buf _serial_rx_buf A[9:9]c
