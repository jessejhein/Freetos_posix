;@Id: boot.tpl#46 @
;=============================================================================
;  FILENAME:   boot.asm
;  VERSION:    4.04
;  DATE:       18 November 2003
;
;  DESCRIPTION:
;  M8C Boot Code for CY8C27xxx microcontroller family.
;
;  Copyright (C) Cypress MicroSystems 2001-2003. All rights reserved.
;
; NOTES:
; PSoC Designer's Device Editor uses a template file, BOOT.TPL, located in
; the project's root directory to create BOOT.ASM. Any changes made to 
; BOOT.ASM will be  overwritten every time the project is generated; therfore
; changes should be made to BOOT.TPL not BOOT.ASM. Care must be taken when
; modifying BOOT.TPL so that replacement strings (such as @PROJECT_NAME)
; are not accidentally modified.
;
;=============================================================================

include ".\lib\GlobalParams.inc"
include "m8c.inc"
include "m8ssc.inc"

;-----------------------------------------------------------------------------
; Optimization flags
;-----------------------------------------------------------------------------
C_LANGUAGE_SUPPORT: equ 1   ;Set to 0 to optimize for ASM only

;-----------------------------------------------------------------------------
; The following equate is required for proper operation. Reseting its value
; is discouraged.  WAIT_FOR_32K is effective only if the crystal oscillator is
; selected.  If the designer chooses to not wait then the ECO and PLL_Lock
; processes must take place within user code. See the family data sheet for 
; the requirements of starting the ECO and PLL lock mode.
;-----------------------------------------------------------------------------
WAIT_FOR_32K:       equ 1   ; Wait for Crystal to start before allowing main()

;-----------------------------------------------------------------------------
; Export Declarations
;-----------------------------------------------------------------------------

export __Start
export __Exit
export __bss_start

export __lit_start
export __idata_start
export __data_start
export __func_lit_start
export __text_start
export __usermodules_start
export __psoc_config_start

;-----------------------------------------------------------------------------
; Interrupt Vector Table
;-----------------------------------------------------------------------------
;
; Interrupt vector table entries are 4 bytes long and contain the code that
; services the interrupt (or causes it to be serviced).
;
;-----------------------------------------------------------------------------

    AREA    TOP(ROM,ABS,CON)

    org 0                   ;Reset Interrupt Vector
    ljmp __Start             ;First instruction executed following a Reset

    org 04h                 ;Supply Monitor Interrupt Vector
    halt                    ;Stop execution if power falls too low

    org 08h                 ;Analog Column 0 Interrupt Vector
    `@INTERRUPT_2`
    reti

    org 0Ch                 ;Analog Column 1 Interrupt Vector
    `@INTERRUPT_3`
    reti

    org 10h                 ;Analog Column 2 Interrupt Vector
    `@INTERRUPT_4`
    reti

    org 14h                 ;Analog Column 3 Interrupt Vector
    `@INTERRUPT_5`
    reti
    
    org 18h                 ;VC3 Interrupt Vector
    `@INTERRUPT_6`
    reti
    
    org 1Ch                 ;GPIO Interrupt Vector
    `@INTERRUPT_7`
    reti
    
    org 20h                 ;PSoC Block DBB00 Interrupt Vector
    `@INTERRUPT_8`
    reti

    org 24h                 ;PSoC Block DBB01 Interrupt Vector
    `@INTERRUPT_9`
    reti

    org 28h                 ;PSoC Block DCB02 Interrupt Vector
    `@INTERRUPT_10`
    reti

    org 2Ch                 ;PSoC Block DCB03 Interrupt Vector
    `@INTERRUPT_11`
    reti

    org 30h                 ;PSoC Block DBB10 Interrupt Vector
    `@INTERRUPT_12`
    reti

    org 34h                 ;PSoC Block DBB11 Interrupt Vector
    `@INTERRUPT_13`
    reti

    org 38h                 ;PSoC Block DCB12 Interrupt Vector
    `@INTERRUPT_14`
    reti

    org 3Ch                 ;PSoC Block DCB13 Interrupt Vector
    `@INTERRUPT_15`
    reti
	
    // 40h through 5Fh are reserved for future products

    org 60h                 ;PSoC I2C Interrupt Vector
    `@INTERRUPT_24`
    reti

    org 64h                 ;Sleep Timer Interrupt Vector
    `@INTERRUPT_25`
    reti

