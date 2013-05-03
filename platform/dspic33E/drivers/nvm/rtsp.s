/**********************************************************************
* � 2011 Microchip Technology Inc.
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
* Sinha P 			02/02/11  First release of source file
* Srikar D			04/27/11  Updated for dsPIC33E / PIC24E
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* ADDITIONAL NOTES:
*
*
**********************************************************************/
.equ __33EP512MU810, 1
.include "p33Exxxx.inc"


.equ    FLASH_PAGE_ERASE_CODE,	0x4003
.equ    FLASH_ROW_PROG_CODE,		0x4002

.equ  	INSTRUCTION_PER_ROW,		128
.equ  	ROW_PER_PAGE,						8
.equ  	INSTRUCTION_PER_PAGE,		(INSTRUCTION_PER_ROW * ROW_PER_PAGE)

.global _flashPageRead
.global _flashPageErase
.global _flashPageWrite
.global _flashPageModify

.section .text


/******************************************************************************
  Flash Page Read 
  Read EIGHT rows (PAGE) of memory, upper PM byte is discarded
*******************************************************************************/
_flashPageRead:
	push	w6
	push	TBLPAG
	mov		w0, TBLPAG
	mov 	TBLPAG, w0
	mov   #INSTRUCTION_PER_PAGE, w3

readNext:     
	tblrdl	[w1],[w2++]
	tblrdh	[w1++],w6		; Discard PM upper byte
	dec			w3,w3
	bra			nz,readNext

	clr		w0
	pop		TBLPAG
	pop		w6
	return


/******************************************************************************
  Flash Page Modify 
  Read EIGHT rows (PAGE) of memory, upper PM byte is discarded
*******************************************************************************/
_flashPageModify:
	sl 		w0,#8,w0
	add		w0,w3,w3

modifyNext:     
	mov		[w2++],[w3++]	; Discard PM upper byte
	dec		w1, w1
	bra		nz, modifyNext

	return


/******************************************************************************
  Flash Page Erase 
  Erase EIGHT rows (PAGE) of memory
*******************************************************************************/
_flashPageErase:
	push	w7
	mov		w0,NVMADRU	; Init Pointer to page to be erased
	mov		w1,NVMADR

	mov		#FLASH_PAGE_ERASE_CODE,w7
	mov		w7,NVMCON
	bset	w7,#WR

	disi	#06					; Block all interrupt with priority <7 for next 5 instructions
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
	pop		w7
	return


/******************************************************************************
  Flash Page Program
  Program EIGHT rows (PAGE) of memory, 
  Each row contains 384 bytes of data (128 instructions, with upper PM byte == NOP) 
*******************************************************************************/
_flashPageWrite:
	push	w5
	push	w6
	push	w7
	push	w8
	push	w9

	push	TBLPAG
	MOV 	W0, W9					; Add loc to be written
	MOV 	w1, w8
	mov 	#0xFA, w0
	mov 	w0, TBLPAG			; TBLPAG has to point to the SRAM latch section
	mov 	#0, w1
		
	mov		#0,w6
	mov		#ROW_PER_PAGE,w5	; w5=row counter
row_loop:
	mov 	#INSTRUCTION_PER_ROW,w3
	mov 	#0, w1					; clear w1 - address is relative to the row start address
pinst_loop: 
	TBLWTL.b	[w2++], [w1]
	TBLWTH.b	w6, [w1++]	; write 0 to high byte to be decoded as nop
	TBLWTL.b	[w2++], [w1++]
	dec		w3, w3
	bra		NZ, pinst_loop
		
	MOV		w9, NVMADRU			; point to the row to be written to
	MOV		w8, NVMADR
				
	; Setup NVCON for row program
	mov		#FLASH_ROW_PROG_CODE,w7
	mov		w7,NVMCON
	bset	w7,#WR

	disi 	#06							; Block all interrupt with priority <7 for next 5 instructions
	mov		#0x55, W0
	mov		W0, NVMKEY
	mov		#0xAA, W0
	mov		W0, NVMKEY
	mov		w7, NVMCON			; Start Program Operation
	nop
	nop
prog_wait:     
	btsc	NVMCON, #WR
	bra		prog_wait

	mov		#256,w1		; load with row size
	mov		w8,w0			; load previous row address
	add		w1,w0,w0	; compute new row address
	mov		w0,w8			; load new row address in w8

	dec		w5, w5
	bra		nz, row_loop	; test if all rows are written

	clr		w0
	pop		TBLPAG

	pop		w9
	pop		w8
	pop		w7
	pop		w6
	pop		w5
	return

.end
