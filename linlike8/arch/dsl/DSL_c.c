/*======================================================================================================*/
/* Project Name			:	DSL Controller HW independent coding													  */
/*	Modules Name			:	DSL_c.c																							  */
/*	Modules Description	:	DSL Lowest Level Core																		  */
/*	Compiler Used			:	Turbo C++ Ver3.0																				  */
/*	Person Resposible		:	Yan																								  */
/* Copyright (C) 2000, SC																										  */
/*======================================================================================================*/
/*																																		  */
/*	Revision History																												  */
/*																																		  */
/* Rev	Date			Who			Reason																					  */
/*	0.01	29-03-2000	yan			original																					  */
/*	0.02	03-04-2000	yan			for global compiler																	  */
/*	0.03	19-05-2000	yan			compare to pc_c.c																		  */
/*																																		  */
/*======================================================================================================*/

/********************************************************************************************************/
/* include files																													  */
/********************************************************************************************************/
#define Ctrller_C										/* define for Controller program									  */

#define   __B31__
#include "Main.h"										/* Main Application													  */
#include <dos.h>										/* for - void outportb(int portid, unsigned char value);	  */
#include <string.h>
#include <fcntl.h>									/* for open() and close()											  */
#include <stdio.h>
#include <io.h>
#include <time.h>										/*	tm - time structure												  */
															/* time_t - time variable											  */
															/* mktime() - convert the time structure to time variable  */
															/* stime() - set time												  */
															/*			int inport(int portid);									  */
#include <stdio.h>									/* fopen																	  */
#include <v55.h>
#include "Ctrller.h"									/* Controller Core module 											  */
#include "Uart.h"										/* Uart																	  */
#include "Fon.h"

/********************************************************************************************************/
/* Definition of Local Constant																								  */
/********************************************************************************************************/
		/* Port																														  */
				/* Port 1																											  */
#define INST0 94										/* Com1 Tx, vetcor num												  */
#define INSR0 92										/* Com1 Rx, vetcor num												  */
#define EN_TX 0xbf									/* Enable Tx															  */
#define TX_EMPTY 0x20								/* Tx buf is empty, ready for next Tx							  */
#define RX_COMPLETE 0x10							/* Rx buf is full, ready for read								  */
				/* Port 2																											  */
#define INST1 95
#define INSR1 93
		/* PIC 8259 setting																										  */
#define PIC8259EOI 0x20								/* PIC 8259 EOI														  */
#define PIC8259MASK 0x21							/* PIC 8259 MASK														  */
		/* byte format																												  */
#define NO_BREAK 0x00
#define NO_PARITY 0x00
#define TWO_STOP_BITS 0x04
#define ONE_STOP_BITS 0x00
#define EIGHT_BIT_DATA 0x03
		/* Timer																														  */
#define 	V55IC20		84								/* interrupt vector of 1/18.2 Sec timer interval			  */
#define TIME25MSEC 0xa2c2							/* 12.5MHz, running cnt., delay 25mSec							  */
#define v55_fint()    asm	DB  0Fh,92h
		/* CPU System																												  */
#define PARA "Para.txt"								/* Parameter file														  */
		/* Buffer																													  */
#define BUF1FULL 180									/* Buffer 1	full														  */
#define BUF2FULL 180									/* Buffer 2	full														  */
		/* LCD																														  */
#define  CONTROL     *v55_p5
#define  CS1_EN      (temp |= 0x04)  /* CS1 =  CONTROL BIT2 */
#define  INSTRUCTION (temp &= 0x06)
#define  CS1_DIS     (temp &= 0x03)  /* left side           */
#define  DATA_PORT   *v55_p4
#define  E_H         (temp |= 0x02)  /* E   = /CONTROL BIT1 */
#define  E_L         (temp &= 0x05)
#define  DATA        (temp |= 0x01)  /* D/I = /CONTROL BIT0 */
#define		RIGHT_POS	7
#define		RIGHT_POS_16	3

/********************************************************************************************************/
/* Definition of Local Variables																								  */
/********************************************************************************************************/
		/* DSL Controller register																								  */
typedef unsigned char __far *V55PO;
				/* Interrupt																										  */
V55PO v55_IMC = (V55PO) 0xFF000EC5;									/* IMC												  */
				/* timer																												  */
V55PO v55_tmc1  = (V55PO) 0xFF000F31;
V55PO v55_tm2   = (V55PO) 0xFF000F44;
V55PO v55_cm21  = (V55PO) 0xFF000F5A;
V55PO v55_ic20  = (V55PO) 0xFF000ED4;
		/* Com 1																														  */
V55PO v55_uasm0  = (V55PO) 0xFF000F73;
V55PO v55_uasm1 = (V55PO) 0xFF000F7B;
V55PO v55_pc3   = (V55PO) 0xff000F23;
V55PO v55_prs1   = (V55PO) 0xFF000F7A;
V55PO v55_rxbrg1 = (V55PO) 0xFF000F79;
V55PO v55_p0    = (V55PO) 0xFF000F00;
V55PO v55_pm0   = (V55PO) 0xFF000F10;
V55PO v55_txbrg0 = (V55PO) 0xFF000F70;
V55PO v55_rxbrg0 = (V55PO) 0xFF000F71;
V55PO v55_prs0   = (V55PO) 0xFF000F72;
V55PO v55_txbrg1 = (V55PO) 0xFF000F78;
V55PO v55_uast1 = (V55PO) 0xFF000F7C;
V55PO v55_txb1  = (V55PO) 0xFF000F7D;
V55PO v55_uast0 = (V55PO) 0xFF000F74;
V55PO v55_txb0  = (V55PO) 0xFF000F75;
V55PO v55_ic28  = (V55PO) 0xFF000EDC;
V55PO v55_ic30  = (V55PO) 0xFF000EDE;
V55PO v55_rxb0  = (V55PO) 0xFF000F76;
		/* Com 2																														  */
V55PO v55_ic29 = (V55PO) 0xFF000EDD;
V55PO v55_ic31 = (V55PO) 0xFF000EDF;
V55PO v55_rxb1  = (V55PO) 0xFF000F7E;
V55PO v55_mk1l  = (V55PO) 0xFF000EC2;
		/* 1/18.2 timer interrupt																								  */
void interrupt (*OldTimer55mSecISR)(void);						/* original 1/18.2 timer ISR					  */
unsigned char ErrTuningCnt;
unsigned char Timer50Cnt;
unsigned char Timer1Cnt;
unsigned char Timer1Flag;
unsigned char Timer2Cnt;
unsigned char Timer2Flag;
unsigned char Timer3Cnt;
unsigned char Timer3Flag;
unsigned char Timer4Cnt;
unsigned char Timer4Flag;
unsigned char Timer5Cnt;
unsigned char Timer5Flag;
		/* pins Control																											  */