;-----------------------------------------------------------------------------
;  Start of Execution
;  CPU is operating at 3 MHz, change to 12 MHz
;  IO Bank is Bank0
;-----------------------------------------------------------------------------
    org 68h
__Start:
IF (WATCHDOG_ENABLE)
    M8C_EnableWatchDog      ;Enable the WDT if selected in Global Params
ENDIF
IF (SELECT_32K)
    or  reg[CPU_SCR1],CPU_SCR1_ECO_ALLOWED    ;ECO will be used in this project
ELSE
    and reg[CPU_SCR1],~CPU_SCR1_ECO_ALLOWED   ;Prevent ECO from being enabled
ENDIF
    ;-------------------------------------------------------------------------
    ; Set the AGND Bypass bit. This is found in the BandGap Trim register, so
    ; the trim value must be read from the Trim Table. That is all that is 
    ; done for 5V operation if the AGND Bypass pin is chosen. For 3.3V
    ; operation the IMO Clock and BandGap trim registers always have to be set
    ; (they default to 5V trims).
    ;-------------------------------------------------------------------------
IF (SUPPLY_VOLTAGE)                ; 1 means 5.0V
IF (AGND_BYPASS)                   ; need to set the AGNDBYP bit in BDG_TR
    mov  [bSSC_TABLE_TableId],1    ; Point to the Trim table
    SSC_Action TABLE_READ          ; Perform a table read supervisor call
    M8C_SetBank1
    mov  A,[VOLTAGE_TRIM_5V]       ; Get the bandgap trim seting for 5V 
    or   A,AGND_BYPASS_JUST        ; OR in the bypass setting
    mov  reg[BDG_TR],A             ; Update the register value
    M8C_SetBank0
ENDIF
ELSE                               ; 0 means 3.3V
    mov  [bSSC_TABLE_TableId],1    ; Point to the Trim table
    SSC_Action TABLE_READ          ; Perform a table read supervisor call
    M8C_SetBank1
    mov  A,[OSCILLATOR_TRIM_3V]   
    mov  reg[IMO_TR],A             ; Load the 3V trim oscillator setting
    mov  A,[VOLTAGE_TRIM_3V]
    or   A,AGND_BYPASS_JUST        ; OR in the bypass setting
    mov  reg[BDG_TR],A             ; Load the bandgap trim setting for 3V
    M8C_SetBank0
ENDIF ;(SUPPLY_VOLTAGE)

    mov  [bSSC_KEY1],0             ; Lockout Flash and Supervisiory operations
    mov  [bSSC_KEYSP],0      

    mov  A,_stack_start           ; Set top of stack to end of used RAM
    swap SP,A

    ;-------------------------------------------------------------------------
    ; If the user has requested the Crystal oscillator then turn it on and
    ; wait for it to stabilize and the system to switch over to it.  The wait
    ; will be one sleep timer period, approximately 1 second.
    ;-------------------------------------------------------------------------

IF (SELECT_32K & WAIT_FOR_32K)
    ; Start the ECO and and leave the PLL off
    ; Set SleepTimer to 1 sec to time the wait for the ECO to stabilize
    M8C_SetBank1
    mov  reg[OSC_CR0],(SELECT_32K_JUST | OSC_CR0_SLEEP_1Hz | OSC_CR0_CPU_12MHz) 
    M8C_SetBank0
    M8C_ClearWDTAndSleep            ; Reset the sleep timer to get a full second

    ; The sleep interrupt will be used for timing Xtal and PLL startup.
    or   reg[INT_MSK0],INT_MSK0_SLEEP

    ; Wait for a SleepTimer interrupt to tell that 1 sec has passed
    mov  reg[INT_VC],0              ; Clear all pending interrupts
.WaitFor1s:
    tst  reg[INT_CLR0],INT_MSK0_SLEEP   ; test the Interrupt Status
    jz  .WaitFor1s                  ; TimeOut occurs on Sleep Timer 1s

ELSE ;!(SELECT_32K & WAIT_FOR_32K)
    ; Either no ECO, or waiting for stable clock is to be done in main
    M8C_SetBank1
    mov  reg[OSC_CR0],(SELECT_32K_JUST | PLL_MODE_JUST | SLEEP_TIMER_JUST | OSC_CR0_CPU_12MHz)
    M8C_SetBank0
    M8C_ClearWDTAndSleep            ; Reset the watch dog

