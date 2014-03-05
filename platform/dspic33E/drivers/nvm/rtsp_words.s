/**********************************************************************
* � 2005 Microchip Technology Inc.
*
* FileName:        rtsApi.s
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC33Exxxx/PIC24Exxxx
* Compiler:        MPLAB� C30 v3.30 or higher
*
*
* SOFTWARE LICENSE AGREEMENT:
* Microchip Technology Incorporated ("Microchip") retains all ownership and 
* intellectual property rights in the code accompanying this message and in all 
* derivatives hereto.  You may use this code, and any derivatives created by 
* any person or entity by or on your behalf, exclusively with Microchip's
* proprietary products.  Your acceptance and/or use of this code constitutes 
* agreement to the terms and conditions of this notice.
*
* CODE ACCOMPANYING THIS MESSAGE IS SUPPLIED BY MICROCHIP "AS IS".  NO 
* WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED 
* TO, IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A 
* PARTICULAR PURPOSE APPLY TO THIS CODE, ITS INTERACTION WITH MICROCHIP'S 
* PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
*
* YOU ACKNOWLEDGE AND AGREE THAT, IN NO EVENT, SHALL MICROCHIP BE LIABLE, WHETHER 
* IN CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), 
* STRICT LIABILITY, INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, 
* PUNITIVE, EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF 
* ANY KIND WHATSOEVER RELATED TO THE CODE, HOWSOEVER CAUSED, EVEN IF MICROCHIP HAS BEEN 
* ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
* ALLOWABLE BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO 
* THIS CODE, SHALL NOT EXCEED THE PRICE YOU PAID DIRECTLY TO MICROCHIP SPECIFICALLY TO 
* HAVE THIS CODE DEVELOPED.
*
* You agree that you are solely responsible for testing the code and 
* determining its suitability.  Microchip has no obligation to modify, test, 
* certify, or support the code.
*
* REVISION HISTORY:
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author            Date      Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Settu D 			03/15/06  First release of source file
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Sorin M 			07/01/10  Updated for dsPIC33E
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Srikar D			04/27/11  Updated for dsPIC33E / PIC24E
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* ADDITIONAL NOTES:
*
*
**********************************************************************/

.ifdef __dsPIC33E
.include "p33Exxxx.inc"
.endif
.ifdef __PIC24E
.include "p24Exxxx.inc"
.endif

.equ    FLASH_PAGE_ERASE_CODE, 	0x4003
.equ    FLASH_ROW_PROG_CODE, 	0x4002
.equ    FLASH_WORD_PROG_CODE,	0x4001

.global _flashPageRead
.global _flashPageErase
.global _flashPageWrite
.global _flashPageModify
.global _flashWordModify
.global _flashWordRead

.section .text

/******************************************************************************
  Flash Page Read 
  Read EIGHT rows (PAGE) of memory, upper PM byte is discarded
*******************************************************************************/
_flashPageRead:
	push	TBLPAG
	mov		w0, TBLPAG
	mov		#1024, w3

readNext:     
	tblrdl	[w1],[w2++]
	tblrdh	[w1++], w6	; Discard PM upper byte
	dec			w3, w3
	bra			nz, readNext

	clr		w0
	pop		TBLPAG
	return



/******************************************************************************
  Flash Page Modify 
  Modify EIGHT rows (PAGE) of memory, upper PM byte is discarded
*******************************************************************************/
_flashPageModify:
	sl	w0,#7,w0
	add	w0,w3,w3
				
modifyNext:     
	mov	[w2++], [w3++]	; Discard PM upper byte
	dec	w1, w1
	bra	nz, modifyNext

	return



/******************************************************************************
  Flash Page Erase 
  Erase EIGHT rows (PAGE) of memory
*******************************************************************************/
_flashPageErase:      
	push	TBLPAG

	mov			w0, TBLPAG		; Init Pointer to page to be erased
	mov			w0, NVMADRU		; Init Pointer to row to be erased
	mov			w1, NVMADR
	tblwtl	w1, [w1]

	mov		#FLASH_PAGE_ERASE_CODE, w7
	mov		w7, NVMCON
	bset 	w7, #WR
	disi	#5					; Block all interrupt with priority <7 for next 5 instructions
	mov		#0x55, W0
	mov		W0, NVMKEY
	mov		#0xAA, W0
	mov		W0, NVMKEY
	mov		w7, NVMCON	; Start Program Operation
	nop
	nop

