;;*****************************************************************************
;;*****************************************************************************
;;  FILENAME:   UART_1INT.asm
;;  Version: 4.2, Updated on 2003/11/18 at 15:02:35
;;  Generated by PSoC Designer ver 4.1 b931 : 21 January, 2004
;;
;;  DESCRIPTION:  UART Interrupt Service Routine.
;;-----------------------------------------------------------------------------
;;  Copyright (c) Cypress MicroSystems 2000-2003. All Rights Reserved.
;;*****************************************************************************
;;*****************************************************************************


include  "UART_1.inc"
include  "m8c.inc"

;-----------------------------------------------
;  Global Symbols
;-----------------------------------------------
export  _UART_1_TX_ISR
export  _UART_1_RX_ISR

IF (UART_1_RXBUF_ENABLE)
export  UART_1_aRxBuffer
export _UART_1_aRxBuffer
export  UART_1_bRxCnt
export _UART_1_bRxCnt
export  UART_1_fStatus
export _UART_1_fStatus
ENDIF


;-----------------------------------------------
; Variable Allocation
;-----------------------------------------------
AREA bss(RAM)

IF (UART_1_RXBUF_ENABLE)
 UART_1_fStatus:
_UART_1_fStatus:      BLK  1
 UART_1_bRxCnt:
_UART_1_bRxCnt:       BLK  1
 UART_1_aRxBuffer:
_UART_1_aRxBuffer:    BLK UART_1_RX_BUFFER_SIZE
ENDIF


;@PSoC_UserCode_INIT@ (Do not change this line.)
;---------------------------------------------------
; Insert your custom declarations below this banner
;---------------------------------------------------

;------------------------
;  Constant Definitions
;------------------------


;------------------------
; Variable Allocation
;------------------------


;---------------------------------------------------
; Insert your custom declarations above this banner
;---------------------------------------------------
;@PSoC_UserCode_END@ (Do not change this line.)


AREA UserModules (ROM, REL)

;-----------------------------------------------------------------------------
;  FUNCTION NAME: _UART_1_TX_ISR
;
;  DESCRIPTION:
;     UART TX interrupt handler for instance UART_1.
;
;     This is a place holder function.  If the user requires use of an interrupt
;     handler for this function, then place code where specified.
;-----------------------------------------------------------------------------

_UART_1_TX_ISR:
   ;@PSoC_UserCode_BODY_1@ (Do not change this line.)
   ;---------------------------------------------------
   ; Insert your custom code below this banner
   ;---------------------------------------------------
   ;   NOTE: interrupt service routines must preserve
   ;   the values of the A and X CPU registers.

   ;---------------------------------------------------
   ; Insert your custom code above this banner
   ;---------------------------------------------------
   ;@PSoC_UserCode_END@ (Do not change this line.)
   reti


;-----------------------------------------------------------------------------
;  FUNCTION NAME: _UART_1_RX_ISR
;
;  DESCRIPTION:
;     UART RX interrupt handler for instance UART_1.
;     This ISR handles the background processing of received characters if
;     the buffer is enabled.
;
;
;  The following assumes that the RX buffer feature has been enabled.
;
;  SIDE EFFECTS:
;     There are 3 posible errors that may occur with the serial port.
;      1) Parity Error
;      2) Framing Error
;      3) OverRun Error
;
;  This user module check for parity and framing error.  If either of these
;  two errors are detected, the data is read and ignored.  When an overRun
;  error occurs, the last byte was lost, but the current byte is valid.  For
;  this reason this error is ignored at this time.  Code could be added to
;  this ISR to set a flag if an error condition occurs.
;
;  THEORY of OPERATION:
;     When using the RX buffer feature, the ISR collects received characters
;     in a buffer until the user defined command terminator is detected.  After
;     the command terminator is detected, the command bit is set and all other
;     characters will be ignored until the command bit is reset.  Up to
;     buffer_size - 1 characters will be collected waiting for a command
;     terminator.  After that, the characters will be discarded, although
;     a command determinator will still cause the command bit to be set.
;
;-----------------------------------------------------------------------------
_UART_1_RX_ISR:

   ;@PSoC_UserCode_BODY_2@ (Do not change this line.)
   ;---------------------------------------------------
   ; Insert your custom code below this banner
   ;---------------------------------------------------
   ;   NOTE: interrupt service routines must preserve
   ;   the values of the A and X CPU registers.

   ;---------------------------------------------------
   ; Insert your custom code above this banner
   ;---------------------------------------------------
   ;@PSoC_UserCode_END@ (Do not change this line.)