ENDIF ;(SELECT_32K & WAIT_FOR_32K)

    ;-------------------------------------------------------------------------
    ; Crystal is now fully operational (assuming WAIT_FOR_32K was left 
    ; enabled). Now start up PLL lock mode if selected.
    ;-------------------------------------------------------------------------
IF (PLL_MODE)
    M8C_SetBank1
    mov  reg[OSC_CR0],(SELECT_32K_JUST | PLL_MODE_JUST | OSC_CR0_SLEEP_64Hz | OSC_CR0_CPU_3MHz)
    M8C_SetBank0
    M8C_ClearWDTAndSleep            ; Reset the sleep timer

    mov  reg[INT_VC],0              ; Clear all pending interrupts
.WaitFor16ms:
    tst  reg[INT_CLR0],INT_MSK0_SLEEP   ; test the Interrupt Status
    jz  .WaitFor16ms                ; TimeOut occurs on Sleep Timer 16ms
IF  (WAIT_FOR_32K)
ELSE ;!(WAIT_FOR_32K)
    ERROR_PSoC Disabling WAIT_FOR_32K requires that the PLL_Lock must be enabled in user code.
ENDIF ;(WAIT_FOR_32K)
ENDIF ;(PLL_MODE)

    ;-------------------------------------------------------------------------
    ; Set CT block RTopMux to OUT and RBotMux to AGND.  This closes leakage 
    ; paths through the CT blocks.
    ;-------------------------------------------------------------------------
    mov  reg[ACB00CR0],05h
    mov  reg[ACB01CR0],05h
    mov  reg[ACB02CR0],05h
    mov  reg[ACB03CR0],05h

    ;-------------------------------------------------------------------------
    ; All the user selections and UserModule selections are now loaded,
    ; except CPU frequency (CPU is runing at 12 MHz).  Load the PSoC 
    ; configuration with a 12 MHz CPU clock to keep config time short.
    ;-------------------------------------------------------------------------
    lcall LoadConfigInit           ; Configure PSoC blocks per Dev Editor


IF (C_LANGUAGE_SUPPORT)
    call InitCRunTime              ; Initialize for C language
ENDIF ;(C_LANGUAGE_SUPPORT)

    
IF (SUPPLY_VOLTAGE)
    ;-------------------------------------------------------------------------
    ; Nominal 5.0 V operating voltage is selected.
    ; Set the Precision Power-On Reset (PPOR) level for the operating voltage
    ; range selected. In addition, if the SMP is running make sure time was 
    ; given to let Vdd slew up to the set voltage before enabling the reset.
    ;-------------------------------------------------------------------------

IF (SWITCH_MODE_PUMP ^ 1)
    ;-------------------------------------------------------------------------
    ; If using the SMP at 5V, must wait for Vdd to slew from 3.1V to 5V
    ; The sleep interrupt will be used for timing Xtal and PLL startup.
    ;-------------------------------------------------------------------------
    or   reg[INT_MSK0],INT_MSK0_SLEEP   

    M8C_SetBank1
    and  reg[OSC_CR0],~OSC_CR0_SLEEP
    or   reg[OSC_CR0],OSC_CR0_SLEEP_512Hz
    M8C_SetBank0

    M8C_ClearWDTAndSleep         ; Reset the sleep timer

    mov  reg[INT_VC],0           ; Clear all pending interrupts
.WaitFor2ms:
    tst  reg[INT_CLR0],INT_MSK0_SLEEP   ; test the Interrupt Status
    jz  .WaitFor2ms              ; TimeOut occurs on Sleep Timer 2ms
ENDIF ;(SWITCH_MODE_PUMP ^ 1)

IF (CPU_CLOCK_JUST ^ OSC_CR0_CPU_24MHz) ; Clock is not 24MHz
    ;-------------------------------------------------------------------------
    ; <24 MHz operation is requested, so set PPOR to 3.0V.  If a PPOR level
    ;  of 4.5V is desired, then VLT_CR can be set in user code.
    ;-------------------------------------------------------------------------
    M8C_SetBank1
    and  reg[VLT_CR],~VLT_CR_PORLEV
    or   reg[VLT_CR],VLT_CR_3V0_POR
    M8C_SetBank0

