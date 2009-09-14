/**
 * \file
 * boot up file for module using FreeRTOS
 * \author Dennis Tsang <dennis@amonics.com>
 */

/**
 * \page boot SYSTEM BOOT UP
 * \li This file is created based on the Demo Application in /freeRTOS/Demo/dsPIC_MPLAB/main.c
 * \li User should implement their own vSetupHardware() and vUserMain().
 * \li vSetupHardware is used to set up the hardware (e.g. DI/DO pins, fd_uart, fd_eeprom, etc.)
 * \li vUserMain() should contains the creation of application threads/coroutine
 */

/*
  FreeRTOS.org V5.0.3 - Copyright (C) 2003-2008 Richard Barry.

  This file is part of the FreeRTOS.org distribution.

  FreeRTOS.org is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  FreeRTOS.org is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with FreeRTOS.org; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  A special exception to the GPL can be applied should you wish to distribute
  a combined work that includes FreeRTOS.org, without being obliged to provide
  the source code for any proprietary components.  See the licensing section 
  of http://www.FreeRTOS.org for full details of how and when the exception
  can be applied.

    ***************************************************************************
    ***************************************************************************
    *                                                                         *
    * SAVE TIME AND MONEY!  We can port FreeRTOS.org to your own hardware,    *
    * and even write all or part of your application on your behalf.          *
    * See http://www.OpenRTOS.com for details of the services we provide to   *
    * expedite your project.                                                  *
    *                                                                         *
    ***************************************************************************
    ***************************************************************************

  Please ensure to read the configuration and relevant port sections of the
  online documentation.

  http://www.FreeRTOS.org - Documentation, latest information, license and 
  contact details.

  http://www.SafeRTOS.com - A version that is certified for use in safety 
  critical systems.

  http://www.OpenRTOS.com - Commercial support, development, porting, 
  licensing and training services.
*/

/* Scheduler includes. */
#include <FreeRTOS.h>
#include <asm/delay.h>
#include <asm/types.h>
#ifdef CRTHREAD_SCHED
#include <pthread.h>
#endif

#define portTIMER_PRESCALE 8

/* Defined for backward compatability with project created prior to 
FreeRTOS.org V4.3.0. */
#ifndef configKERNEL_INTERRUPT_PRIORITY
  #define configKERNEL_INTERRUPT_PRIORITY 1
#endif

#if configKERNEL_INTERRUPT_PRIORITY != 1
  #error If configKERNEL_INTERRUPT_PRIORITY is not 1 then the #32 in the following macros needs changing to equal the portINTERRUPT_BITS value, which is ( configKERNEL_INTERRUPT_PRIORITY << 5 )
#endif

/************************************************************************************************
 * Configuration Bits Setting
 * +-- Refer to dsPIC33F Family Data Sheet (DS70165E) 
 * 		+-- Section 23.1 for definitions of configuration bits
 * 		+-- Section 8.1 for system clock settings
 ************************************************************************************************/

_FOSCSEL(FNOSC_FRCPLL & IESO_OFF);                    // Internal Fast RC (FRC) w/ PLL
                                                      // Start up with user-selected oscillator

#ifdef EXTERNAL_CLOCK_SOURCE
_FOSC(FCKSM_CSECME & POSCMD_HS);                      // Clock Switching and Fail Safe Clock Monitor are enabled
                                                      // Primary Oscillator Mode: High Speed
                                                      // OSC pin has clock out function
#else  /* INTERNAL_CLOCK_SOURCE */
_FOSC(FCKSM_CSECME & POSCMD_NONE & OSCIOFNC_ON);      // Clock Switching and Fail Safe Clock Monitor are enabled
                                                      // Primary Oscillator Mode: Disabled
                                                      // OSC pin funcition as Digital I/O
#endif /* INTERNAL_CLOCK_SOURCE */

_FWDT(FWDTEN_OFF);                                    // Watchdog Timer Disabled

