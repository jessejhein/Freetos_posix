; Generated by PSoC Designer ver 4.1 b931 : 21 January, 2004
;
include "m8c.inc"
;  Personalization tables 
export LoadConfigTBL_mwlike8_Bank1
export LoadConfigTBL_mwlike8_Bank0
export LoadConfigTBL_mwlike8_Ordered
AREA lit(rom, rel)
LoadConfigTBL_mwlike8_Ordered:
;  Ordered Global Register values
	M8C_SetBank1
	mov	reg[00h], ffh		; Port_0_DriveMode_0 register (PRT0DM0)
	mov	reg[01h], 00h		; Port_0_DriveMode_1 register (PRT0DM1)
	M8C_SetBank0
	mov	reg[03h], 00h		; Port_0_DriveMode_2 register (PRT0DM2)
	mov	reg[02h], 00h		; Port_0_GlobalSelect register (PRT0GS)
	M8C_SetBank1
	mov	reg[02h], 00h		; Port_0_IntCtrl_0 register (PRT0IC0)
	mov	reg[03h], 00h		; Port_0_IntCtrl_1 register (PRT0IC1)
	M8C_SetBank0
	mov	reg[01h], 00h		; Port_0_IntEn register (PRT0IE)
	M8C_SetBank1
	mov	reg[04h], ffh		; Port_1_DriveMode_0 register (PRT1DM0)
	mov	reg[05h], 00h		; Port_1_DriveMode_1 register (PRT1DM1)
	M8C_SetBank0
	mov	reg[07h], 00h		; Port_1_DriveMode_2 register (PRT1DM2)
	mov	reg[06h], 00h		; Port_1_GlobalSelect register (PRT1GS)
	M8C_SetBank1
	mov	reg[06h], 00h		; Port_1_IntCtrl_0 register (PRT1IC0)
	mov	reg[07h], 00h		; Port_1_IntCtrl_1 register (PRT1IC1)
	M8C_SetBank0
	mov	reg[05h], 00h		; Port_1_IntEn register (PRT1IE)
	M8C_SetBank1
	mov	reg[08h], ffh		; Port_2_DriveMode_0 register (PRT2DM0)
	mov	reg[09h], 00h		; Port_2_DriveMode_1 register (PRT2DM1)
	M8C_SetBank0
	mov	reg[0bh], 00h		; Port_2_DriveMode_2 register (PRT2DM2)
	mov	reg[0ah], 00h		; Port_2_GlobalSelect register (PRT2GS)
	M8C_SetBank1
	mov	reg[0ah], 00h		; Port_2_IntCtrl_0 register (PRT2IC0)
	mov	reg[0bh], 00h		; Port_2_IntCtrl_1 register (PRT2IC1)
	M8C_SetBank0
	mov	reg[09h], 00h		; Port_2_IntEn register (PRT2IE)
	M8C_SetBank1
	mov	reg[0ch], dfh		; Port_3_DriveMode_0 register (PRT3DM0)
	mov	reg[0dh], 2ah		; Port_3_DriveMode_1 register (PRT3DM1)
	M8C_SetBank0
	mov	reg[0fh], 00h		; Port_3_DriveMode_2 register (PRT3DM2)
	mov	reg[0eh], a0h		; Port_3_GlobalSelect register (PRT3GS)
	M8C_SetBank1
	mov	reg[0eh], 0ah		; Port_3_IntCtrl_0 register (PRT3IC0)
	mov	reg[0fh], 0ah		; Port_3_IntCtrl_1 register (PRT3IC1)
	M8C_SetBank0
	mov	reg[0dh], 0ah		; Port_3_IntEn register (PRT3IE)
	M8C_SetBank1
	mov	reg[10h], ffh		; Port_4_DriveMode_0 register (PRT4DM0)
	mov	reg[11h], 0ah		; Port_4_DriveMode_1 register (PRT4DM1)
	M8C_SetBank0
	mov	reg[13h], 00h		; Port_4_DriveMode_2 register (PRT4DM2)
	mov	reg[12h], 00h		; Port_4_GlobalSelect register (PRT4GS)
	M8C_SetBank1
	mov	reg[12h], 00h		; Port_4_IntCtrl_0 register (PRT4IC0)
	mov	reg[13h], 00h		; Port_4_IntCtrl_1 register (PRT4IC1)
	M8C_SetBank0
	mov	reg[11h], 00h		; Port_4_IntEn register (PRT4IE)
	M8C_SetBank1
	mov	reg[14h], 0fh		; Port_5_DriveMode_0 register (PRT5DM0)
	mov	reg[15h], 08h		; Port_5_DriveMode_1 register (PRT5DM1)
	M8C_SetBank0
	mov	reg[17h], 00h		; Port_5_DriveMode_2 register (PRT5DM2)
	mov	reg[16h], 00h		; Port_5_GlobalSelect register (PRT5GS)
	M8C_SetBank1
	mov	reg[16h], 08h		; Port_5_IntCtrl_0 register (PRT5IC0)
	mov	reg[17h], 08h		; Port_5_IntCtrl_1 register (PRT5IC1)
	M8C_SetBank0
	mov	reg[15h], 08h		; Port_5_IntEn register (PRT5IE)
	ret