ELSE ;!(CPU_CLOCK_JUST ^ OSC_CR0_CPU_24MHz)
    ;-------------------------------------------------------------------------
    ; 24 MHz operation, so Vdd >= 4.75V required.
    ;-------------------------------------------------------------------------
    M8C_SetBank1
    and  reg[VLT_CR],~VLT_CR_PORLEV
    or   reg[VLT_CR],VLT_CR_4V75_POR
    M8C_SetBank0
ENDIF ;(CPU_CLOCK_JUST ^ OSC_CR0_CPU_24MHz)

ELSE ;!(SUPPLY_VOLTAGE)
    ;-------------------------------------------------------------------------
    ; Nominal 3.3 V operating voltage is selected, set the PPOR to 3.0V.
    ;-------------------------------------------------------------------------
    M8C_SetBank1
    and  reg[VLT_CR],~VLT_CR_PORLEV
//  or   reg[VLT_CR],VLT_CR_3V0_POR   ; not needed, 0b00 is 3.0V
    M8C_SetBank0

ENDIF ;(SUPPLY_VOLTAGE)

    ;-------------------------------------------------------------------------
    ; Disable the Sleep interrupt that was used for timing above.  In fact,
    ; no interrupts should be enabled now, so may as well clear the register.
    ;-------------------------------------------------------------------------
    mov  reg[INT_MSK0],0

    ;-------------------------------------------------------------------------
    ; Everything has started OK. Now select requested CPU & sleep frequency.
    ;-------------------------------------------------------------------------
    M8C_SetBank1
    mov  reg[OSC_CR0],(SELECT_32K_JUST | PLL_MODE_JUST | SLEEP_TIMER_JUST | CPU_CLOCK_JUST)
    M8C_SetBank0

    ;-------------------------------------------------------------------------
    ; Global Interrupt are NOT enabled, this should be done in main().
    ; LVD is set but will not occur unless Global Interrupts are enabled. 
    ; Global Interrupts should be as soon as possible in main().
    ;-------------------------------------------------------------------------
    mov  reg[INT_VC],0              ; Clear any pending interrupts which may
                                    ; have been set during the boot process. 
    lcall _main                     ; Call main

__Exit:
    jmp  __Exit                     ; Wait here till power is turned off



;-----------------------------------------------------------------------------
; C Runtime Environment Initialization
; The following code is conditionally assembled.
;-----------------------------------------------------------------------------

IF (C_LANGUAGE_SUPPORT)

InitCRunTime:
    ;-----------------------------
    ; clear bss segment
    ;-----------------------------
    mov  A,0
    mov  [__r0],<__bss_start
BssLoop:
    cmp  [__r0],<__bss_end
    jz   BssDone
    mvi  [__r0],A
    jmp  BssLoop
BssDone:
    ;----------------------------
    ; copy idata to data segment
    ;----------------------------
    mov  A,>__idata_start
    mov  X,<__idata_start
    mov  [__r0],<__data_start
IDataLoop:
    cmp  [__r0],<__data_end
    jz   IDataDone
    push A
    romx
    mvi  [__r0],A
    pop  A
    inc  X
    adc  A,0
    jmp  IDataLoop
IDataDone:
    ret

ENDIF ;(C_LANGUAGE_SUPPORT)


;-----------------------------------------------------------------------------
; RAM segments for C CONST, static & global items
;-----------------------------------------------------------------------------
    AREA lit
__lit_start:

    AREA idata
__idata_start:

    AREA func_lit
__func_lit_start:

    AREA psoc_config(ROM,REL,CON)
__psoc_config_start:

    AREA UserModules(ROM,REL,CON)
__usermodules_start:
;---------------------------------------------
;         CODE segment for general use
;---------------------------------------------
    AREA text(ROM,REL,CON)
__text_start:

;---------------------------------------------
;         Begin RAM area usage
;---------------------------------------------
    AREA data(RAM,REL,CON)
__data_start:

    AREA virtual_registers(RAM,REL,CON)
__virtual_registers_end:

;---------------------------------------------
;         RAM segment for general use
;---------------------------------------------
    AREA bss(RAM,REL,CON)
__bss_start:

;end of file
