/************************************************************************************************
 * File: 			port.c
 * Description:		manipulation of dspic30 registers during context switch using FreeRTOS
 * 					setting Timer 1 as the kernel clock
 ***********************************************************************************************
 * DESCRIPTION:
 * 	1) 	This file is created based on /freeRTOS/Source/portable/MPLAB/PIC24_dsPIC/port.c
 * 	2)	User should define MPLAB_DSPIC30_PORT in makefile.
 * 	3)	Changes made included:
 * 		a) Delete
 * 				#ifdef MPLAB_PIC24_PORT
 * 					...
 * 				#endif /* MPLAB_PIC24_PORT */			/*
 * 		b) Rename (IMPORTANT NOTE: TWO renaming are needed)
 * 				MPLAB_DSPIC_PORT to MPLAB_DSPIC30_PORT
 *      c) Move timer1 related functions and constants to boot.c
 ***********************************************************************************************
 * Date			Author		Remarks
 * 30-01-2006	Dennis		First Creation
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

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the dsPic30F5011
 *----------------------------------------------------------*/

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

/* Hardware specifics. */
#define portBIT_SET 1
#define portINITIAL_SR	0

/* The program counter is only 23 bits. */
#define portUNUSED_PR_BITS	0x7f

/* Records the nesting depth of calls to portENTER_CRITICAL(). */
unsigned portBASE_TYPE uxCriticalNesting = 0xef;

#ifdef MPLAB_DSPIC30_PORT

	#define portRESTORE_CONTEXT()																						\
		asm volatile(	"MOV	_pxCurrentTCB, W0		\n"	/* Restore the stack pointer for the task. */				\
						"MOV	[W0], W15				\n"																\
						"POP	W0						\n"	/* Restore the critical nesting counter for the task. */	\
						"MOV	W0, _uxCriticalNesting	\n"																\
						"POP	PSVPAG					\n"																\
						"POP	CORCON					\n"																\
						"POP	DOENDH					\n"																\
						"POP	DOENDL					\n"																\
						"POP	DOSTARTH				\n"																\
						"POP	DOSTARTL				\n"																\
						"POP	DCOUNT					\n"																\
						"POP	ACCBU					\n"																\
						"POP	ACCBH					\n"																\
						"POP	ACCBL					\n"																\
						"POP	ACCAU					\n"																\
						"POP	ACCAH					\n"																\
						"POP	ACCAL					\n"																\
						"POP	TBLPAG					\n"																\
						"POP	RCOUNT					\n"	/* Restore the registers from the stack. */					\
						"POP	W14						\n"																\
						"POP.D	W12						\n"																\
						"POP.D	W10						\n"																\
						"POP.D	W8						\n"																\
						"POP.D	W6						\n"																\
						"POP.D	W4						\n"																\
						"POP.D	W2						\n"																\
						"POP.D	W0						\n"																\
						"POP	SR						  " );


	#define portSAVE_CONTEXT()																							\
		asm volatile(	"PUSH	SR						\n"	/* Save the SR used by the task.... */						\
						"PUSH	W0						\n"	/* ....then disable interrupts. */							\
						"MOV	#224, W0				\n"																\
						"MOV	W0, SR					\n"																\
						"PUSH	W1						\n"	/* Save registers to the stack. */							\
						"PUSH.D	W2						\n"																\
						"PUSH.D	W4						\n"																\
						"PUSH.D	W6						\n"																\
						"PUSH.D W8						\n"																\
						"PUSH.D W10						\n"																\
						"PUSH.D	W12						\n"																\
						"PUSH	W14						\n"																\
						"PUSH	RCOUNT					\n"																\
						"PUSH	TBLPAG					\n"																\
						"PUSH	ACCAL					\n"																\
						"PUSH	ACCAH					\n"																\
						"PUSH	ACCAU					\n"																\
						"PUSH	ACCBL					\n"																\
						"PUSH	ACCBH					\n"																\
						"PUSH	ACCBU					\n"																\
						"PUSH	DCOUNT					\n"																\
						"PUSH	DOSTARTL				\n"																\
						"PUSH	DOSTARTH				\n"																\
						"PUSH	DOENDL					\n"																\
						"PUSH	DOENDH					\n"																\
						"PUSH	CORCON					\n"																\
						"PUSH	PSVPAG					\n"																\
						"MOV	_uxCriticalNesting, W0	\n"	/* Save the critical nesting counter for the task. */		\
						"PUSH	W0						\n"																\
						"MOV	_pxCurrentTCB, W0		\n"	/* Save the new top of stack into the TCB. */				\
						"MOV	W15, [W0]				  " );