LoadConfigTBL_mwlike8_Bank0:
;  Global Register values
	db		60h, 28h		; AnalogColumnInputSelect register (AMX_IN)
	db		66h, 00h		; AnalogComparatorControl1 register (CMP_CR1)
	db		63h, 15h		; AnalogReferenceControl register (ARF_CR)
	db		65h, 00h		; AnalogSyncControl register (ASY_CR)
	db		e6h, 00h		; DecimatorControl_0 register (DEC_CR0)
	db		e7h, 00h		; DecimatorControl_1 register (DEC_CR1)
	db		d6h, 00h		; I2CConfig register (I2CCFG)
	db		b0h, 0ch		; Row_0_InputMux register (RDI0RI)
	db		b1h, 00h		; Row_0_InputSync register (RDI0SYN)
	db		b2h, 00h		; Row_0_LogicInputAMux register (RDI0IS)
	db		b3h, 33h		; Row_0_LogicSelect_0 register (RDI0LT0)
	db		b4h, 33h		; Row_0_LogicSelect_1 register (RDI0LT1)
	db		b5h, 00h		; Row_0_OutputDrive_0 register (RDI0SRO0)
	db		b6h, 00h		; Row_0_OutputDrive_1 register (RDI0SRO1)
	db		b8h, 55h		; Row_1_InputMux register (RDI1RI)
	db		b9h, 00h		; Row_1_InputSync register (RDI1SYN)
	db		bah, 10h		; Row_1_LogicInputAMux register (RDI1IS)
	db		bbh, 33h		; Row_1_LogicSelect_0 register (RDI1LT0)
	db		bch, 33h		; Row_1_LogicSelect_1 register (RDI1LT1)
	db		bdh, 00h		; Row_1_OutputDrive_0 register (RDI1SRO0)
	db		beh, 80h		; Row_1_OutputDrive_1 register (RDI1SRO1)
;  Instance name Timer8_1, User Module Timer8
;       Instance name Timer8_1, Block Name TIMER8(DCB13)
	db		3fh, 04h		;Timer8_1_CONTROL_REG(DCB13CR0)
	db		3dh, a0h		;Timer8_1_PERIOD_REG(DCB13DR1)
	db		3eh, 00h		;Timer8_1_COMPARE_REG(DCB13DR2)
;  Instance name UART_1, User Module UART
;       Instance name UART_1, Block Name RX(DCB03)
	db		2fh, 00h		;UART_1_RX_CONTROL_REG(DCB03CR0)
	db		2dh, 00h		;UART_1_(DCB03DR1)
	db		2eh, 00h		;UART_1_RX_BUFFER_REG (DCB03DR2)
;       Instance name UART_1, Block Name TX(DCB12)
	db		3bh, 00h		;UART_1_TX_CONTROL_REG(DCB12CR0)
	db		39h, 00h		;UART_1_TX_BUFFER_REG (DCB12DR1)
	db		3ah, 00h		;UART_1_(DCB12DR2)
	db		ffh
LoadConfigTBL_mwlike8_Bank1:
;  Global Register values
	db		61h, 00h		; AnalogClockSelect1 register (CLK_CR1)
	db		69h, 00h		; AnalogClockSelect2 register (CLK_CR2)
	db		60h, 00h		; AnalogColumnClockSelect register (CLK_CR0)
	db		62h, 00h		; AnalogIOControl_0 register (ABF_CR0)
	db		67h, 33h		; AnalogLUTControl0 register (ALT_CR0)
	db		68h, 33h		; AnalogLUTControl1 register (ALT_CR1)
	db		63h, 00h		; AnalogModulatorControl_0 register (AMD_CR0)
	db		66h, 00h		; AnalogModulatorControl_1 register (AMD_CR1)
	db		d1h, 00h		; GlobalDigitalInterconnect_Drive_Even_Input register (GDI_E_IN)
	db		d3h, 00h		; GlobalDigitalInterconnect_Drive_Even_Output register (GDI_E_OU)
	db		d0h, 00h		; GlobalDigitalInterconnect_Drive_Odd_Input register (GDI_O_IN)
	db		d2h, 00h		; GlobalDigitalInterconnect_Drive_Odd_Output register (GDI_O_OU)
	db		e1h, 50h		; OscillatorControl_1 register (OSC_CR1)
	db		e2h, 00h		; OscillatorControl_2 register (OSC_CR2)
	db		dfh, 19h		; OscillatorControl_3 register (OSC_CR3)
	db		deh, 01h		; OscillatorControl_4 register (OSC_CR4)
	db		e3h, 87h		; VoltageMonitorControl register (VLT_CR)
;  Instance name Timer8_1, User Module Timer8
;       Instance name Timer8_1, Block Name TIMER8(DCB13)
	db		3ch, 20h		;Timer8_1_FUNC_REG(DCB13FN)
	db		3dh, 07h		;Timer8_1_INPUT_REG(DCB13IN)
	db		3eh, 40h		;Timer8_1_OUTPUT_REG(DCB13OU)
;  Instance name UART_1, User Module UART
;       Instance name UART_1, Block Name RX(DCB03)
	db		2ch, 05h		;UART_1_RX_FUNC_REG   (DCB03FN)
	db		2dh, d1h		;UART_1_RX_INPUT_REG  (DCB03IN)
	db		2eh, 00h		;UART_1_RX_OUTPUT_REG (DCB03OU)
;       Instance name UART_1, Block Name TX(DCB12)
	db		38h, 0dh		;UART_1_TX_FUNC_REG   (DCB12FN)
	db		39h, 01h		;UART_1_TX_INPUT_REG  (DCB12IN)
	db		3ah, 07h		;UART_1_TX_OUTPUT_REG (DCB12OU)
	db		ffh


; PSoC Configuration file trailer PsocConfig.asm
