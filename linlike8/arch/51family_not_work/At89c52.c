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
#include "main.h"                                          // main header file => main application and its functions
#include "ctrller.h"                                       // Header file for hardware driver 

#ifdef AT89C52
#include <at89x52.h>                                       // call for sdcc compiler, include at89c52 structure
#include "uart.h"										/* Uart																	  */
#include "timer.h"                              // timer header

/*
 * Local Data
 ****************************************************************
 */
//#ifdef TIMER0
//idata unsigned int delayMSec;                               // global veriable for delay 
//#endif	// end of "#ifdef TIMER0"
#ifdef UART1
idata unsigned char uart1TxFlag;
#endif  // end of "#ifdef UART1"
 
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

#ifdef DEV_KIT_0
    // development kit #0 
    //*****************************************************************************************************
 TestOff();
 //P2 = (0x00);
 //P0_0 = 0;		
 //dis485Tx();
#ifdef LED_BUZZER
        	// clear all Leds and Buzzer 
 OutputDeviceOFF(BUZZER);
 OutputDeviceOFF(REDLED);
 OutputDeviceOFF(GREENLED);
 
#endif  // end of "#ifdef LED_BUZZER"
 

#endif	// end of "#ifdef DEV_KIT_0"

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
 if (uart1TxFlag!=TRUE) {
      //en485Tx();
      uart1TxFlag = TRUE;
      SBUF = Uart1TxBuf[pRdUart1TxBuf];
      pRdUart1TxBuf++;
      if (pRdUart1TxBuf>=MAX_UART_TXRX_BUF) pRdUart1TxBuf = 0;
     }
 return(TRUE);
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
                       // active the interrupt on high status
         pWrUart1RxBufBackup = pWrUart1RxBuf;                                   /* inc. pointer                                                                                   */
         pWrUart1RxBufBackup++;
         if (pWrUart1RxBufBackup>=MAX_UART_TXRX_BUF) pWrUart1RxBufBackup = 0;
         if (pWrUart1RxBufBackup!=pRdUart1RxBuf) {                      /* buf not ful                  */
                 Uart1RxBuf[pWrUart1RxBuf] = SBUF;              /* Rx one byte                                                                                            */
                 pWrUart1RxBuf = pWrUart1RxBufBackup;
                }
         //else                                                                                 // buf full return error
        }

        // Tx
        //*****************************************************************************************************
 else if (TI) {
      TI = 0;                           // after verifying which interrupts(TX/RX), clr

                                       // interrupt immediately, since this flag will

                                       // active the interrupt on high status
      if (pRdUart1TxBuf!=pWrUart1TxBuf) {                                                    // buf not empty, Tx again
         SBUF = Uart1TxBuf[pRdUart1TxBuf];                                                      // read one byte to transmit out
         pRdUart1TxBuf++;                                                                                                       // update read ptr of Tx buf
          if (pRdUart1TxBuf>=MAX_UART_TXRX_BUF) pRdUart1TxBuf = 0;
        }
      else {                         //  => don't put data into SBUF, then stop the TX
         uart1TxFlag = FALSE;       // clear Tx flag as complete tx 
         //dis485Tx();
        }
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
// delayMSec = delay;
 
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
// TL0 = (65536-delayMSec) % 256;
// TH0 = (65536-delayMSec) / 256;
 // HW auto. clear TF0

 runTimer();// call to 2nd part of isr of timer
}
#endif	// end of "#ifdef TIMER0"

#endif	// end of "#ifdef AT89C52"
    

/*
#include "util.h"                                       // header file of utilities

#ifdef PORT_CTRL
        	// Initialize for port ctrl 

 cntPortCtrl = 0;                                           // port control app. bit as 0 to start
#endif	// end of "#ifdef PORT_CTRL"
*/