V55PO v55_p7 = (V55PO) 0xff000F07;
V55PO v55_pm7 = (V55PO) 0xFF000F17;
V55PO v55_pmc7 = (V55PO) 0xFF000F27;
		/* LCD																														  */
V55PO v55_pm4 = (V55PO) 0xFF000F14;
V55PO v55_pm5 = (V55PO) 0xFF000F15;
V55PO v55_pmc4 = (V55PO) 0xFF000F24;
V55PO v55_pmc5 = (V55PO) 0xFF000F25;
V55PO v55_p5 = (V55PO) 0xff000F05;
V55PO v55_p4 = (V55PO) 0xff000F04;
char    temp;
int     Ctrl_i,j;

/********************************************************************************************************/
/* Definition of Local Functions																								  */
/********************************************************************************************************/
		/* Uart																														  */
				/* Com1																												  */
void interrupt Com1TxISR(void);
void interrupt Com1RxISR(void);
				/* Com2																												  */
void interrupt Com2TxISR(void);
void interrupt Com2RxISR(void);
		/* Timer																														  */
void interrupt Timer55mSecISR(void);
		/* LCD																														  */
void PUT_WORD8(int x,int y,char *cWordType,int word_number);
void WRITE(unsigned char i);
void BLANK();
void LcdPrintf(int iX,int iY,BYTE *pbDat,BYTE bLen,BYTE bFonSize);
void BlankData();
void	PUT_WORD(int x,int y,char *cWordType,int word_number);
void LcdPrintNumber16(int iX,int iY,BYTE *pbDat,BYTE bLen,BYTE bDotPos);
void LcdPrintNumber(int iX,int iY,unsigned char *pbDat,unsigned char bLen,unsigned char bDotPos);

/********************************************************************************************************/
/*		Core System																													  */
/********************************************************************************************************/
/********************************************************************************************************/

/********************************************************************************************************/
/* Sub Name				:	CtrllerInit																							  */
/*	Sub Description	:	Init																									  */
/********************************************************************************************************/
void CtrllerInit(void)
{
	*v55_txbrg1 = *v55_txbrg0;                     /* copy contents */
	*v55_rxbrg1 = *v55_rxbrg0;                     /* of channel 0 */
	*v55_prs1   = *v55_prs0;                       /* registers to */
	//*v55_uast1  = *v55_uast0;                      /* channel 1 */
	*v55_uasm1  = 0xce;                      /* registers */
	*v55_uasm0 |= 0xce;                            /* rx & tx enabled */
	/* Enable RS232 on channel 1 */
	*v55_pc3 &= 0xbf;
	*v55_pm0    = 0x00;                            /* output mode */
	*v55_p0    |= 0x0C;
				/* set baud rate										*/
 *v55_txbrg1 = 162;                     /* copy contents */
 *v55_rxbrg1 = 162;                     /* of channel 0 */
 *v55_prs1   = 0;
		/* Com 1																														  */
 setvect(INST0,Com1TxISR);										/* set interrupt											  */
 setvect(INSR0,Com1RxISR);										/* set interrupt											  */
 *v55_ic30=0x43;													/* Tx int. req. ctrl reg.								  */
 *v55_ic28=0x3;													/* Rx int. req. ctrl reg.								  */
		/* Com 2																														  */
 setvect(INST1,Com2TxISR);										/* set interrupt											  */
 setvect(INSR1,Com2RxISR);										/* set interrupt											  */
 *v55_ic31=0x43;													/* Tx int. req. ctrl reg.								  */
 *v55_ic29=0x3;													/* Rx int. req. ctrl reg.								  */
 *v55_mk1l &= 0x7f;												/* Interrupt mask of Com 2								  */

		/* 1/18.2 Timer interrupt service subroutine																		  */
				/* init. timer 0																									  */
 *v55_tmc1=0x09;													/* Timer control registers, CE-enable, PRM-/32	  */
 *v55_tm2=0;														/* Timer 2 register										  */
 *(v55_tm2+1)=0;
 *v55_cm21=0xfd;													/* any err. in timing control, i don't know		  */
 *(v55_cm21+1)=0xff;												
 ErrTuningCnt = 0;
/* *v55_cm21=0x4b;													/* timer 2 compare reg.									  */
/* *(v55_cm21+1)=0x4c;												/* about 50mSec											  */
 setvect(V55IC20,Timer55mSecISR);										/* set interrupt											  */
 *v55_ic20=0x3;													/* int. req. ctrl reg.									  */
				/* prepare Timer1, Timer2 ...																					  */
 Timer1Flag = FALSE;
 Timer2Flag = FALSE;
 Timer3Flag = FALSE;
 Timer4Flag = FALSE;
 Timer5Flag = FALSE;

		/* Port Setting																												  */
 *v55_pmc7=0;						// Setup P7 as input and output port 
 *v55_pm7=0xe0;						// P7.7 to P7.0 as IIIO OOOO
 *v55_p7=0xe0;							// initialize P7.7 to P7.0 as 1110 0OOO

		/* set screen, clear it																									  */
  *v55_pmc4=0;
  *v55_pmc5=0;
  *v55_pm4=0;
  *v55_pm5=0;
 temp = 0x00;                       /* CS2-CS1-E-DI = 0 0 0 0 */
 CONTROL=temp;
 //CONTROL,CS2_EN;          /* right side enable        */
 CONTROL=CS1_EN;          /* left side enable         */
 CONTROL=INSTRUCTION;     /* D/I = 0 , Instruction    */
 WRITE(0x3F);		            /* display on , bit0 = 1    */
 BLANK();
 CONTROL=CS1_DIS;          /* left side enable         */
 CONTROL=INSTRUCTION;     /* D/I = 0 , Instruction    */
 WRITE(0x3F);		            /* display on , bit0 = 1    */

 BLANK();                           /* clear display data RAM   */
}

/********************************************************************************************************/
/* Sub Name				: ExitProgram																							  */
/*	Sub Description	: method to Exit this program		 																  */
/* Return				: TRUE -- Exit, FALSE -- Continue					 											  */
/********************************************************************************************************/
unsigned char ExitProgram(void)
{
 if (kbhit()) if (getch()=='A') return TRUE;
 return FALSE;
}

