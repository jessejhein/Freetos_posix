;;*****************************************************************************
;;*****************************************************************************
;;  FILENAME:   UART_1Plus.asm
;;  Version: 4.2, Updated on 2003/11/18 at 15:02:35
;;  Generated by PSoC Designer ver 4.1 b931 : 21 January, 2004
;;
;;  DESCRIPTION:
;;   Optional higher level firmware for the UART User Module.  Implemented
;;   for the 22/24/25/26/27xxx families.
;;
;;  NOTE: User Module APIs conform to the fastcall convention for marshalling
;;        arguments and observe the associated "Registers are volatile" policy.
;;        This means it is the caller's responsibility to preserve any values
;;        in the X and A registers that are still needed after the API
;;        function returns. Even though these registers may be preserved now,
;;        there is no guarantee they will be preserved in future releases.
;;-----------------------------------------------------------------------------
;;  Copyright (c) Cypress MicroSystems 2003. All Rights Reserved.
;;*****************************************************************************
;;*****************************************************************************


;-----------------------------------------------
; include instance specific register definitions
;-----------------------------------------------
include "m8c.inc"
include "UART_1.inc"

area text (ROM, REL)

;-----------------------------------------------
;  Global Symbols
;-----------------------------------------------
;-------------------------------------------------------------------
;  Declare the functions global for both assembler and C compiler.
;
;  Note that there are two names for each API. First name is
;  assembler reference. Name with underscore is name refence for
;  C compiler.  Calling function in C source code does not require
;  the underscore.
;-------------------------------------------------------------------
export  UART_1_IntCntl
export _UART_1_IntCntl

export  UART_1_TxIntMode
export _UART_1_TxIntMode

export  UART_1_PutSHexByte
export _UART_1_PutSHexByte
export  UART_1_PutSHexInt
export _UART_1_PutSHexInt

export  UART_1_CPutString
export _UART_1_CPutString
export  UART_1_PutString
export _UART_1_PutString
export  UART_1_PutChar
export _UART_1_PutChar
export  UART_1_Write
export _UART_1_Write
export  UART_1_CWrite
export _UART_1_CWrite

export  UART_1_cGetChar
export _UART_1_cGetChar
export  UART_1_cReadChar
export _UART_1_cReadChar
export  UART_1_iReadChar
export _UART_1_iReadChar
export  UART_1_PutCRLF
export _UART_1_PutCRLF

IF (UART_1_RXBUF_ENABLE)
export  UART_1_CmdReset
export _UART_1_CmdReset
export  UART_1_bCmdCheck
export _UART_1_bCmdCheck
export  UART_1_bCmdLength
export _UART_1_bCmdLength
export  UART_1_bErrCheck
export _UART_1_bErrCheck

export  UART_1_szGetParam
export _UART_1_szGetParam
export  UART_1_szGetRestOfParams
export _UART_1_szGetRestOfParams


;-----------------------------------------------
; Variable Allocation
;-----------------------------------------------
area bss(RAM)
 ptrParam:   BLK  1

ENDIF

area text (ROM,REL)

;-----------------------------------------------
;  EQUATES
;-----------------------------------------------
bfCONTROL_REG_START_BIT:   equ   1     ; Control register start bit

AREA UserModules (ROM, REL)


.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: UART_1_IntCntl
;
;  DESCRIPTION:
;     This function is used to enable/disable the Rx and Tx interrupt.
;
;  ARGUMENTS:
;     A => Interrupt mask
;
;  RETURNS:
;     none.
;
;  SIDE EFFECTS:
;    REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;  THEORY of OPERATION or PROCEDURE:
;     Set or Clears the Tx/Rx user module interrupt enable mask bit in the TX
;     and RX block.
;
;-----------------------------------------------------------------------------
 UART_1_IntCntl:
_UART_1_IntCntl:

   push A
   and  A,UART_1_ENABLE_RX_INT
   jz   .DisRxInt
     ; Enable Rx Interrupt
   M8C_EnableIntMask UART_1_RX_INT_REG, UART_1_RX_INT_MASK
   jmp  .CheckTxInt
