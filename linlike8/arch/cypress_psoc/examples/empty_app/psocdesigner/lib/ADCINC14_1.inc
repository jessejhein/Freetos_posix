;;*****************************************************************************
;;*****************************************************************************
;;  FILENAME:   ADCINC14_1.inc
;;  Version: 1.3, Updated on 2005/03/06 at 17:14:19
;;  Generated by PSoC Designer ver 4.2  b1013 : 02 September, 2004
;;
;;  DESCRIPTION:  Assembler declarations for the UMName user module interface
;;                for the 22/24/27xxx PSoC family of devices.
;;-----------------------------------------------------------------------------
;;  Copyright (c) Cypress MicroSystems 2000-2003. All Rights Reserved.
;;*****************************************************************************
;;*****************************************************************************


;--------------------------------------------------
; Constants for ADCINC14_1 API's.
;--------------------------------------------------
; Counter Constants
ADCINC14_1_bfCounter_Mask:             equ   01h
ADCINC14_1_bfCounter_INT_REG:          equ   0e1h

; PWM Constants
ADCINC14_1_bfPWM24_Mask:               equ   20h
ADCINC14_1_bfPWM24_INT_REG:            equ   0e1h

; Power Settings
ADCINC14_1_bfPOWERMASK:                equ   03h
ADCINC14_1_OFF:                        equ   00h
ADCINC14_1_LOWPOWER:                   equ   01h
ADCINC14_1_MEDPOWER:                   equ   02h
ADCINC14_1_HIGHPOWER:                  equ   03h

; Parameter Settings
ADCINC14_1_CALCTIME:                   equ   8fch
ADCINC14_1_fCOMPARE_TRUE:              equ   08h      ; Bit to enable compare True interrupts

; Functionality constants
ADCINC14_1_fFSW0:                      equ   10h      ; Switch Cap FSW0 switch enable
ADCINC14_1_NoAZ:                       equ   01h      ; Set if AutoZero is no enabled
ADCINC14_1_fAutoZero:                  equ   20h      ; Switch Cap AutoZero switch enable
ADCINC14_1_fDBLK_ENABLE:               equ   01h      ; Digital block enable bit
ADCINC14_1_fPULSE_WIDE:                equ   04h      ; Use full clock width pulse.

; fStatus definitions
ADCINC14_1_fDATA_READY:                equ   01h      ; This bit is set when data is available

; Data Format
ADCINC14_1_DATA_FORMAT:                equ   0

;--------------------------------------------------
; Register Address Constants for ADCINC14_1
;--------------------------------------------------
; ADCINC14 PSoC Block register Definitions
; Integrator Block Register Definitions
ADCINC14_1_bfAtoDcr0:   equ 88h
ADCINC14_1_bfAtoDcr1:   equ 89h
ADCINC14_1_bfAtoDcr2:   equ 8ah
ADCINC14_1_bfAtoDcr3:   equ 8bh

; Counter Block Register Definitions
ADCINC14_1_fCounterFN:  equ 20h
ADCINC14_1_fCounterSL:  equ 21h
ADCINC14_1_fCounterOS:  equ 22h
ADCINC14_1_bCount:  equ 20h
ADCINC14_1_bPeriod: equ 21h
ADCINC14_1_bCompare:    equ 22h
ADCINC14_1_bCounter_CR0:    equ 23h

; PWM24 Block Register Definitions
ADCINC14_1_bfPWM_LSB_FN:    equ 2ch
ADCINC14_1_bfPWM_ISB_FN:    equ 30h
ADCINC14_1_bfPWM_MSB_FN:    equ 34h
ADCINC14_1_fPWM_LSB_CR0:    equ 2fh
ADCINC14_1_fPWM_ISB_CR0:    equ 33h
ADCINC14_1_fPWM_MSB_CR0:    equ 37h
ADCINC14_1_bPWM_Count_LSB:  equ 2ch
ADCINC14_1_bPWM_Count_ISB:  equ 30h
ADCINC14_1_bPWM_Count_MSB:  equ 34h
ADCINC14_1_bPWM_Period_LSB: equ 2dh
ADCINC14_1_bPWM_Period_ISB: equ 31h
ADCINC14_1_bPWM_Period_MSB: equ 35h
ADCINC14_1_bPWM_IntTime_LSB:    equ 2eh
ADCINC14_1_bPWM_IntTime_ISB:    equ 32h
ADCINC14_1_bPWM_IntTime_MSB:    equ 36h
ADCINC14_1_bfPWM_LSB_FN:    equ 2ch
ADCINC14_1_bfPWM_ISB_FN:    equ 30h
ADCINC14_1_bfPWM_MSB_FN:    equ 34h


; end of file ADCINC14_1.inc