/********************************************************************************************************/
/* Sub Name				:	CtrllerClose																						  */
/*	Sub Description	:	Close and restore the old setting															  */
/********************************************************************************************************/
void CtrllerClose(void)
{
				/* 1/18.2 Timer interrupt service subroutine												*/
// setvect(TIMER55MSEC, OldTimer55mSecISR);  					
}

/********************************************************************************************************/
/*		Uart1 System																													  */
/********************************************************************************************************/
/********************************************************************************************************/

/********************************************************************************************************/
/* Sub Name				:	Com1TxISR																							  */
/*	Sub Description	:	ISR of Port 1, layer 1																			  */
/********************************************************************************************************/
void interrupt Com1TxISR(void)
{
 if (*v55_uast0&TX_EMPTY) {
	 if (pRdUart1TxBuf==pWrUart1TxBuf) *v55_ic30 |= ~EN_TX;
	 else {
		 *v55_txb0 = Uart1TxBuf[pRdUart1TxBuf];
		 pRdUart1TxBuf++;												/* update read ptr of Tx buf							  */
		 if (pRdUart1TxBuf>=MAX_UART_TXRX_BUF) pRdUart1TxBuf = 0;
		}
	}

		/* end of interrupt signal																								  */										
 v55_fint();                               
}

void interrupt Com1RxISR(void)
{
 unsigned char aTemp;
 unsigned int pWrUart1RxBufBackup;
 if (*v55_uast0&RX_COMPLETE) {
	 pWrUart1RxBufBackup = pWrUart1RxBuf;					/* inc. pointer											  */
	 pWrUart1RxBufBackup++;
	 if (pWrUart1RxBufBackup>=MAX_UART_TXRX_BUF) pWrUart1RxBufBackup = 0;
	 if (pWrUart1RxBufBackup!=pRdUart1RxBuf) {			// buf not ful 
		 Uart1RxBuf[pWrUart1RxBuf] = *v55_rxb0;			/* Rx one byte												  */
		 pWrUart1RxBuf = pWrUart1RxBufBackup;
 		}
	 else aTemp = *v55_rxb0;										// buf full return error, 
	}

		/* end of interrupt signal																								  */										
 v55_fint();                               
}

/********************************************************************************************************/
/* Sub Name				:	EnTxCom1																								  */
/*	Sub Description	:	Enable Tx Com2																						  */
/********************************************************************************************************/
unsigned char EnTxCom1(void)
{
 *v55_txb0 = Uart1TxBuf[pRdUart1TxBuf];		/* read one byte to transmit out	*/
 pRdUart1TxBuf++;												/* update read ptr of Tx buf							  */
 if (pRdUart1TxBuf>=MAX_UART_TXRX_BUF) pRdUart1TxBuf = 0;
 *v55_ic30 &= EN_TX;													/* int. req. ctrl reg.									  */
 return(TRUE);
}

/********************************************************************************************************/
/*		Uart2 System																													  */
/********************************************************************************************************/
/********************************************************************************************************/

/********************************************************************************************************/
/* Sub Name				:	Com2TxISR																							  */
/*	Sub Description	:	ISR of Port 2, layer 1																			  */
/********************************************************************************************************/
void interrupt Com2TxISR(void)
{
 if (*v55_uast1&TX_EMPTY) {
	 if (pRdUart2TxBuf==pWrUart2TxBuf) *v55_ic31 |= ~EN_TX;
	 else {
		 *v55_txb1 = Uart2TxBuf[pRdUart2TxBuf];
		 pRdUart2TxBuf++;												/* update read ptr of Tx buf							  */
		 if (pRdUart2TxBuf>=MAX_UART_TXRX_BUF) pRdUart2TxBuf = 0;
		}
	}

		/* end of interrupt signal																								  */										
 v55_fint();                               
}

void interrupt Com2RxISR(void)
{
 unsigned char aTemp;
 unsigned int pWrUart2RxBufBackup;
 if (*v55_uast1&RX_COMPLETE) {
	 pWrUart2RxBufBackup = pWrUart2RxBuf;					/* inc. pointer											  */
	 pWrUart2RxBufBackup++;
	 if (pWrUart2RxBufBackup>=MAX_UART_TXRX_BUF) pWrUart2RxBufBackup = 0;
	 if (pWrUart2RxBufBackup!=pRdUart2RxBuf) {			// buf not ful 
		 Uart2RxBuf[pWrUart2RxBuf] = *v55_rxb1;			/* Rx one byte												  */
		 pWrUart2RxBuf = pWrUart2RxBufBackup;
 		}
	 else aTemp = *v55_rxb1;										// buf full return error, 
	}

		/* end of interrupt signal																								  */										
 v55_fint();                               
}

/********************************************************************************************************/
/* Sub Name				:	EnTxUart2																							  */
/*	Sub Description	:	Enable Tx Com2																						  */
/********************************************************************************************************/
unsigned char EnTxCom2(void)
{
 *v55_txb1 = Uart2TxBuf[pRdUart2TxBuf];		/* read one byte to transmit out	*/
 pRdUart2TxBuf++;												/* update read ptr of Tx buf							  */
 if (pRdUart2TxBuf>=MAX_UART_TXRX_BUF) pRdUart2TxBuf = 0;
 *v55_ic31 &= EN_TX;													/* int. req. ctrl reg.								  */
 return(TRUE);
}

/********************************************************************************************************/
/*		Beeper and LEDs System																									  */
/********************************************************************************************************/
/********************************************************************************************************/

/********************************************************************************************************/
/* Sub Name				:	OutputDeviceON																						  */
/*	Sub Description	:	Output Device ON																					  */
/********************************************************************************************************/
void OutputDeviceON(unsigned char which)
{
 if (which&BEEPER) {
	 sound(1000);
	 printf("Buzzer ON\n");
	 *v55_p7 = *v55_p7 | 0x01;
	}
 if (which&GREENLED) {
	 printf("Green LED ON\n");
	 *v55_p7 = *v55_p7 | 0x02;
	}
 if (which&REDLED) {
	 printf("Red LED ON\n");
	 *v55_p7 = *v55_p7 | 0x04;
	}
 if (which&RELAY0) {
	 printf("Relay0 ON\n");
	}
 if (which&RELAY1) {
	 printf("Relay1 ON\n");
	}
}