.DisRxInt:
     ; Disable Rx Interrupt
   M8C_DisableIntMask UART_1_RX_INT_REG, UART_1_RX_INT_MASK

.CheckTxInt:
   pop  A
   and  A,UART_1_ENABLE_TX_INT
   jz   .DisTxInt
     ; Enable Tx Interrupt
   M8C_EnableIntMask UART_1_TX_INT_REG, UART_1_TX_INT_MASK
   jmp  .End_IntCntl
.DisTxInt:
     ; Disable Tx Interrupt
   M8C_DisableIntMask UART_1_TX_INT_REG, UART_1_TX_INT_MASK

.End_IntCntl:
   ret
.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: UART_1_TxIntMode
;
;  DESCRIPTION:
;     This function is used to change the TX Interrupt mode.
;
;  ARGUMENTS:
;     A => Tx Interrupt mode
;             0 => Interrupt on TX_Reg_Empty  (Default)
;             1 => Interrupt on TX Complete
;
;  RETURNS:
;     none.
;
;  SIDE EFFECTS:
;    REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;  THEORY of OPERATION or PROCEDURE:
;
;
;-----------------------------------------------------------------------------
 UART_1_TxIntMode:
_UART_1_TxIntMode:

   and  A,UART_1_INT_MODE_TX_COMPLETE
   M8C_SetBank1
   cmp  A,UART_1_INT_MODE_TX_COMPLETE
   jz   .SetTxCmpMode

   and  reg[UART_1_TX_FUNC_REG],0xEF             ; Set Interrupt on Tx_Reg_Empty
   jmp  .TxIntMode_End

.SetTxCmpMode:                                   ; Set Interrupt on TX Complete
   or   reg[UART_1_TX_FUNC_REG],0x10

.TxIntMode_End:
   M8C_SetBank0
   ret
.ENDSECTION

;-----------------------------------------------------------------------------
;  FUNCTION NAME: UART_1_PutSHexByte
;
;  DESCRIPTION:
;     Print a byte in Hex (two characters) to the UART Tx
;
;  ARGUMENTS:
;     A  => (BYTE) Data/char to be printed
;
;  RETURNS:
;     none.
;
;  SIDE EFFECTS:
;    REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;-----------------------------------------------------------------------------
.LITERAL
UART_1_HEX_STR:
     DS    "0123456789ABCDEF"
.ENDLITERAL

.SECTION
 UART_1_PutSHexByte:
_UART_1_PutSHexByte:

    push  A                            ; Save lower nibble
    asr   A                            ; Shift high nibble to right
    asr   A
    asr   A
    asr   A
    and   A,0Fh                        ; Mask off nibble
    index UART_1_HEX_STR               ; Get Hex value
    call  UART_1_PutChar               ; Write data to screen
    pop   A                            ; Restore value
    and   A,0Fh                        ; Mask off lower nibble
    index UART_1_HEX_STR               ; Get Hex value
    call  UART_1_PutChar               ; Write data to screen
    ret
.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: UART_1_PutSHexInt
;
;  DESCRIPTION:
;     Print an Int in Hex (four characters) to UART Tx
;
;  ARGUMENTS:
;     Pointer to string
;     A  => ASB of Int
;     X  => MSB of Int
;
;  RETURNS:
;     none.
;
;  SIDE EFFECTS:
;    REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;
;-----------------------------------------------------------------------------

 UART_1_PutSHexInt:
_UART_1_PutSHexInt:
    swap  A,X
    call  UART_1_PutSHexByte           ; Print MSB
    mov   A,X                          ; Move LSB into position
    call  UART_1_PutSHexByte           ; Print LSB
    ret
.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: UART_1_PutChar
;
;  DESCRIPTION:
;     Send character out through UART TX port.
;
;
;  ARGUMENTS:
;     A has Character to send to UART Tx Port
;
;  RETURNS:
;     none
;
;  SIDE EFFECTS:
;    REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;  THEORY of OPERATION or PROCEDURE:
;     none.
;
;-----------------------------------------------------------------------------
 UART_1_PutChar:
_UART_1_PutChar:

   tst REG[UART_1_TX_CONTROL_REG], UART_1_TX_BUFFER_EMPTY    ; Check Tx Status
   jz  UART_1_PutChar
   mov REG[UART_1_TX_BUFFER_REG], A                      ; Write data to Tx Port
   ret

.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: UART_1_cGetChar
;
;  DESCRIPTION:
;     Read character from UART RX port.
;
;
;  ARGUMENTS:
;      none
;
;  RETURNS:
;     char that is returned from UART
;
;  SIDE EFFECTS:
;     REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;     Program flow will stay in this function until a character is received.
;     If the watchdog timer is used, care must be taken to make sure that
;     the delay between characters is less than the watchdog timeout.
;
;  THEORY of OPERATION or PROCEDURE:
;     none.
;
;-----------------------------------------------------------------------------
 UART_1_cGetChar:
_UART_1_cGetChar:

   tst REG[UART_1_RX_CONTROL_REG],UART_1_RX_REG_FULL   ; Check if a character is ready
   jz  UART_1_cGetChar                           ; If not loop

   mov A, REG[UART_1_RX_BUFFER_REG]              ; Get character
   ret
.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: UART_1_cReadChar
;
;  DESCRIPTION:
;     Read character from UART RX port.
;
;  ARGUMENTS:
;      none
;
;  RETURNS:
;     char that is returned from UART
;
;  SIDE EFFECTS:
;     REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;     A valid 0x00 character will be ignored, since a 0x00 return value
;     implies a valid character or an error condition occured.
;
;  THEORY of OPERATION or PROCEDURE:
;     none.
;
;-----------------------------------------------------------------------------
 UART_1_cReadChar:
_UART_1_cReadChar:


   mov  A,REG[UART_1_RX_CONTROL_REG]                       ; Get Status of RX
   push A
   and  A,UART_1_RX_COMPLETE                               ; Check if a character is ready
   jnz  .RX_DATA_RDY                                       ; Data Ready go read it.
   pop  A
   jmp  .RX_NO_VALID_CHAR

.RX_DATA_RDY:
   mov  A,REG[UART_1_RX_BUFFER_REG ]          
   swap A,X                                                ; determine if data is valid

   pop  A                                                  ; Check for errors
   and  A,(UART_1_RX_PARITY_ERROR | UART_1_RX_FRAMING_ERROR)
   jnz  .RX_NO_VALID_CHAR                                  ; No character, exit
   swap A,X                                                ; Put data in A and exit
   ret

.RX_NO_VALID_CHAR:
   mov A,0x00                                              ; Zero out character

 End_UART_1_cReadChar:
   ret
.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: UART_1_iReadChar
;
; WARNING WARNING WARNING  Negative return value not correct!!!!  We may want
; to just set a value in the upper byte if error conditions exists.
;
;  DESCRIPTION:
;     Read character from UART RX port.
;
;  ARGUMENTS:
;      none
;
;  RETURNS:
;     An integer value is returned.  A negative value inplies and error
;     condition, a positive value between 0 and 255 is the return character.
;
;     Error Codes:
;        0x80CC    Parity Error
;        0x40CC    Overrun Error
;        0x20CC    Framing Error
;        0x01CC    No Data available
;
;  SIDE EFFECTS:
;     REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;  THEORY of OPERATION or PROCEDURE:
;     none.
;
;-----------------------------------------------------------------------------
 UART_1_iReadChar:
_UART_1_iReadChar:


   mov  A,REG[UART_1_RX_CONTROL_REG]                       ; Get Status of RX
                                                           ; Mask only errors and data ready
   and  A,(UART_1_RX_ERROR|UART_1_RX_REG_FULL)
   push A
   and  A,UART_1_RX_COMPLETE                               ; Check if a character is ready
   jnz  .RX_GET_DATA                                       ; Data Ready go read it.
   pop  A
   or   A,UART_1_RX_NO_DATA                                ; Add no data flag
   swap A,X
   jmp  End_UART_1_iReadChar

.RX_GET_DATA:
   pop  A
   and  A,UART_1_RX_ERROR
   swap A,X
   mov  A,REG[UART_1_RX_BUFFER_REG ]                       ; Read data first, then
                                                           ; determine if data is valid

 End_UART_1_iReadChar:
   ret
