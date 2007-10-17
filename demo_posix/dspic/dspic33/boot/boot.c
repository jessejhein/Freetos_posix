/************************************************************************************************
 * File: 			boot.c
 * Description:		boot up file for module using FreeRTOS
 ***********************************************************************************************
 * DESCRIPTION:
 * 	1) 	This file is created based on the Demo Application in /freeRTOS/Demo/dsPIC_MPLAB/main.c
 * 	2)	User should implement their own vSetupHardware() and vUserMain().
 * 	3) 	vSetupHardware is used to set up the hardware (e.g. DI/DO pins, fd_uart, fd_eeprom, etc.)
 * 	4) 	vUserMain() should contains the creation of application threads/coroutine
 ***********************************************************************************************/

/*
	FreeRTOS.org V4.1.3 - Copyright (C) 2003-2006 Richard Barry.

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
	See http://www.FreeRTOS.org for documentation, latest information, license 
	and contact details.  Please ensure to read the configuration and relevant 
	port sections of the online documentation.
	***************************************************************************
*/

/* Scheduler includes. */
#include <FreeRTOS.h>
#include <asm/delay.h>
#include <asm/types.h>
#ifdef CRTHREAD_SCHED
#include <pthread.h>
#endif

#define portTIMER_PRESCALE 8

/************************************************************************************************
 * Configuration Bits Setting
 * +-- Refer to dsPIC33F Family Data Sheet (DS70165E) 
 * 		+-- Section 23.1 for definitions of configuration bits
 * 		+-- Section 8.1 for system clock settings
 ************************************************************************************************/
_FOSCSEL(FNOSC_FRCPLL);									// FRC Oscillator with PLL (default FRC divide by 8)
_FOSC(FCKSM_CSDCMD & OSCIOFNC_ON  & POSCMD_NONE); 		// Clock Switching and Fail Safe Clock Monitor is disabled
														// OSC2 Pin Function: OSC2 is Digital I/O
														// Primary Oscillator Mode: Disabled
_FWDT(FWDTEN_OFF);              						// Watchdog Timer Enabled/disabled by user software
														// (LPRC can be disabled by clearing SWDTEN bit in RCON register
/************************************************************************************************/

/************************************************************************************************
 * External functions for Hardware Setup and User Main Program before kernal starts
 ************************************************************************************************/
extern void vSetupHardware(void);	//Defined in user main application
extern void vUserMain(void);		//Defined in user main application

/************************************************************************************************
 * Global functions
 ************************************************************************************************/
void prvSetupTimerInterrupt( void );

/************************************************************************************************
 * Global Variables 
 *************************************************************************************************/
time_t one_sec_cnt;
int timer_count;                //counts from 0 upto configTICK_RATE_HZ
#ifndef FREERTOS_SCHED 
    time_t jiffies;
#endif
volatile int errno = 0;     //Indicate error state of open(), read() write();

/************************************************************************************************
 * main()
 * 	+--Entry point after hardware reset
 ************************************************************************************************/
int main( void )
{
	//--------------------------------------------------------------------------------
	// Configure Oscillator to operate the device at 40Mhz
	// Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
	// Fosc= 8M*40/(2*2)=80Mhz for 8M input clock
	//--------------------------------------------------------------------------------
	//	FRC(7.37MHz) -> PLLPRE(/2) -> X --> VCO -> PLLPOST(/2) -> FOSC  -> (/2) -> FCY
	//								 /|\     |
	//								  ---- PLLDIV (x40)
	//--------------------------------------------------------------------------------	
    _PLLDIV = 38;                   // M=40: PLL Feedback Divisor bits
    CLKDIV = 0;                     // N1=2: PLL VCO Output Divider Select bits
                                    // N2=2: PLL Phase Detector Input Divider bits
	OSCTUN = TUNE_FRC;				// Tune FRC oscillator, if FRC is used; 
									// 0: Center frequency (7.37 MHz nominal)
									// 22: +8.25% (7.98 MHz)
	RCONbits.SWDTEN = 0;			// Disable Watch Dog Timer
	while(OSCCONbits.LOCK != 1);	// Wait for PLL to lock
	//--------------------------------------------------------------------------------
	
	/* Configure any hardware. */
	vSetupHardware();

#ifdef CRTHREAD_SCHED
    unsigned char index;
    for(index=0; index<MAX_CRTHREAD; index++)
    {
        crthread[index] = (((crthread_t) 0 ) + MAX_CRTHREAD);
    }
#endif

#ifndef FREERTOS_SCHED 
    prvSetupTimerInterrupt();       //start timer if FreeRTOS scheduler is disabled 
#endif

	/* Create the main task. */
	vUserMain();

#ifdef FREERTOS_SCHED 
	/* Finally start the scheduler. */
	vTaskStartScheduler();

	/* Will only reach here if there is insufficient heap available to start
	the scheduler. */
    ERR_LED0_EN();
    ERR_LED1_EN();
    while(1){
        ERR_LED0(1);
        ERR_LED1(1);
        mdelay(100); 
        ERR_LED0(0);
        ERR_LED1(0);
        mdelay(100); 
    }  
#endif
	return 0;
}
/*-----------------------------------------------------------*/

/*
 * Setup a timer for a regular tick.
 */
void prvSetupTimerInterrupt( void )
{
const unsigned portLONG ulCompareMatch = ( configCPU_CLOCK_HZ / portTIMER_PRESCALE ) / configTICK_RATE_HZ;

    /* Initialize counters */
    one_sec_cnt = 0;
    timer_count = 0;
#ifndef FREERTOS_SCHED 
    jiffies = 0;
#endif

    /* Prescale of 8. */
    T1CON = 0;
    TMR1 = 0;

    PR1 = ( unsigned portSHORT ) ulCompareMatch;

    /* Setup timer 1 interrupt priority. */
    IPC0bits.T1IP = portKERNEL_INTERRUPT_PRIORITY;

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

void __attribute__((__interrupt__)) _T1Interrupt( void )
{
#ifdef FREERTOS_SCHED 
    vTaskIncrementTick();
#else
    jiffies++;
#endif

    /* record time */
    if(++timer_count == configTICK_RATE_HZ)
    {
       one_sec_cnt++;
       timer_count = 0; 
    }
    
    /* Clear the timer interrupt. */
    IFS0bits.T1IF = 0;

#ifdef FREERTOS_SCHED 
    #if configUSE_PREEMPTION == 1
        portYIELD();
    #endif
#endif
}