IF (UART_1_RXBUF_ENABLE)
   push A
   push X
   mov  X,[UART_1_bRxCnt]                                  ; Load X with byte counter
   mov  A,REG[UART_1_RX_CONTROL_REG]                       ; Read the control register
   push A                                                  ; Store copy for later test
                                                           ; IF real RX interrupt
   and  A,UART_1_RX_REG_FULL                               ; Did really really get an IRQ
   jnz  .UARTRX_ReadRx                                     ; Data ready, go get it
   pop  A                                                  ; Restore stack
   jmp  .END_UARTRX_ISR

.UARTRX_ReadRx:

   pop  A                                                  ; Restore status flags
                                                           ; IF there is no error, get data
                                                           ; Check for parity or framing error


   and  A,UART_1_RX_ERROR
   jz   .UARTRX_NO_ERROR                                   ; If there is not an Error go read data

   or   [UART_1_fStatus],A                                 ; Set error flags (parity,framing,overrun) bits

   mov  A,REG[UART_1_RX_BUFFER_REG ]                       ; Read the data buffer to clear it.

   and  A,UART_1_RX_FRAMING_ERROR                          ; Check for framing error special case
   jz   .END_UARTRX_ISR                                    ; Not framing error, all done

                                                           ; Disable and re-enable RX to reset after
                                                           ; framing error.
   and   REG[UART_1_RX_CONTROL_REG], ~UART_1_RX_ENABLE     ; Disable RX
   or    REG[UART_1_RX_CONTROL_REG],  UART_1_RX_ENABLE     ; Enable RX
   jmp  .END_UARTRX_ISR                                    ; Done with framing error, leave.


.UARTRX_NO_ERROR:
   mov  A,REG[UART_1_RX_BUFFER_REG ]                       ; Read the data buffer

                                                           ; IF buffer not full
   tst  [UART_1_fStatus],UART_1_RX_BUF_CMDTERM             ; Check for buffer full
   jnz  .END_UARTRX_ISR                                    ; All done

   cmp  A,UART_1_CMD_TERM                                  ; Check for End of command
   jnz  .UARTRX_CHK_CTLCHAR
   or   [UART_1_fStatus],UART_1_RX_BUF_CMDTERM             ; Set command ready bit
   mov  [X + UART_1_aRxBuffer],00h                         ; Zero out last data
   jmp  .END_UARTRX_ISR

.UARTRX_CHK_CTLCHAR:                                       ; Ignore charaters below this value
                                                           ; If ignore char is set to 0x00, do not
                                                           ; ignore any characters.
IF(UART_1_RX_IGNORE_BELOW)
   cmp  A,UART_1_RX_IGNORE_BELOW
   jc   .END_UARTRX_ISR
ENDIF

.UARTRX_CHK_OVFL:                                          ; Check for MAX String here
   cmp  [UART_1_bRxCnt],(UART_1_RX_BUFFER_SIZE - 1)
   jc   .UARTRX_ISR_GETDATA
   mov  [X + UART_1_aRxBuffer],00h                         ; Zero out last data
   or   [UART_1_fStatus],UART_1_RX_BUF_OVERRUN             ; Set error flags (parity,framing,overrun) bits
   jmp  .END_UARTRX_ISR

                                                           ; IF input data == "CR", then end of command
.UARTRX_ISR_GETDATA:
                                                           ; X is already loaded with pointer
   mov  [X+UART_1_aRxBuffer],A                             ; store data in array
   inc  X                                                  ; Inc the pointer
   mov  [UART_1_bRxCnt],X                                  ; Restore the pointer
                                                           ; ENDIF max string size

.END_UARTRX_ISR:
   pop  X
   pop  A

ENDIF

UART_1_RX_ISR_END:
   reti

; end of file UART_1INT.asm

