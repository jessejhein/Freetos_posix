/**********************************************************************************************
 * File: 			eeprom_asm.s
 * Description:		utilities for read, erase, and write eeprom
 ***********************************************************************************************/

.section .text			;place code in text section (program flash for codes)

/**********************************************************************************************
 * Declaration for functions (Interface with C)
 ***********************************************************************************************/
.global _readEEByte		;void readEEByte(int tablepage, int eeoffset, char* pdata);
.global _eraseEEWord	;void eraseEEByte(int tablepage, int eeoffset);
.global _writeEEWord	;void writeEEByte(int tablepage, int eeoffset, int* pdata);

/**********************************************************************************************
 * Read a byte to buffer
 ***********************************************************************************************/
_readEEByte:
        push    	TBLPAG				;w0 = base of eeData
        mov     	w0, TBLPAG			;w1 = offset for eeData in byte
		tblrdl.b  	[w1], [w2]			;w2 = pointer to user buffer
		pop     	TBLPAG
        return

/**********************************************************************************************
 * Erase a word in EEPROM
 ***********************************************************************************************/
_eraseEEWord:
        push    	TBLPAG				
		mov 		w0, NVMADRU			;w0 = base of eeData
		mov			w1, NVMADR			;w1 = offset for eeData in word
		mov			#0x4044, w0			
		mov			w0, NVMCON			;Set to erase operation
		push 		SR					;Disable global interrupts
		mov 		#0x00E0, w0
		ior 		SR
		mov 		#0x55, w0 			;Write the KEY sequence
		mov 		w0, NVMKEY
		mov 		#0xAA, w0			
		mov 		w0, NVMKEY
		bset 		NVMCON, #15			;Start the erase cycle, bit 15 = WR
        nop
        nop
        pop     	SR					;Enable global interrupts
L1:     btsc    	NVMCON, #15			;while(NVMCONbits.WR)
        bra     	L1
        clr     	w0
		pop     	TBLPAG
		return

/**********************************************************************************************
 * Write a word to EEPROM 
 ***********************************************************************************************/
_writeEEWord:
        push    	TBLPAG				;w0 = base of eeData
        mov     	w0, TBLPAG			;w1 = offset for eeData in byte
		tblwtl  	[w2], [w1]			;w2 = pointer to user buffer
		mov 		#0x4004, w0			;Set to write operation
		MOV 		w0, NVMCON
		push 		SR					;Disable global interrupts
		mov 		#0x00E0, w0
		ior 		SR
		mov 		#0x55, w0 			;Write the KEY sequence
		mov 		w0, NVMKEY
		mov 		#0xAA, w0			
		mov 		w0, NVMKEY
		bset 		NVMCON, #15			;Start the erase cycle, bit 15 = WR
        nop
        nop
        pop     	SR					;Enable global interrupts
L2:     btsc    	NVMCON, #15			;while(NVMCONbits.WR);
        bra     	L2
        clr     	w0
		pop     	TBLPAG
        return
