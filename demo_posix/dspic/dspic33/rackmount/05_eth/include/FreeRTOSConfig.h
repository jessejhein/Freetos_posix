/************************************************************************************************
 * File:            FreeRTOSConfig.h
 * Description:     Configuration file for FreeRTOS for dsPic
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

#include <define.h>

/*=======================System definitions================================*/
/*
 * configUSE_PREEMPTION
 * +-- Set to 1 to use the preemptive kernel
 * +-- Tasks will yield at regular interval defined by configTICK_RATE_HZ
 *     otherwise, will only yield when task block
 */
#define configUSE_PREEMPTION            1                              
/*
 * configUSE_IDLE_HOOK
 * +-- Set to 1 to use Idle Hook
 * +-- The Idle Hook is embedded in the Idle Task which will be run whenever
 *     when no other task are active.
 * +-- User should never block in Idle Hook
 */
#define configUSE_IDLE_HOOK             1
/*
 * configUSE_TICK_HOOK
 * +-- Set to 1 to use Tick Hook
 * +-- The Tick Hook is run at regular interval defined by configTICK_RATE_HZ
 * +-- Since Tick Hook is run in ISR, it should never be long
 */
#define configUSE_TICK_HOOK             0
/*
 * configTICK_RATE_HZ
 * +-- Determine the frequency of context switch
 * +-- 10Hz (100ms); 100Hz (10ms); 1000Hz (1ms) 
 */
#define configTICK_RATE_HZ              ( ( portTickType ) 100 )
/*
 * configCPU_CLOCK_HZ
 * +-- Inform the kernel of the System clock speed for proper time measurement
 * +-- SYSTEM_CLK_HZ is defined in <define.h>
 */
#define configCPU_CLOCK_HZ              SYSTEM_CLK_HZ
/*
 * configMAX_PRIORITIES
 * +-- The number of priorities available to the application tasks
 * +-- Increase this value increases RAM consumption 
 * +-- 1 (tskIDLE_PRIORITY); 2 (tskIDLE_PRIORITY +1), etc. 
 */
#define configMAX_PRIORITIES            ( ( unsigned portBASE_TYPE ) 1 )
/*
 * configMINIMAL_STACK_SIZE
 * +-- The stack size used by tasks, units dependable to platform
 * +-- For dsPic, units in WORD, so multiply by 2 to get BYTE
 */
#define configMINIMAL_STACK_SIZE        ( 250 )
/*
 * configTOTAL_HEAP_SIZE
 * +-- The total amount of RAM (in bytes) available to the kernel
 * +-- This value should be greater than 
 *          (configMINIMAL_STACK_SIZE*2)*(No of application + 1)
 */
#define configTOTAL_HEAP_SIZE           ( ( size_t ) 2048 )     // this 3 tasks(including idle), must at least 256x2 X3=1536
/*
 * configMAX_TASK_NAME_LEN
 * +-- The maximum permissible length of the descriptive name given 
 *     to a task when the task is created. 
 */
#define configMAX_TASK_NAME_LEN         ( 4 )
/*
 * configUSE_TRACE_FACILITY
 * +-- Set to 1 to use the trace visualisation functionality
 */
#define configUSE_TRACE_FACILITY        0
/*
 * configUSE_16_BIT_TICKS
 * +-- Setting this constant to 1 uses a 16-bit variable to count time tick
 * +-- Setting this constant to 0 uses a 32-bit variable to count time tick
 * +-- This constant affects the maximum duration that a task can be blocked
 */
#define configUSE_16_BIT_TICKS          1
/*
 * configIDLE_SHOULD_YIELD
 * +-- When set to 1, the Idle Task will yield immediately when it is finished
 * +-- When set to 0, the Idle Task will wait until the end of next context swtich
 *     occurence
 */
#define configIDLE_SHOULD_YIELD         1


/*=======================Co-routine definitions================================*/
/*
 * configUSE_CO_ROUTINES
 * +-- Set to 1 to use FreeRTOS coroutine
 */
#define configUSE_CO_ROUTINES           0
/*
 * configMAX_CO_ROUTINE_PRIORITIES
 * +-- The number of priorities available to the coroutines
 */
#define configMAX_CO_ROUTINE_PRIORITIES ( 1 )


/*=====Set the following definitions to 1 to include the API function,==========
  ============= or zero to exclude the API function.============================ */
#define INCLUDE_vTaskPrioritySet        0
#define INCLUDE_uxTaskPriorityGet       0
#define INCLUDE_vTaskDelete             0
#define INCLUDE_vTaskCleanUpResources   0
#define INCLUDE_vTaskSuspend            0
#define INCLUDE_vTaskDelayUntil         0
#define INCLUDE_vTaskDelay              1


#endif /* FREERTOS_CONFIG_H */
