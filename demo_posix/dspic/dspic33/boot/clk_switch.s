/*********************************************************************/
.ifdef __33FJ128GP306
.include "p33FJ128GP306.inc"
.endif
/**********************************************************************/
.ifdef __33FJ256GP506
.include "p33FJ256GP506.inc"
.endif

	.global ___builtin_write_OSCCONH
	.global ___builtin_write_OSCCONL

	.section .text

/******************************************************************************
  change OSCCON setting (High Byte)
*******************************************************************************/
___builtin_write_OSCCONH:
	;OSCCONH (high byte) Unlock Sequence
		MOV #OSCCONH, w1
		MOV #0x78, w2
		MOV #0x9A, w3
		MOV.B w2, [w1] ; Write 0x78
		MOV.B w3, [w1] ; Write 0x9A
	;Set New Oscillator Selection
		MOV.B w0, [w1]
	    return

/******************************************************************************
  change OSCCON setting (Low Byte)
*******************************************************************************/
___builtin_write_OSCCONL:
	;OSCCONL (low byte) Unlock Sequence
		MOV #OSCCONL, w1
		MOV #0x46, w2
		MOV #0x57, w3
		MOV.B w2, [w1] ; Write 0x46
		MOV.B w3, [w1] ; Write 0x9A
	; Enable Clock Switch
		MOV.B w0, [w1] ; Request Clock Switching by Setting OSWEN bit
	    return

.end