/********************************************************************************************************/
/* Sub Name				:	OutputDeviceOFF																					  */
/*	Sub Description	:	Output Device OFF																					  */
/********************************************************************************************************/
void OutputDeviceOFF(unsigned char which)
{
 if (which&BEEPER) {
	 nosound();
	 printf("Buzzer OFF\n");
	 *v55_p7 = *v55_p7 & ~0x01;
	}
 if (which&GREENLED) {
	 printf("Green LED OFF\n");
	 *v55_p7 = *v55_p7 & ~0x02;
	}
 if (which&REDLED) {
	 printf("Red LED Off\n");
	 *v55_p7 = *v55_p7 & ~0x04;
	}
 if (which&RELAY0) {
	}
 if (which&RELAY1) {
	}
}

/********************************************************************************************************/
/* Sub Name				:	InputDevice																							  */
/*	Sub Description	:	status of Input Device																			  */
/*								sensor1 => keybaord input 'A'																	  */
/*								sensor2 => keybaord input 'L'																	  */
/********************************************************************************************************/
unsigned char InputDevice(unsigned char which)
{
 if (kbhit()) {
	 if (which&SENSOR0) if (getch()=='A') return TRUE;
	 if (which&SENSOR1) if (getch()=='L') return TRUE;
	}
 return FALSE;
}

/********************************************************************************************************/
/*		Real Time Clock System																									  */
/********************************************************************************************************/
/********************************************************************************************************/

/********************************************************************************************************/
/* Name			: SetCurTimeDate																								  */
/*	Description	: set the current time and date						 													  */
/*	Parameters	: pData_TimeDate -- ptr as								 													  */
/*						position 0 as int tm_sec;      -- Seconds						 									  */
/*						position 1 as int tm_min;      -- Minutes						 									  */
/*						position 2 as int tm_hour;     -- Hour (0--23)						 							  */
/*						position 3 as int tm_mday;     -- Day of month (1--31)										  */
/*						position 4 as int tm_mon;      -- Month (0--11)						 							  */
/*						position 5 as int tm_year;     -- Year (calendar year minus 1900)							  */
/*						position 6 as int tm_wday;     -- Weekday (0--6; Sunday = 0)								  */
/********************************************************************************************************/
void SetCurTimeDate(unsigned int* pData_TimeDate)
{
 struct tm CurDateTime;
 time_t t;
 
 CurDateTime.tm_sec = *(pData_TimeDate+0);
 CurDateTime.tm_min = *(pData_TimeDate+1);
 CurDateTime.tm_hour = *(pData_TimeDate+2);
 CurDateTime.tm_mday = *(pData_TimeDate+3);
 CurDateTime.tm_mon = *(pData_TimeDate+4)-1;
 CurDateTime.tm_year = *(pData_TimeDate+5)-1900;
 CurDateTime.tm_wday = *(pData_TimeDate+6);
 t = mktime(&CurDateTime);
 stime(&t);
}

/********************************************************************************************************/
/* Name			: GetCurTimeDate																								  */
/*	Description	: get the current time and date						 													  */
/*	Parameters	: CurTimeDateBuf -- ptr as								 													  */
/*						position 0 as int tm_sec;      -- Seconds						 									  */
/*						position 1 as int tm_min;      -- Minutes						 									  */
/*						position 2 as int tm_hour;     -- Hour (0--23)						 							  */
/*						position 3 as int tm_mday;     -- Day of month (1--31)										  */
/*						position 4 as int tm_mon;      -- Month (1--12)						 							  */
/*						position 5 as int tm_year;     -- Year 															  */
/*						position 6 as int tm_wday;     -- Weekday (0--6; Sunday = 0)								  */
/********************************************************************************************************/
void GetCurTimeDate(unsigned int* CurTimeDateBuf)
{
 time_t t;
 struct tm *tblock;

		/* gets time of day								*/
 t = time(NULL);
		/* converts date/time to a structure		*/
 tblock = localtime(&t);
 *(CurTimeDateBuf+0) = tblock->tm_sec;
 *(CurTimeDateBuf+1) = tblock->tm_min;
 *(CurTimeDateBuf+2) = tblock->tm_hour;
 *(CurTimeDateBuf+3) = tblock->tm_mday;
 *(CurTimeDateBuf+4) = tblock->tm_mon+1; 
 *(CurTimeDateBuf+5) = tblock->tm_year+1900;
 *(CurTimeDateBuf+6) = tblock->tm_wday;
}

/********************************************************************************************************/
/*		Timer System																													  */
/********************************************************************************************************/
/********************************************************************************************************/

/********************************************************************************************************/
/* Name			: Timer55mSecISR																								  */
/*	Description	: about 50mSec to interrupt for once				 													  */
/********************************************************************************************************/
void interrupt Timer55mSecISR(void)
{
 *v55_ic20|=0x40;
 *v55_ic20&=0xbf;

		/* any err. in timing control, i don't know		  */
 ErrTuningCnt++;
 if (ErrTuningCnt>=76) {
	 ErrTuningCnt = 0;
	 Timer50Cnt++;
 	}
 if (Timer50Cnt==Timer1Cnt) {
		/* do Time1 ISR for operating system timer					 													  */

	}
 if (Timer50Cnt==Timer2Cnt) {
		/* do Time2 ISR														 													  */
	 Timer2Flag = FALSE;
	}
 if (Timer50Cnt==Timer3Cnt) {
		/* do Time3 ISR														 													  */
	 Timer3Flag = FALSE;
	}
 if (Timer50Cnt==Timer4Cnt) {
		/* do Time4 ISR														 													  */
	 Timer4Flag = FALSE;
	}
 if (Timer50Cnt==Timer5Cnt) {
		/* do Time5 ISR														 													  */
	 Timer5Flag = FALSE;
	}

		// call the original ISR
// OldTimer55mSecISR();
	v55_fint();                               // end of interrupt signal
}

/********************************************************************************************************/
/* Name			: Timer1Act																										  */
/*	Description	: Active Timer1 																								  */
/*					  time cal. is the times of 55mSec 																		  */
/* Parameter	: unsigned char TimeCal - length of timer																  */
/* Return		: TRUE - start OK; FALSE - it is busy, it cannot serve you		 								  */
/********************************************************************************************************/
unsigned char Timer1Act(unsigned char TimeCal)
{
 if (Timer1Flag) return FALSE;				// it is busy
 else {
	 Timer1Cnt = Timer50Cnt + TimeCal;
	 Timer1Flag = TRUE;
	 return TRUE;
	}
}

/********************************************************************************************************/
/* Name			: Timer1Sta																										  */
/*	Description	: status of Timer1										 													  */
/* Return		: FALSE - idle, you can use it						 													  */
/*					  TRUE - busy												 													  */
/********************************************************************************************************/
unsigned char Timer1Sta(void)
{
 return Timer1Flag;
}

