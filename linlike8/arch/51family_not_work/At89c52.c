/* 
 * Project Name	: os
 * Project Description : os -- ... -- At89c52 -- At89c52 template driver
 * c file name : tAt89c52.c
 * c file Description : -- template driver of At89c52
 * Copyright (C) 10-2001, SC
 * Compiler Used : sdcc 2.2.1 UNIX
 * Person Resposible : Yan
 *
 ****************************************************************
 * 
 * Revision History
 * Rev  Date        Who     Reason
 * 0.01 20-10-2001  yan     start to create the template driver of At89c52
 * 0.02	21-10-2001  yan	    add uart tx and rx buffer 
 * 
 */

/*
 * include files 
 ****************************************************************
 */

#ifdef AT89C52
#include <at89x52.h>                                       // call for sdcc compiler, include at89c52 structure
#include "uart.h"										/* Uart																	  */
#include "timer.h"                              // timer header

/*
 * Local Data
 ****************************************************************
 */
 
/* 
 * name : CtrllerInit
 ****************************************************************
 */
void CtrllerInit(void)
{

    // At89c52 
    //*****************************************************************************************************
            // enable all interrupts
 IE |= 0x80;                                                                                    // enable All Flag

}

/* 
 * name : CtrllerClose
 ****************************************************************
 */
void CtrllerClose(void)
{
    
}

#ifdef UART1
/****************************************************************
 * module : Uart1 System
 ****************************************************************
 ****************************************************************/

/*
 * name : uart1HwSetup
 ****************************************************************
 */
//void uart1HwSetup(unsigned char mode, unsigned char enRec, unsigned char baudRate)
void uart1HwSetup(void)
{
 SCON = (1<<6)|0x10;			// mode 1 and rec enable
 TMOD = (TMOD&0x0f) | 0x20;                                             // timer 1, Ctrl by TR1, mode 2
 TH1 = 0xfd;                                                                                    // 9600 baud

    // Initialize for Uart
    //*****************************************************************************************************
// if (enRec) SCON = (mode<<6)|0x10;			// mode 1 and rec enable
// else SCON = (mode<<6);

               // enable time 1 as baud rate controller
// switch (baudRate) {
//      case 0 : 
//	    TMOD = (TMOD&0x0f) | 0x20;                                             // timer 1, Ctrl by TR1, mode 2
//	    TH1 = 0xfd;                                                                                    // 9600 baud
//           break;
//      case 1 :
//	    TMOD = (TMOD&0x0f) | 0x20;                                             // timer 1, Ctrl by TR1, mode 2
//	    TH1 = 0xfd;                                                                                    // 9600 baud
//           break;
//     }
     
    // Uart start ^M
    //*****************************************************************************************************^M
 uart1TxFlag = FALSE;
 TR1 = 1;                                                                                               // enable timer 1^M
 ES = 1;                                                                                                // enable UART1 interrupt^M
}

/*
 * name : EnTxCom1
 ****************************************************************
 */
unsigned char EnTxCom1(void) reentrant
{
      SBUF = ;
}

/*
 * name : Uart1ISR
 * using register bank 1
 ****************************************************************
 */
void Uart1ISR (void) interrupt 4 using 1
{
 unsigned char  pWrUart1RxBufBackup;

        // Rx
        //*****************************************************************************************************
 if (RI) {
         RI = 0;                               // after verifying which interrupts(TX/RX), clr
                                               // interrupt immediately, since this flag will
        }

        // Tx
        //*****************************************************************************************************
 else if (TI) {
      TI = 0;                           // after verifying which interrupts(TX/RX), clr

                                       // interrupt immediately, since this flag will

                                       // active the interrupt on high status
     }

}

/*
 * name : uart1HwClose
 ****************************************************************
 */
void uart1HwClose(void) 
{
}

#endif  // end of "#ifdef UART1"

#ifdef TIMER0
/*
 * name : timer0HwSetup
 ****************************************************************
 */
//void timer0Setup(unsigned int delay, unsigned char mode) reentrant
void timer0HwSetup(void) 
{
 
        	// Initialize for timer 
        	//*****************************************************************************************************
// TMOD = (TMOD&0xf0) | mode;                                 // timer0, Ctrl by TR0, mode 1
 TMOD = (TMOD&0xf0) | 1;                                    // timer0, Ctrl by TR0, mode 1
 TL0 = (65536-50000) % 256;                                  // delay for 50mSec
 TH0 = (65536-50000) / 256;
            // timer start 
            //*****************************************************************************************************
 TR0 = 1;												// enable timer 0
 ET0 = 1;												// enable timer 0 interrupt

}

/*
 * name : timer0HwSetup
 ****************************************************************
 */
void timer0HwClose(void)
{
}

/* 
 * name : Timer0ISR
 * using register bank 1
 ****************************************************************
 */
void Timer0ISR (void) interrupt 1 using 1 
{

	// setting for next interrupt 
	//*****************************************************************************************************
 TL0 = (65536-50000) % 256;                                  // delay for 50mSec
 TH0 = (65536-50000) / 256;
 // HW auto. clear TF0

 runTimer();// call to 2nd part of isr of timer
}
#endif	// end of "#ifdef TIMER0"

#endif	// end of "#ifdef AT89C52"
    

