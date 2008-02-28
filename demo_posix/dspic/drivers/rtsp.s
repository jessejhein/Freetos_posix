/**********************************************************************
* ?2005 Microchip Technology Inc.
*
* FileName:        rtsApi.s
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC33Fxxxx/PIC24Hxxxx
* Compiler:        MPLAB?C30 v3.00 or higher
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
*
* ADDITIONAL NOTES:
*
*
**********************************************************************/

/*********************************************************************
 * The following .ifdef are added so that the code can work with both
 * dspic33fj128gp306 and dspic30f5011								 */
.ifdef __33FJ128GP306

.include "/usr/pic30-elf/support/inc/p33FJ128GP306.inc"

.equ    FLASH_PAGE_ERASE_CODE, 	0x4042
.equ    FLASH_ROW_PROG_CODE, 	0x4001

.equ	INSTRUCTION_PER_ROW,	64
.equ	ROW_PER_PAGE,			8
.equ	INSTRUCTION_PER_PAGE,	(INSTRUCTION_PER_ROW*ROW_PER_PAGE)
.endif
/**********************************************************************/
.ifdef __33FJ256GP506

.include "/usr/pic30-elf/support/inc/p33FJ256GP506.inc"

.equ    FLASH_PAGE_ERASE_CODE, 	0x4042
.equ    FLASH_ROW_PROG_CODE, 	0x4001

.equ	INSTRUCTION_PER_ROW,	64
.equ	ROW_PER_PAGE,			8
.equ	INSTRUCTION_PER_PAGE,	(INSTRUCTION_PER_ROW*ROW_PER_PAGE)
.endif
/**********************************************************************/
.ifdef __30F5011

.include "/usr/pic30-elf/support/inc/p30f5011.inc"
.equ    FLASH_ROW_ERASE_CODE, 	0x4041
.equ    FLASH_ROW_PROG_CODE, 	0x4001

.equ	INSTRUCTION_PER_ROW,	32
.equ	ROW_PER_PAGE,			1
.equ	INSTRUCTION_PER_PAGE,	(INSTRUCTION_PER_ROW*ROW_PER_PAGE)
.endif
/**********************************************************************/

	.global _flashPageRead
	.global _flashPageErase
	.global _flashPageWrite
	.global _flashPageModify

	.section .text

/******************************************************************************
  Flash Page Read 
  dspic33: Read EIGHT rows (PAGE) of memory, upper PM byte is discarded
  dspic30: Read ONE row (PAGE) of memory, upper PM byte is discarded  
*******************************************************************************/
_flashPageRead:
        push    TBLPAG
        mov     w0, TBLPAG
        mov     #INSTRUCTION_PER_PAGE, w3
readNext:     
		tblrdl  [w1],[w2++]
		tblrdh	[w1++],w6		; Discard PM upper byte
        dec     w3, w3
        bra     nz, readNext

		clr 	w0
 		pop     TBLPAG
        return

/******************************************************************************
  Flash Page Erase 
  dspic33: Erase EIGHT rows (PAGE) of memory
  dspic30: Erase ONE row (PAGE) of memory
*******************************************************************************/
_flashPageErase:
        push    TBLPAG

        mov     w0, TBLPAG		; Init Pointer to row to be erased
		tblwtl  w1,[w1]			; Dummy write to select the row

								; Setup NVCON for page erase
		mov   	#FLASH_PAGE_ERASE_CODE,w7
        mov     w7, NVMCON
		bset 	w7,#WR
		disi 	#5				; Block all interrupt with priority <7 for next 5 instructions	
		mov     #0x55, W0
        mov     W0, NVMKEY
        mov     #0xAA, W0
        mov     W0, NVMKEY		
		mov     w7,NVMCON		; Start Program Operation
        nop
        nop


erase_wait:     
		btsc    NVMCON, #WR
        bra     erase_wait

		clr 	w0
		pop     TBLPAG
        return


/******************************************************************************
  Flash Page Program
  dsPic33:	Program EIGHT rows (PAGE) of memory, 
  			Each row contains 192 bytes of data (64 instructions, with upper PM byte == NOP) 
  dsPic30:	Program ONE row (PAGE) of memory, 
  			Each row contains 96 bytes of data (32 instructions, with upper PM byte == NOP) 
*******************************************************************************/
_flashPageWrite:
        push    TBLPAG
        mov     w0, TBLPAG		; Init Pointer to row to be programed
		
		mov		#0,w6
		mov		#ROW_PER_PAGE,w5			; w5=row counter
row_loop:
	    mov 	#INSTRUCTION_PER_ROW,w3

	    	    
pinst_loop: 
		tblwtl  [W2++],[W1]
		tblwth  w6,[W1++]		; load 0x00 into 3rd byte (will be decoded as NOP should PC attempt to execute)

        dec     w3, w3
        bra     nz, pinst_loop
		
		/*Device ID errata workaround */
;		sub #8,w1			;move pointer back to the address with MSB 0x18
;		sub #8,w2			;move pointer back to the data for address with MSB 0x18
;		tblwtl  [W2],[W1] ;reload latch at the address with MSB 0x18 with original data

								; Setup NVCON for row program
		mov   	#FLASH_ROW_PROG_CODE,w7
        mov     w7, NVMCON
		bset 	w7,#WR
		disi 	#5				; Block all interrupt with priority <7 for next 5 instructions	
		mov     #0x55, W0
        mov     W0, NVMKEY
        mov     #0xAA, W0
        mov     W0, NVMKEY		
		mov     w7,NVMCON		; Start Program Operation
        nop
        nop

prog_wait:     
		btsc    NVMCON, #WR
        bra     prog_wait

		dec     w5, w5
		bra     nz, row_loop

		clr 	w0
        pop     TBLPAG
        return


.end