#endif /* MPLAB_DSPIC_PORT */

/*
 * Setup the timer used to generate the tick interrupt.
 */
extern void prvSetupTimerInterrupt( void );

/* 
 * See header file for description. 
 */
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
unsigned portSHORT usCode;
portBASE_TYPE i;

const portSTACK_TYPE xInitialStack[] = 
{
	0x1111,	/* W1 */
	0x2222, /* W2 */
	0x3333, /* W3 */
	0x4444, /* W4 */
	0x5555, /* W5 */
	0x6666, /* W6 */
	0x7777, /* W7 */
	0x8888, /* W8 */
	0x9999, /* W9 */
	0xaaaa, /* W10 */
	0xbbbb, /* W11 */
	0xcccc, /* W12 */
	0xdddd, /* W13 */
	0xeeee, /* W14 */
	0xcdce, /* RCOUNT */
	0xabac, /* TBLPAG */

	/* dsPIC specific registers. */
	#ifdef MPLAB_DSPIC30_PORT
		0x0202, /* ACCAL */
		0x0303, /* ACCAH */
		0x0404, /* ACCAU */
		0x0505, /* ACCBL */
		0x0606, /* ACCBH */
		0x0707, /* ACCBU */
		0x0808, /* DCOUNT */
		0x090a, /* DOSTARTL */
		0x1010, /* DOSTARTH */
		0x1110, /* DOENDL */
		0x1212, /* DOENDH */
	#endif
};

	/* Setup the stack as if a yield had occurred.

	Save the low bytes of the program counter. */
	usCode = ( unsigned portSHORT ) pxCode;
	*pxTopOfStack = ( portSTACK_TYPE ) usCode;
	pxTopOfStack++;

	/* Save the high byte of the program counter.  This will always be zero
	here as it is passed in a 16bit pointer.  If the address is greater than
	16 bits then the pointer will point to a jump table. */
	*pxTopOfStack = ( portSTACK_TYPE ) 0;
	pxTopOfStack++;

	/* Status register with interrupts enabled. */
	*pxTopOfStack = portINITIAL_SR;
	pxTopOfStack++;

	/* Parameters are passed in W0. */
	*pxTopOfStack = ( portSTACK_TYPE ) pvParameters;
	pxTopOfStack++;

	for( i = 0; i < ( sizeof( xInitialStack ) / sizeof( portSTACK_TYPE ) ); i++ )
	{
		*pxTopOfStack = xInitialStack[ i ];
		pxTopOfStack++;
	}

	*pxTopOfStack = CORCON;
	pxTopOfStack++;
	*pxTopOfStack = PSVPAG;
	pxTopOfStack++;

	/* Finally the critical nesting depth. */
	*pxTopOfStack = 0x00;
	pxTopOfStack++;

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

portBASE_TYPE xPortStartScheduler( void )
{
	/* Setup a timer for the tick ISR. */
	prvSetupTimerInterrupt(); 

	/* Restore the context of the first task to run. */
	portRESTORE_CONTEXT();

	/* Simulate the end of the yield function. */
	asm volatile ( "return" );

	/* Should not reach here. */
	return pdTRUE;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* It is unlikely that the scheduler for the PIC port will get stopped
	once running.  If required disable the tick interrupt here, then return 
	to xPortStartScheduler(). */
}
/*-----------------------------------------------------------*/

/*
 * Manual context switch.  This is similar to the tick context switch,
 * but does not increment the tick count.  It must be identical to the
 * tick context switch in how it stores the stack of a task.
 */
void vPortYield( void )
{
	portSAVE_CONTEXT();
	vTaskSwitchContext();
	portRESTORE_CONTEXT();
}
/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
	portDISABLE_INTERRUPTS();
	uxCriticalNesting++;
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
	uxCriticalNesting--;
	if( uxCriticalNesting == 0 )
	{
		portENABLE_INTERRUPTS();
	}
}