erase_wait:     
	btsc	NVMCON, #WR
	bra		erase_wait

	clr		w0
	pop		TBLPAG
	return



/******************************************************************************
  Flash Page Program
  Program EIGHT rows (PAGE) of memory, 
  Each row contains 192 bytes of data (64 instructions, with upper PM byte == NOP) 
*******************************************************************************/
_flashPageWrite:
	push	TBLPAG
	MOV		W0, W9				; Add loc to be written
	MOV		w1, w8
	mov		#0xFA, w0
	mov		w0, TBLPAG		;TBLPAG has to point to the SRAM latch section
	mov		#0, w1

	mov		#0,w6
	mov		#8,w5					; w5=row counter
row_loop:
	mov		#64,w3
	mov		#0, w1				;clear w1 - address is relative to the row start address
pinst_loop: 
	TBLWTL.b	[w2++], [w1]
	TBLWTH.b	w6, [w1++]	; write 0 to high byte to be decoded as nop
	TBLWTL.b	[w2++], [w1++]

	TBLWTL.b	[w2++], [w1]
	TBLWTH.b	w6, [w1++]	; write 0 to high byte to be decoded as nop
	TBLWTL.b	[w2++], [w1++]

	dec2  w1,w1
	dec2  w1,w1
		
	MOV		w9, NVMADRU			;point to the row to be written to
	MOV		w8, NVMADR

	; Setup NVCON for row program
	mov		#FLASH_WORD_PROG_CODE,w7
	mov		w7, NVMCON
	bset 	w7,#WR
	disi	#5					; Block all interrupt with priority <7 for next 5 instructions
	mov		#0x55, W0
	mov		W0, NVMKEY
	mov		#0xAA, W0
	mov		W0, NVMKEY
	mov		w7, NVMCON	; Start Program Operation
	nop
	nop
	add		w8,#4,w8
prog_wait:     
	btsc	NVMCON, #WR
	bra		prog_wait
	dec		w3, w3
	bra		NZ, pinst_loop

	dec		w5, w5
	bra		nz, row_loop	; test if all rows are written

	clr		w0
	pop		TBLPAG
	return



/******************************************************************************
  Flash Word Program
  Modify double word of memory
*******************************************************************************/
_flashWordModify:
	push	TBLPAG
	sl		w0,#7,w0
	add		w0,w3,w3
	mov 	#0xFA, w0
	mov 	w0, TBLPAG
	add		w1,w3,w3
	MOV 	W2, W9; Add loc to be written
	MOV 	w3, w8

	mov #0, w3
	mov #0, w6

	; Copy the bytes to the latches.
	TBLWTL.b	[w4++], [w3]
	TBLWTH.b	w6, [w3++]		; write 0 to high byte to be decoded as nop
	TBLWTL.b	[w4++], [w3++]

	TBLWTL.b	[w4++], [w3]
	TBLWTH.b	w6, [w3++]		; write 0 to high byte to be decoded as nop
	TBLWTL.b	[w4++], [w3++]

	MOV w9, NVMADRU
	MOV w8, NVMADR

	; Setup NVCON for word program
	mov	#FLASH_WORD_PROG_CODE,w7
	MOV	W7, NVMCON
	disi #5

	MOV #0x55, W0
	MOV W0, NVMKEY
	MOV #0xAA, W0
	MOV W0, NVMKEY

	bset NVMCON, #15
	nop
	nop

	wait:
	btsc NVMCON, #15
	bra wait

	clr W0
	pop TBLPAG
	return



/******************************************************************************
  Flash Word Read
  Read word of memory
*******************************************************************************/
_flashWordRead:
	push 	TBLPAG
	sl 		w0,#7,w0
	add		w0,w3,w3
	add		w1,w3,w3
	mov		w2, TBLPAG
	mov 	#2, w5

readloop:
	tblrdl.b  [w3], [w4++]
	tblrdh.b  [w3++], [w4++]
	tblrdl.b  [w3++], [w4]
	dec 	w5, w5
	bra 	nz, readloop

	clr 	w0
	pop 	TBLPAG
	return

.end