/********************************************************************************************************/
/* Name			: Timer1Clo																										  */
/*	Description	: Close System Timer 																						  */
/* Return		: 10															 													  */
/********************************************************************************************************/
unsigned int Timer1Clo(void)
{
 Timer1Flag = FALSE;
 return (10);
}

/********************************************************************************************************/
/* Name			: Timer2Act																										  */
/*	Description	: Active Timer2 																								  */
/*					  time cal. is the times of 55mSec 																		  */
/* Parameter	: unsigned char TimeCal - length of timer																  */
/* Return		: TRUE - start OK; FALSE - it is busy, it cannot serve you		 								  */
/********************************************************************************************************/
unsigned char Timer2Act(unsigned char TimeCal)
{
 if (Timer2Flag) return FALSE;				// it is busy
 else {
	 Timer2Cnt = Timer50Cnt + TimeCal;
	 Timer2Flag = TRUE;
	 return TRUE;
 	}
}

/********************************************************************************************************/
/* Name			: Timer2Sta																										  */
/*	Description	: status of Timer2										 													  */
/* Return		: FALSE - idle, you can use it						 													  */
/*					  TRUE - busy												 													  */
/********************************************************************************************************/
unsigned char Timer2Sta(void)
{
 return Timer2Flag;
}

/********************************************************************************************************/
/* Name			: Timer2Clo																										  */
/*	Description	: Close System Timer 																						  */
/* Return		: 10															 													  */
/********************************************************************************************************/
unsigned int Timer2Clo(void)
{
 Timer2Flag = FALSE;
 return (10);
}

/********************************************************************************************************/
/* Name			: Timer3Act																										  */
/*	Description	: Active Timer3 																								  */
/*					  time cal. is the times of 55mSec 																		  */
/* Parameter	: unsigned char TimeCal - length of timer																  */
/* Return		: TRUE - start OK; FALSE - it is busy, it cannot serve you		 								  */
/********************************************************************************************************/
unsigned char Timer3Act(unsigned char TimeCal)
{
 if (Timer3Flag) return FALSE;				// it is busy
 else {
	 Timer3Cnt = Timer50Cnt + TimeCal;
	 Timer3Flag = TRUE;
	 return TRUE;
 	}
}

/********************************************************************************************************/
/* Name			: Timer3Sta																										  */
/*	Description	: status of Timer3										 													  */
/* Return		: FALSE - idle, you can use it						 													  */
/*					  TRUE - busy												 													  */
/********************************************************************************************************/
unsigned char Timer3Sta(void)
{
 return Timer3Flag;
}

/********************************************************************************************************/
/* Name			: Timer3Clo																										  */
/*	Description	: Close System Timer 																						  */
/* Return		: 10															 													  */
/********************************************************************************************************/
unsigned int Timer3Clo(void)
{
 Timer3Flag = FALSE;
 return (10);
}

/********************************************************************************************************/
/* Name			: Timer4Act																										  */
/*	Description	: Active Timer4 																								  */
/*					  time cal. is the times of 55mSec 																		  */
/* Parameter	: unsigned char TimeCal - length of timer																  */
/* Return		: TRUE - start OK; FALSE - it is busy, it cannot serve you		 								  */
/********************************************************************************************************/
unsigned char Timer4Act(unsigned char TimeCal)
{
 if (Timer4Flag) return FALSE;				// it is busy
 else {
	 Timer4Cnt = Timer50Cnt + TimeCal;
	 Timer4Flag = TRUE;
	 return TRUE;
 	}
}

/********************************************************************************************************/
/* Name			: Timer4Sta																										  */
/*	Description	: status of Timer4										 													  */
/* Return		: FALSE - idle, you can use it						 													  */
/*					  TRUE - busy												 													  */
/********************************************************************************************************/
unsigned char Timer4Sta(void)
{
 return Timer4Flag;
}

/********************************************************************************************************/
/* Name			: Timer4Clo																										  */
/*	Description	: Close System Timer 																						  */
/* Return		: 10															 													  */
/********************************************************************************************************/
unsigned int Timer4Clo(void)
{
 Timer4Flag = FALSE;
 return (10);
}

/********************************************************************************************************/
/* Name			: Timer5Act																										  */
/*	Description	: Active Timer5 																								  */
/*					  time cal. is the times of 55mSec 																		  */
/* Parameter	: unsigned char TimeCal - length of timer																  */
/* Return		: TRUE - start OK; FALSE - it is busy, it cannot serve you		 								  */
/********************************************************************************************************/
unsigned char Timer5Act(unsigned char TimeCal)
{
 if (Timer5Flag) return FALSE;				// it is busy
 else {
	 Timer5Cnt = Timer50Cnt + TimeCal;
	 Timer5Flag = TRUE;
	 return TRUE;
 	}
}

/********************************************************************************************************/
/* Name			: Timer5Sta																										  */
/*	Description	: status of Timer5										 													  */
/* Return		: FALSE - idle, you can use it						 													  */
/*					  TRUE - busy												 													  */
/********************************************************************************************************/
unsigned char Timer5Sta(void)
{
 return Timer5Flag;
}

/********************************************************************************************************/
/* Name			: Timer5Clo																										  */
/*	Description	: Close System Timer 																						  */
/* Return		: 10															 													  */
/********************************************************************************************************/
unsigned int Timer5Clo(void)
{
 Timer5Flag = FALSE;
 return (10);
}

/********************************************************************************************************/
/*		CPU System																													  */
/********************************************************************************************************/
/********************************************************************************************************/

/********************************************************************************************************/
/* Name			: EraseEeprom																									  */
/*	Description	: Erase Parameters							 			 													  */
/* Parameter	: none																											  */
/* Return		: TRUE -- deleted											 													  */
/*					  FALSE -- error in deleted							 													  */
/********************************************************************************************************/
unsigned char EraseEeprom(void)
{
 gotoxy(1,1);
 if (remove(PARA)==0) return TRUE;
 return FALSE;
}

/********************************************************************************************************/
/* Name			: WrPara																											  */
/*	Description	: Write Parameters							 			 													  */
/* Parameter	: unsigned char* Buf - Buf Ptr																			  */
/*					  unsigned int LenBuf - Length for writing															  */
/*					  unsigned char Cnt - Cnt 0 -- 1st 64 bytes, Cnt 1 -- 2nd 64 bytes ...						  */
/* Return		: FALSE 														 													  */
/*					  TRUE 														 													  */
/********************************************************************************************************/
int WrPara(unsigned char* Buf, unsigned int LenBuf, unsigned char Cnt)
{
 FILE *change;

 printf("write Parameters %d\n",Cnt);

 change = fopen(PARA, "a+b");
 fwrite(Buf, LenBuf, 1, change);			/* no need to seek, since always append to write					  */

 fclose(change);
 return(TRUE);
}

