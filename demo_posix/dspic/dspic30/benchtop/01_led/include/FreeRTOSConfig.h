/************************************************************************************************
 * File: 			FreeRTOSConfig.h
 * Description:		Configuration file for FreeRTOS for dsPic30F5011
 ***********************************************************************************************
 * DESCRIPTION:
 * 	1)	This file is modified from /FreeRTOS/Demo/dsPIC_MPLAB/FreeRTOSConfig.h
 * 	2)	Kernel Clock Ticks every 10ms 
 * 			#define configTICK_RATE_HZ				( ( portTickType ) 100 )   	//10ms
 * 	3)	System Clock Frequency is (29491200) 
 * 			#define configCPU_CLOCK_HZ				SYSTEM_CLK_HZ				//See define.h
 * 	4)	Heap size is 1024
 * 			#define configTOTAL_HEAP_SIZE			( ( size_t ) 1024 )
 * 	5)	All task operations are not disabled
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

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <p30f5011.h>
#include <define.h>

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION			1	//Use preemptive scheduling
#define configUSE_IDLE_HOOK				1
#define configUSE_TICK_HOOK				0
#define configTICK_RATE_HZ				( ( portTickType ) 100 )   	//10ms
#define configCPU_CLOCK_HZ				SYSTEM_CLK_HZ				//See define.h
//#define configCPU_CLOCK_HZ				( ( unsigned portLONG ) 30000000 ) 			//See define.h
#define configMAX_PRIORITIES			( ( unsigned portBASE_TYPE ) 1 )	//Use tskIDLE_PRIORITY only
#define configMINIMAL_STACK_SIZE		( 105 )	//x2 for number of bytes
#define configTOTAL_HEAP_SIZE			( ( size_t ) 1024 )
#define configMAX_TASK_NAME_LEN			( 4 )
#define configUSE_TRACE_FACILITY		0
#define configUSE_16_BIT_TICKS			1
#define configIDLE_SHOULD_YIELD			1

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0				//disable freeRTOS coroutines
#define configMAX_CO_ROUTINE_PRIORITIES ( 1 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet		0
#define INCLUDE_uxTaskPriorityGet		0
#define INCLUDE_vTaskDelete				0
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			0
#define INCLUDE_vTaskDelayUntil			0
#define INCLUDE_vTaskDelay				1


#endif /* FREERTOS_CONFIG_H */