.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: UART_1_PutString
;
;  DESCRIPTION:
;     Send String out through UART TX port.
;
;
;  ARGUMENTS:
;     Pointer to String
;     A has MSB of string address
;     X has LSB of string address
;
;  RETURNS:
;     none
;
;  SIDE EFFECTS:
;     REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;  THEORY of OPERATION or PROCEDURE:
;     none.
;
;-----------------------------------------------------------------------------
 UART_1_PutString:
_UART_1_PutString:

   mov   A,[X]                             ; Get value pointed to by X
   jz    End_PutString                     ; Check for end of string
   call  UART_1_PutChar                     ; Send character to Tx port
   inc   X                                 ; Advance pointer to next character
   jmp   UART_1_PutString                   ; Get next character

End_PutString:
   ret
.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: UART_1_Write
;
;  DESCRIPTION:
;     Send String of length X to serial port
;
;
;  ARGUMENTS:
;     Pointer to String
;     [X]   has MSB of string address
;     [X-1] has LSB of string address
;     [X-2] Count of characters to send
;
;  RETURNS:
;     none
;
;  SIDE EFFECTS:
;     REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;  THEORY of OPERATION or PROCEDURE:
;     none.
;
;-----------------------------------------------------------------------------
STR_LSB:    equ  0           ; LSB pointer of string
STR_MSB:    equ -1           ; MSB pointer of string
CNT_LEN:    equ -2           ; Length of data to send

 UART_1_Write:
_UART_1_Write:

   mov   A,[X+CNT_LEN]                     ; Get length of string to send
   jz    End_Write
   dec   [X+CNT_LEN]                       ; Decrement counter
   push  X
   mov   X,[X+STR_LSB]                     ; Get character to send
   mov   A,[X]
   call  UART_1_PutChar                     ; Send character to UART
   pop   X
   inc   [X+STR_LSB]
   jmp   UART_1_Write

End_Write:
   ret
.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: UART_1_CWrite
;
;             WARNING WARNING NOT COMPLETE
;
;  DESCRIPTION:
;     Send String of length X to serial port
;
;  ARGUMENTS:
;     Pointer to String
;     [X]   has LSB of string address
;     [X-1] has MSB of string address
;     [X-2] LSB of Count of character to send
;     [X-3] MSB of Count of character to send
;
;  RETURNS:
;     none
;
;  SIDE EFFECTS:
;     REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;  THEORY of OPERATION or PROCEDURE:
;     none.
;
;-----------------------------------------------------------------------------
CSTR_LSB:   equ  0           ; LSB pointer of string
CSTR_MSB:   equ -1           ; MSB pointer of string
CLEN_LSB:   equ -2           ; LSB Length of data to send
CLEN_MSB:   equ -3           ; MSB Length of data to send

 UART_1_CWrite:
_UART_1_CWrite:
                                            ; Check for zero counter
   cmp   [X+CLEN_MSB],0x00
   jnz   .CW_WRITEIT
   cmp   [X+CLEN_LSB],0x00
   jz    .End_CWrite                         ; Leave if done

.CW_WRITEIT:                                 ; Save pointers
   push  X
   mov   A,[X+CSTR_MSB]
   mov   X,[X+CSTR_LSB]
   romx                                     ; Get character from ROM
   call  UART_1_PutChar                      ; Send Char
   pop   X

   add   [X+CSTR_LSB],1                     ; Increment the string pointer
   add   [X+CSTR_MSB],0

                                            ; Dec the counter
   sub   [X+CLEN_LSB],0x01
   sbb   [X+CLEN_MSB],0x00

   jmp   UART_1_CWrite

.End_CWrite:
   ret
.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: UART_1_CPutString
;
;  DESCRIPTION:
;     Send String out through UART TX port.
;
;
;  ARGUMENTS:
;     Pointer to String
;     A has MSB of string address
;     X has LSB of string address
;
;  RETURNS:
;     none
;
;  SIDE EFFECTS:
;     REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;  THEORY of OPERATION or PROCEDURE:
;     none.
;
;-----------------------------------------------------------------------------
 UART_1_CPutString:
_UART_1_CPutString:

   push  A                                       ; Store ROM pointer
   push  X
   romx                                          ; Get character from ROM
   jz    .End_CPutString
   call  UART_1_PutChar                     ; Print character
   pop   X
   pop   A
   inc   X                                       ; Inc LSB of pointer
   jnc   UART_1_CPutString                       ; Check for carry
   inc   A                                       ; Inc MSB of pointer
   jmp   UART_1_CPutString


.End_CPutString:
   pop   X                                       ; Don't really have to restore A and X
   pop   X                                       ; but need to correct the stack.
   ret
.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: UART_1_PutCRLF
;
;  DESCRIPTION:
;     Send a CR and LF
;
;  ARGUMENTS:
;     none.
;
;  RETURNS:
;     none.
;
;  SIDE EFFECTS:
;     REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;  THEORY of OPERATION or PROCEDURE:
;
;-----------------------------------------------------------------------------
 UART_1_PutCRLF:
_UART_1_PutCRLF:

   mov  A,0x0D                        ; Send CR
   call UART_1_PutChar
   mov  A,0x0A                        ; Send LF
   call UART_1_PutChar
   ret
.ENDSECTION


IF (UART_1_RXBUF_ENABLE)
.SECTION
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------
;
;     Command Buffer commands
;
;-----------------------------------------------------------------------------
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
;  FUNCTION NAME: UART_1_CmdReset
;
;  DESCRIPTION:
;     Reset command string and status flags
;
;  ARGUMENTS:
;     none.
;
;  RETURNS:
;     none.
;
;  SIDE EFFECTS:
;     REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;  THEORY of OPERATION or PROCEDURE:
;     Clear the command buffer, command counter, and flag.
;
;-----------------------------------------------------------------------------
 UART_1_CmdReset:
_UART_1_CmdReset:

   mov [UART_1_aRxBuffer], 0x00
   mov [UART_1_bRxCnt], 0x00
   and [UART_1_fStatus], 0x00
   mov [ptrParam],0x00
   ret
.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: UART_1_bCmdCheck
;
;  DESCRIPTION:
;     Check to see if valid command in buffer.
;
;  ARGUMENTS:
;     none.
;
;  RETURNS:
;     BYTE  fStatus - Status of command receive buffer.
;                     Returns non-zero value in A if command is valid.
;
;  SIDE EFFECTS:
;     REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;  THEORY of OPERATION or PROCEDURE:
;     Read the status and control register.
;
;-----------------------------------------------------------------------------
 UART_1_bCmdCheck:
_UART_1_bCmdCheck:
   mov A,  [UART_1_fStatus]
   and A, UART_1_RX_BUF_CMDTERM                  ; Mask off Command status
   ret
.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: UART_1_bErrCheck
;
;  DESCRIPTION:
;     Check to see if an error has occured since last CmdReset
;
;  ARGUMENTS:
;     none.
;
;  RETURNS:
;     BYTE  fStatus - Status of command receive buffer.
;                     Returns non-zero value in A if command is valid.
;           0x80 => Parity Error
;           0x40 => OverRun Error
;           0x20 => Framing Error
;           0x10 => Software Buffer OverRun
;
;  SIDE EFFECTS:
;     Error Status is clear when read.
;     REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;  THEORY of OPERATION or PROCEDURE:
;     Read RX buffer error status and clear status
;
;-----------------------------------------------------------------------------
 UART_1_bErrCheck:
_UART_1_bErrCheck:
   mov A,  [UART_1_fStatus]
   and A, UART_1_RX_BUF_ERROR                    ; Mask off Error status
   and [UART_1_fStatus], ~UART_1_RX_BUF_ERROR
   ret
.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: UART_1_bCmdLength
;
;  DESCRIPTION:
;     Get length of command string
;
;  ARGUMENTS:
;     none.
;
;  RETURNS:
;     BYTE  bRxCnt    Returns the command length in A.
;
;  SIDE EFFECTS:
;     REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;  THEORY of OPERATION or PROCEDURE:
;
;-----------------------------------------------------------------------------
 UART_1_bCmdLength:
