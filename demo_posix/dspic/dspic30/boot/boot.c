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
#include <asm/system.h>

#define portTIMER_PRESCALE 8

/************************************************************************************************
 * Configuration Bits Setting
 ************************************************************************************************/
_FOSC(CSW_FSCM_OFF & FRC_PLL16);  					//Turn off clock switching (CSW) and 
													//fail-safe clock monitoring (FSCM)
													//Use Internal Fast RC (FRC) as system clock 
                                					//routed via the PLL in 16x multiplier mode
                                					//7.37 MHz throughput of 7.37e+6*16/4=29.4 MIPS(Fcy)
                                					//,~33.9 nanoseconds instruction cycle time(Tcy).
_FWDT(WDT_OFF);                 					//Turn off Watchdog Timer
_FBORPOR(PBOR_ON & BORV_27 & MCLR_DIS & PWRT_16); 	//Set Brown-out Reset voltage to 2.7V
													//Disable MCLR reset pin and set  
                                					//Power-up Timer to 16ms
_FGS(CODE_PROT_OFF);            					//Disable Code Protection
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
#if (FREERTOS_SCHE_ENABLE == 0)
    time_t jiffies;
#endif
volatile int errno = 0;     //Indicate error state of open(), read() write();

/************************************************************************************************
 * main()
 * 	+--Entry point after hardware reset
 ************************************************************************************************/
int main( void )
{
	/* Configure any hardware. */
	vSetupHardware();

#if (FREERTOS_SCHE_ENABLE == 0)
    prvSetupTimerInterrupt();       //start timer if FreeRTOS scheduler is disabled 
#endif
    
	/* Create the main task. */
	vUserMain();

#if (FREERTOS_SCHE_ENABLE == 1)
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
#if (FREERTOS_SCHE_ENABLE == 0)
    jiffies = 0;
#endif
    
    /* Prescale of 8. */
    T1CON = 0;
    TMR1 = 0;

    PR1 = ( unsigned portSHORT ) ulCompareMatch;    //3750 for 30MIP

    /* Setup timer 1 interrupt priority. */
    IPC0bits.T1IP = portKERNEL_INTERRUPT_PRIORITY;

    /* Clear the interrupt as a starting condition. */
    IFS0bits.T1IF = 0;

    /* Enable the interrupt. */
    IEC0bits.T1IE = 1;

    /* Setup the prescale value. */
    T1CONbits.TCKPS = 1;        //Modified by Dennis

    /* Start the timer. */
    T1CONbits.TON = 1;
}

/*-----------------------------------------------------------*/
void __attribute__((__interrupt__)) _T1Interrupt( void )
{
#if (FREERTOS_SCHE_ENABLE == 1)
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
    DISI_PROTECT(IFS0bits.T1IF = 0);

#if (FREERTOS_SCHE_ENABLE == 1)
    #if configUSE_PREEMPTION == 1
        portYIELD();
    #endif
#endif
}