/************************************************************************************************/

/************************************************************************************************
 * External functions for Hardware Setup and User Main Program before kernal starts
 ************************************************************************************************/
extern void vSetupHardware(void);     //Defined in user main application
extern void vUserMain(void);          //Defined in user main application
extern void __builtin_write_OSCCONH(unsigned int);
extern void __builtin_write_OSCCONL(unsigned int);

/************************************************************************************************
 * Global functions
 ************************************************************************************************/
void prvSetupTimerInterrupt( void );

/************************************************************************************************
 * Global Variables 
 *************************************************************************************************/
time_t one_sec_cnt;
int timer_count;                      //counts from 0 upto configTICK_RATE_HZ
#ifndef FREERTOS_SCHED 
  time_t jiffies;
#endif /* FREERTOS_SCHED */
volatile int errno = 0;               //Indicate error state of open(), read() write();

/************************************************************************************************
 * main()
 * 	+--Entry point after hardware reset
 ************************************************************************************************/
int 
main(void)
{
  /*--------------------------------------------------------------------------------
	 * Configure Oscillator to operate the device at 40Mhz
	 * Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
	 * Fosc= 8M*40/(2*2)=80Mhz for 8M input clock
	 *--------------------------------------------------------------------------------
	 *	FRC(7.37MHz) -> PLLPRE(/2) -> X --> VCO -> PLLPOST(/2) -> FOSC  -> (/2) -> FCY
	 *								 /|\     |
	 *								  ---- PLLDIV (x40)
	 *--------------------------------------------------------------------------------
   */	
  _PLLDIV = 38;                     // M=40: PLL Feedback Divisor bits
  CLKDIV = 0;                       // N1=2: PLL VCO Output Divider Select bits
                                    // N2=2: PLL Phase Detector Input Divider bits
  OSCTUN = TUNE_FRC;                // Tune FRC oscillator, if FRC is used; 
                                    // 0: Center frequency (7.37 MHz nominal)
                                    // 22: +8.25% (7.98 MHz)
  RCONbits.SWDTEN = 0;              // Disable Watch Dog Timer
  while(OSCCONbits.LOCK != 1);      // Wait for PLL to lock
  //--------------------------------------------------------------------------------

#ifdef EXTERNAL_CLOCK_SOURCE
  //Perform clk switch to LPRC (use this a a transition because cannot adjust PLL setting when it is in used)
  __builtin_write_OSCCONH(5);       // New OSC = LPRC 
  __builtin_write_OSCCONL(1);       // Start clk switch
  while(OSCCONbits.OSWEN == 1);     // Wait for completion
  
  //Perform clk switch to external crytal
  //PLL config for 10MHz crystal 
  _PLLDIV = 30;                     // M=32: PLL Feedback Divisor bits
  CLKDIV = 0;                       // N1=2: PLL VCO Output Divider Select bits
                                    // N2=2: PLL Phase Detector Input Divider bits
  __builtin_write_OSCCONH(3);       // New OSC = PRI PLL 
  __builtin_write_OSCCONL(1);       // Start clk switch
  while(OSCCONbits.OSWEN == 1);     // Wait for completion
#endif /* EXTERNAL_CLOCK_SOURCE */

  /* allow 1 sec delay for on-board external hardware to stable*/
  mdelay(1000);

  /* set adc compatiable pins to digital io by default */
  AD1PCFGL = 0xFFFF;
  AD1PCFGH = 0xFFFF;

  /* Configure any hardware. */
  vSetupHardware();

#ifdef CRTHREAD_SCHED
  unsigned char index;
  for(index=0; index<MAX_CRTHREAD; index++)
    {
      crlist[index].crthread = (((crthread_t) 0 ) + MAX_CRTHREAD);
    }
#endif /* CRTHREAD_SCHED */

#ifndef FREERTOS_SCHED 
  prvSetupTimerInterrupt();       //start timer if FreeRTOS scheduler is disabled 
#endif /* FREERTOS_SCHED */

  /* Create the main task. */
  vUserMain();

#ifdef FREERTOS_SCHED 
  /* Finally start the scheduler. */
  vTaskStartScheduler();

  /* Will only reach here if there is insufficient heap available to start the scheduler. */
  ERR_LED_CONFIG();
  while(1)
    {
      ERR_LED0(1);
      ERR_LED1(1);
      mdelay(100); 
      ERR_LED0(0);
      ERR_LED1(0);
      mdelay(100);
    }  
#endif /* FREERTOS_SCHED */
  return 0;
}
/*-----------------------------------------------------------*/