/********************************************************************************************************/
/* Name			: GetPara																										  */
/*	Description	: Read Parameter from Eeprom	 						 													  */
/* Parameter	: Parameters ptr																								  */
/*						512 position(0~511) -- Bit size																		  */
/*						64 position(512~575) -- Byte size																	  */
/*						64 position(576~639) -- Word size																	  */
/*						32 position(640~671) -- 8-byte size																	  */
/* Return		: return a base pointer									 													  */
/********************************************************************************************************/
unsigned char* GetPara(unsigned int pPara)
{
 FILE *change;
 unsigned int ParaLocation;
 unsigned char ParaSize;
 static unsigned char ResultByte[8];

		// Cal. the location 
 if (pPara<512) {
	 ParaLocation = pPara/8;
	 ParaSize = 1;																	// bit or byte size
 	}
 else if (pPara<576) ParaLocation = pPara - 512;
 else if (pPara<640) ParaLocation = pPara - 576;
 else ParaLocation = pPara - 576;
		// open parameter file
 change = fopen("Para.txt", "rb");
		// seek to that location 
 fseek(change, ParaLocation, SEEK_SET);
		// read parameter 
 fread(ResultByte, ParaSize, 1, change);
		// ive the result
 if (pPara<512) ResultByte[0] &= (0x01 << pPara%8);

 fclose(change);
 return(ResultByte);
}

/********************************************************************************************************/
/*		Buffer																														  */
/********************************************************************************************************/
/********************************************************************************************************/

/********************************************************************************************************/
/* Name			: WrBuf1																											  */
/*	Description	: Write Buffer1					 						 													  */
/* Parameter	: pData -- ptr of data for save 																			  */
/*					  Len -- Len of data for save, max. size is 0xffff in once										  */
/* Return		: TRUE -- OK, FALSE -- FULL									 											  */
/* Remarks		: at this stage, we only prepare non-cycling buffer												  */
/********************************************************************************************************/
unsigned char WrBuf1(unsigned char* pData, unsigned int Len)
{
 int FileLenHandle;
 FILE *change;

		/* if full, return error																								  */
 FileLenHandle = open(BUF1, O_CREAT);
		/* if full, return error																								  */
 if (filelength(FileLenHandle)>=BUF1FULL) return FALSE;
 close(FileLenHandle);

 if ((change = fopen(BUF1, "a+b"))==NULL) return FALSE;
						/* no need to seek, since always append to write					  */
 fwrite(pData, Len, 1, change);
 fclose(change);

 return(TRUE);
}

/********************************************************************************************************/
/* Name			: RdBuf1																											  */
/*	Description	: Read Buffer1						 						 													  */
/* Parameter	: pData -- ptr of data for save 																			  */
/*					  Len -- Len of data for save, max. size is 0xffff in once										  */
/*					  Buf1Ptr -- Buffer 1 current pointer																	  */
/* Return		: FALSE -- it is err. 											 											  */
/*					  OK -- return the len of successful data																  */
/* Remarks		: -- at this stage, we only prepare non-cycling buffer											  */
/*					  -- the max. of buf len is 64K																			  */
/********************************************************************************************************/
unsigned char RdBuf1(unsigned char* pData, unsigned int Len, unsigned int Buf1Ptr)
{
 FILE *change;
 static unsigned int BufThisLen=0;

		/* Open file																												  */
 if ((change = fopen(BUF1, "r+b"))==NULL) return FALSE;
		/* Seek to pointer location																							  */
 fseek(change, Buf1Ptr, SEEK_SET);
		/* Read a block of data 																								  */
 do {
	 pData[BufThisLen] = fgetc(change);
	 BufThisLen++;
	} while ((BufThisLen<Len)&&(pData[BufThisLen-1] != (unsigned char) EOF));
		/* Update pointer			 																								  */
 Buf1Ptr += Len;
 fclose(change);

 return(BufThisLen);
}

/********************************************************************************************************/
/* Name			: WrBuf2																											  */
/*	Description	: Write Buffer2					 						 													  */
/* Parameter	: pData -- ptr of data for save 																			  */
/*					  Len -- Len of data for save, max. size is 0xffff in once										  */
/* Return		: TRUE -- OK, FALSE -- FULL									 											  */
/* Remarks		: at this stage, we only prepare non-cycling buffer												  */
/********************************************************************************************************/
unsigned char WrBuf2(unsigned char* pData, unsigned int Len)
{
 int FileLenHandle;
 FILE *change;

		/* if full, return error																								  */
 FileLenHandle = open(BUF2, O_CREAT);
 printf("file length in bytes: %ld\n", filelength(FileLenHandle));
		/* if full, return error																								  */
 if (filelength(FileLenHandle)>=BUF2FULL) return FALSE;
 close(FileLenHandle);

 change = fopen(BUF2, "a+b");
						/* no need to seek, since always append to write					  */
 fwrite(pData, Len, 1, change);
 fclose(change);

 return(TRUE);
}

/********************************************************************************************************/
/* Name			: RdBuf2																											  */
/*	Description	: Read Buffer2						 						 													  */
/* Parameter	: pData -- ptr of data for save 																			  */
/*					  Len -- Len of data for save, max. size is 0xffff in once										  */
/* Return		: TRUE -- OK, FALSE -- FULL									 											  */
/* Remarks		: at this stage, we only prepare non-cycling buffer												  */
/********************************************************************************************************/
unsigned char RdBuf2(unsigned char* pData, unsigned int Len)
{
 FILE *change;
 
 if ((change = fopen(BUF2, "a+b"))==NULL) return FALSE;
 fread(pData, Len, 1, change);
 fclose(change);

 return(TRUE);
}

/********************************************************************************************************/
/* Name			: BufLen																											  */
/*	Description	: Cal Buffer length in block, where each block is 4 txlogs										  */
/* Parameter	: BufName -- Buf Name 			 																			  */
/* Return		: Len of Buf			 											 											  */
/********************************************************************************************************/
unsigned int BufLen(const char* BufName)
{
 int FileLenHandle;
 unsigned int FileLen;

		/* if full, return error																								  */
 FileLenHandle = open(BufName, O_CREAT);
		/* if full, return error																								  */
 FileLen = filelength(FileLenHandle)/(4*18);
 if ((filelength(FileLenHandle)%(4*18))!=0) FileLen++;
 close(FileLenHandle);
 return FileLen;
}