_UART_1_bCmdLength:
   mov A,  [UART_1_bRxCnt]
   ret
.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: UART_1_szGetParam
;
;  DESCRIPTION:
;      Return next parameter from UART Rx buffer
;
;
;  ARGUMENTS:  none
;
;     A   Parameter delimiter
;    [X]   LSB of buffer pointer
;
;  RETURNS:
;     A => MSB of parameter
;     X => LSB of parameter
;
;  SIDE EFFECTS:
;     REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;     The receive string is modified by placing Null characters at the end
;     of each parameter as they are recovered.
;
;  THEORY of OPERATION or PROCEDURE:
;     none.
;
;-----------------------------------------------------------------------------
 UART_1_szGetParam:
_UART_1_szGetParam:

    mov  A, <UART_1_aRxBuffer               ; Get address to receive buffer
    add  A, [ptrParam]                      ; Add string offset
    mov  X,A

    mov  A,[X]                              ; Get character pointed by X
    jnz  .CheckForDelim                     ; Check for Null character
    push X                                  ; Save LSB of current pointer
    jmp  .End_GetNextParam

                                            ; Check for delimiter and keep looping until
                                            ; all leading delimiters have been found.
.CheckForDelim:
    cmp  A,UART_1_DELIMITER                 ; Check if we have a delimiter
    jnz  .ParamStartFound
    inc  X                                  ; Increment both current pointer and
    inc  [ptrParam]                         ; stored pointer.
    mov  A,[X]                              ; Get character pointed by X
    cmp  [ptrParam],(UART_1_RX_BUFFER_SIZE -1)  ; Check if we are at the end of buffer
    jnz  .CheckForDelim
                                            ; End of string found
.EndOfString:
    push X                                  ; Save ptr
.TerminateString:
    mov  [X],0x00                           ; Make sure string is zero
    jmp  .End_GetNextParam

.ParamStartFound:
    push X                                  ; Beginning of parameter found, save pointer

.ParamLoop:
                                            ; Now loop until end of parameter found.
    inc  X                                  ; Advance pointers.
    inc  [ptrParam]
    cmp  [ptrParam],(UART_1_RX_BUFFER_SIZE -1)  ; Check if we are at the end of buffer
    jz   .TerminateString
    mov  A,[X]                              ; Get next character
    jz   .End_GetNextParam
    cmp  A,UART_1_DELIMITER                 ; Check if we have a delimiter
    jnz  .ParamLoop                         ; Still no delimiter, loop again

    mov  [X],0x00                           ; Replace delimiter with null for end of substring
    inc  [ptrParam]
    cmp  [ptrParam],(UART_1_RX_BUFFER_SIZE -1)  ; Check if we are at the end of buffer
    jnz  .End_GetNextParam                  ; If not end of string leave
    mov  [ptrParam],(UART_1_RX_BUFFER_SIZE -1)  ; Reset pointer to end of string.


.End_GetNextParam:
   pop  X
   push X
   cmp  [X],0x00
   jnz  .NotNullString
   pop  X
   mov  X,0x00
   mov  A,X
   ret

.NotNullString:
   pop  X
   mov  A,0x00                            ; Return pointer
   ret
.ENDSECTION

.SECTION
;-----------------------------------------------------------------------------
;  FUNCTION NAME: UART_1_szGetRestOfParams
;
;  DESCRIPTION:
;      Return the rest of the UART RX buffer
;
;
;  ARGUMENTS:  none
;
;  RETURNS:
;     A => MSB of parameter
;     X => LSB of parameter
;
;  SIDE EFFECTS:
;     REGISTERS ARE VOLATILE: THE A AND X REGISTERS MAY BE MODIFIED!
;
;  THEORY of OPERATION or PROCEDURE:
;     none.
;
;-----------------------------------------------------------------------------
 UART_1_szGetRestOfParams:
_UART_1_szGetRestOfParams:

    mov  A, <UART_1_aRxBuffer               ; Get address to receive buffer
    add  A, [ptrParam]                      ; Add string offset
    mov  X,A
    mov  A,0x00                             ; Return pointer
    ret
.ENDSECTION

ENDIF


; End of File UART_1Plus.asm