/*
 * Setup a timer for a regular tick.
 */
void 
prvSetupTimerInterrupt( void )
{
  const unsigned portLONG ulCompareMatch = ( configCPU_CLOCK_HZ / portTIMER_PRESCALE ) / configTICK_RATE_HZ;

  /* Initialize counters */
  one_sec_cnt = 0;
  timer_count = 0;
#ifndef FREERTOS_SCHED 
  jiffies = 0;
#endif /* FREERTOS_SCHED */

  /* Prescale of 8. */
  T1CON = 0;
  TMR1 = 0;

  PR1 = ( unsigned portSHORT ) ulCompareMatch;

  /* Setup timer 1 interrupt priority. */
  IPC0bits.T1IP = configKERNEL_INTERRUPT_PRIORITY;

  /* Clear the interrupt as a starting condition. */
  IFS0bits.T1IF = 0;

  /* Enable the interrupt. */
  IEC0bits.T1IE = 1;

  /* Setup the prescale value. */
  T1CONbits.TCKPS0 = 1;
  T1CONbits.TCKPS1 = 0;

  /* Start the timer. */
  T1CONbits.TON = 1;
}
/*-----------------------------------------------------------*/

void 
_IRQ _T1Interrupt( void )
{
  /* Clear the timer interrupt. */
  IFS0bits.T1IF = 0;

#ifdef FREERTOS_SCHED 
  vTaskIncrementTick();
#else /* not FREERTOS_SCHED */
  jiffies++;
#endif /* not FREERTOS_SCHED */

  /* record time */
  if(++timer_count == configTICK_RATE_HZ)
    {
      one_sec_cnt++;
      timer_count = 0;
    }

#ifdef FREERTOS_SCHED 
#if configUSE_PREEMPTION == 1
  portYIELD();
#endif /* configUSE_PREEMPTION */
#endif /* FREERTOS_SCHED */
}

/*
 * Reset System
 */
void
reset(void)
{
  //user shutdown rountine
  vUserShutdown();

#ifdef EXTERNAL_CLOCK_SOURCE
  //Perform clk switch to LPRC (use this a a transition because cannot adjust PLL setting when it is in used)
  __builtin_write_OSCCONH(5);       // New OSC = LPRC 
  __builtin_write_OSCCONL(1);       // Start clk switch
  while(OSCCONbits.OSWEN == 1);     // Wait for completion
  
  //Perform clk switch to internal FRC
  _PLLDIV = 38;                     // M=40: PLL Feedback Divisor bits
  CLKDIV = 0;                       // N1=2: PLL VCO Output Divider Select bits
                                    // N2=2: PLL Phase Detector Input Divider bits
  OSCTUN = TUNE_FRC;                // Tune FRC oscillator, if FRC is used; 
                                    // 0: Center frequency (7.37 MHz nominal)
                                    // 22: +8.25% (7.98 MHz)
  __builtin_write_OSCCONH(1);       // New OSC = PRI PLL 
  __builtin_write_OSCCONL(1);       // Start clk switch
  while(OSCCONbits.OSWEN == 1);     // Wait for completion
#endif /* EXTERNAL_CLOCK_SOURCE */

  asm("reset");
}