/********************************************************************************************************/
/*		LCD display																													  */
/********************************************************************************************************/
/********************************************************************************************************/

/********************************************************************************************************/
/* Name			: ClkDisp																										  */
/*	Description	: Clock Display						 																		  */
/* Parameter	: CurTimeDateBuf -- ptr of Current Time Date Buf													  */
/* Return		: none																 											  */
/********************************************************************************************************/
void ClkDisp(unsigned int* CurTimeDateBuf)
{
  int cx;
    CONTROL=temp;
    CONTROL=CS1_EN;          /* left side enable         */
    cx=(CurTimeDateBuf[5]-1900)/10;
	 if (cx>=10) cx%=10;
    PUT_WORD8(0,48,scAscii8,cx+16);
    PUT_WORD8(0,56,scAscii8,(((CurTimeDateBuf[5]-1900) %10) ) +16);
    PUT_WORD8(0,16,scAscii8,'-'-' ');
    PUT_WORD8(0,24,scAscii8,CurTimeDateBuf[4]/10 +16);
    PUT_WORD8(0,32,scAscii8,CurTimeDateBuf[4]%10 +16);
    PUT_WORD8(0,40,scAscii8,'-'-' ');
    PUT_WORD8(0,0,scAscii8,CurTimeDateBuf[3]/10 +16);
    PUT_WORD8(0,8,scAscii8,CurTimeDateBuf[3]%10 +16);

    CONTROL=CS1_DIS;
    PUT_WORD8(0,16,scAscii8,CurTimeDateBuf[2]/10 +16);
    PUT_WORD8(0,24,scAscii8,CurTimeDateBuf[2]%10 +16);
    PUT_WORD8(0,32,scAscii8,':'-' ');

    PUT_WORD8(0,40,scAscii8,CurTimeDateBuf[1]/10 +16);

    PUT_WORD8(0,48,scAscii8,CurTimeDateBuf[1]%10 +16);

// printf("\n%d/%d/%d  %d:%d:%d",CurTimeDateBuf[3],CurTimeDateBuf[4],CurTimeDateBuf[5],CurTimeDateBuf[2],CurTimeDateBuf[1],CurTimeDateBuf[0]);
}

void PUT_WORD8(int x,int y,char *cWordType,int word_number)
{
 unsigned int i;
 for(i = 0;i < 2;i++) {
	 CONTROL=INSTRUCTION;
	 WRITE(0x40 + y);
	 WRITE(0xb8 + x+i);
	 for(j = word_number * 16+i*8;j < (word_number * 16 +(i+1 ) * 8);j ++) {
		 CONTROL=DATA;
		 WRITE(*(cWordType + j ));
		}
	}
}

void BLANK()
 {
 WRITE(0xc0);                        /* display start line       */
 WRITE(0x40);                        /* set y address , y = 0    */
 for(Ctrl_i = 0;Ctrl_i < 8;Ctrl_i++)                /* clear graphic LCD        */
    { CONTROL=INSTRUCTION; /* D/I = 0 , Instruction    */
      WRITE(0xb8 + Ctrl_i);               /* set page (x address)     */
      for(j = 0;j < 64;j++)
	 { CONTROL=DATA;   /* D/I = 1 , send data      */
	   WRITE(0x00);
	 }
    }
 }

void WRITE(unsigned char i)
 {
 DATA_PORT=i;
 CONTROL=E_H;
 CONTROL=E_L;
 //delay(5);
 }

void LcdPrintf(int iX,int iY,unsigned char *pbDat,unsigned char bLen,unsigned char bFonSize)
{
 int iCn,i,j;
 int iTempX,iTempY,iOffset;
 BYTE bSide;

   iOffset=0;
   if (iY>=RIGHT_POS){
	bSide=FALSE;
	iTempX=iX;

	iTempY=(iY-8)*8;
	CONTROL=CS1_DIS;
	}
   else {
	bSide=TRUE;
	iTempX=iX;
	iTempY=iY*8;
	CONTROL=CS1_EN;
       }
   for (iCn=0;iCn<bLen;iCn++){
      for(i = 0;i < 2;i++) {
	CONTROL=INSTRUCTION;
	//printf("\nY : %02x",0x40 + iTempY+iCn*bFonSize-iOffset);
	//printf("\nX : %02x", 0xb8 + iTempX+i);
	WRITE(0x40 + iTempY+iCn*bFonSize-iOffset);
	WRITE(0xb8 + iTempX+i);
	for(j = iCn *bFonSize*2 +i*bFonSize;j < (iCn * bFonSize*2 +(i+1 ) * bFonSize);j ++)
	   {
	   CONTROL=DATA;
	   WRITE(*(pbDat + j ));
	   }
	}
      if( ((iY+iCn*2+1)>=RIGHT_POS) && (bSide==TRUE)){
	iTempX=iX;
	bSide=FALSE;
	iOffset=(iTempY+(iCn+1)*bFonSize);
	//printf("\n Chage side %02x",iOffset);
	CONTROL=CS1_DIS;
		}
	}
}

 void BlankData()
 {
 unsigned int i;

 CONTROL=CS1_EN;
 WRITE(0xc0);                        /* display start line       */
 WRITE(0x40);                        /* set y address , y = 0    */
 for(i = 2;i < 8;i++)                /* clear graphic LCD        */
    { CONTROL=INSTRUCTION; /* D/I = 0 , Instruction    */
      WRITE(0xb8 + i);               /* set page (x address)     */
      for(j = 0;j < 64;j++)
	 { CONTROL=DATA;   /* D/I = 1 , send data      */
	   WRITE(0x00);
	 }
    }
 CONTROL=CS1_DIS;
 WRITE(0xc0);                        /* display start line       */
 WRITE(0x40);                        /* set y address , y = 0    */
 for(i = 2;i < 8;i++)                /* clear graphic LCD        */
    { CONTROL=INSTRUCTION; /* D/I = 0 , Instruction    */
      WRITE(0xb8 + i);               /* set page (x address)     */
      for(j = 0;j < 64;j++)
	 { CONTROL=DATA;   /* D/I = 1 , send data      */
	   WRITE(0x00);
	 }
    }

 }

void MainScr(unsigned long pbTxAmount,unsigned char bDotPos)
{
 int icx;
 unsigned char ucP;
 unsigned long ulBal;
 BYTE abBal[8];
 BlankData();
 CONTROL=CS1_EN;         /* right side disable       */
 PUT_WORD(3,32,scAscii,':'-' ');
 LcdPrintf(3,0,FareValue,2,16);
 CONTROL=CS1_DIS;
 ultoa(pbTxAmount,abBal,10);
 LcdPrintNumber16(3,4,abBal,(unsigned char)strlen(abBal),bDotPos);
}

	/*****************************************************************************************************/
	/*	Name			: PUT_WORD																									  */
	/*	Description	: 																		  */
	/*****************************************************************************************************/
void	PUT_WORD(int x,int y,char *cWordType,int word_number)
{
 int i;
 for(i = 0;i < 2;i++)
    { CONTROL=INSTRUCTION;
      WRITE(0x40 + y);
      WRITE(0xb8 + x+i);
      for(j = word_number * 32+i*16;j < (word_number * 32 +(i+1 ) * 16);j ++)
	 { CONTROL=DATA;
	   WRITE(*(cWordType + j ));
	 }
    }
}

void LcdPrintNumber16(int iX,int iY,BYTE *pbDat,BYTE bLen,BYTE bDotPos)
{
 int iCn,i,j;
 int iTempX,iTempY,iOffset;
 BYTE bSide;

   iOffset=0;
   if (iY>RIGHT_POS_16){
	bSide=FALSE;
	iTempX=iX;

	iTempY=(iY-RIGHT_POS_16)*16;
	CONTROL=CS1_DIS;
	}
   else {
	bSide=TRUE;
	iTempX=iX;
	iTempY=iY*16;
	CONTROL=CS1_EN;
       }

   //CONTROL=CS1_DIS;
   for (iCn=0;iCn<bLen;iCn++){
      if ((bLen-iCn)==bDotPos) {
	 PUT_WORD(iTempX,iTempY+iCn*16-iOffset,scAscii,'.'-' ');
	 iTempY+=2;
	 }
      PUT_WORD(iTempX,iTempY+iCn*16-iOffset,scAscii,*(pbDat+iCn)-' ');
   if( ((iY+iCn+1)>RIGHT_POS_16) && (bSide==TRUE)){
	iTempX=iX;
	bSide=FALSE;
	//iOffset=(iTempY+(iCn+1)*8);
	//printf("\n Chage side %02x",iOffset);
	iTempY=0;
	iOffset=(iCn+1)*16;
	CONTROL=CS1_DIS;
	}
   //else{

	//iTempX=iX;
	//iTempY=iY*8;
     //	CONTROL=CS1_EN;
     //	}
   //   }

      }
      //PUT_WORD8(iTempX,41,&scAscii8,'.'-' ');
      //PUT_WORD8(iTempX,48,&scAscii8,'0'-' ');

}

	/*****************************************************************************************************/
	/*	Name			: DisplayBalance																							  */
	/*	Description	: Display																		  */
	/*****************************************************************************************************/
void DisplayBalance(unsigned long pbTemp,int iCounter)
{
 unsigned long	ulBal;
 int				iCn;
 unsigned char	bTemp,abAux[4],abBal[4];

 BlankData();
 LcdPrintf(2,0,scBal,2,FON16);									/*	display µ² ¾l										  */
 PUT_WORD(2,32,scAscii,':'-' ');
// LcdPrintf(4,0,scCount,2,FON16);									/* display ¦¸ ¼Æ									  */
// PUT_WORD(4,32,scAscii,':'-' ');
//	  itoa(iCounter,abAux,10);
//	  LcdPrintNumber(4,8,abAux,strlen(abAux),0);
//	  ulBal=(unsigned  long)*pbTemp*16777216L+(unsigned  long)*(pbTemp+1)*65536L+(unsigned  long)*(pbTemp+2)*256+(unsigned  long)*(pbTemp+3);
	  ultoa(pbTemp,abBal,10);
	  iCn=strlen(abBal);
	  if (iCn==1){
	    bTemp=abBal[0];
	    abBal[0]='0';
	    abBal[1]=bTemp;
	    iCn++;
	    }
	  LcdPrintNumber(2,8,abBal,iCn,1);

	    LcdPrintNumber(2,8,abBal,iCn,1);
//	    BeepOK();
}

void LcdPrintNumber(int iX,int iY,unsigned char *pbDat,unsigned char bLen,unsigned char bDotPos)
{
 int iCn,i,j;
 int iTempX,iTempY,iOffset;
 unsigned char bSide;

   iOffset=0;
   if (iY>RIGHT_POS){
	bSide=FALSE;
	iTempX=iX;

	iTempY=(iY-RIGHT_POS)*8;
	CONTROL=CS1_DIS;
	}
   else {
	bSide=TRUE;
	iTempX=iX;
	iTempY=iY*8;
	CONTROL=CS1_EN;
       }

   //CONTROL=CS1_DIS;
   for (iCn=0;iCn<bLen;iCn++){
      if ((bLen-iCn)==bDotPos) {
	 PUT_WORD8(iTempX,iTempY+iCn*8-iOffset,scAscii8,'.'-' ');
	 iTempY+=8;
	 }
      PUT_WORD8(iTempX,iTempY+iCn*8-iOffset,scAscii8,*(pbDat+iCn)-' ');
   if( ((iY+iCn+1)>RIGHT_POS) && (bSide==TRUE)){
	iTempX=iX;
	bSide=FALSE;
	//iOffset=(iTempY+(iCn+1)*8);
	//printf("\n Chage side %02x",iOffset);
	iTempY=0;
	iOffset=(iCn+1)*8;
	CONTROL=CS1_DIS;
	}
   //else{

	//iTempX=iX;
	//iTempY=iY*8;
     //	CONTROL=CS1_EN;
     //	}
   //   }

      }
      //PUT_WORD8(iTempX,41,&scAscii8,'.'-' ');
      //PUT_WORD8(iTempX,48,&scAscii8,'0'-' ');

}

/********************************************************************************************************/
/* Name			: LCDDisplay																									  */
/*	Description	: LCD Display						 						 													  */
/* Parameter	: pChar -- ptr of Character	 																			  */
/*					  X -- X position																								  */
/*					  Y -- Y position																								  */
/*					  Len -- Length of display data																			  */
/* Return		: none																 											  */
/********************************************************************************************************/
void LCDDisplay(unsigned char X, unsigned char Y, unsigned char* pChar, unsigned char Len) 
{
 unsigned int i;
 gotoxy(X,Y);
 for (i=0;i<Len;i++) putc(pChar[i], stdout);
}

