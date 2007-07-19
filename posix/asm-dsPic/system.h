/************************************************************************************************
 * File: 			system.h
 * Description:		System calls Definition
 ***********************************************************************************************/

#ifndef SYSTEM_H_
#define SYSTEM_H_

/***************************************************************************************
 * sti():	SeT Interrupt/Enable Interrupt
 * cli():	CLear Interrupt/Disable Interrupt
 ***************************************************************************************/
#if(FREERTOS_SCHE_ENABLE == 1)
#include <FreeRTOS.h>
#include <task.h>
/***************************************************************************************
 * taskENTER_CRITICAL
 * Macro to mark the start of a critical code region. 
 * Preemptive context switches cannot occur when in a critical region.
 * NOTE: This may alter the stack (depending on the portable implementation) so must be used with care!
 ***************************************************************************************
 * taskEXIT_CRITICAL
 * Macro to mark the end of a critical code region. 
 * Preemptive context switches cannot occur when in a critical region.
 * NOTE: This may alter the stack (depending on the portable implementation) so must be used with care!
 **************************************************************************************/
#   define cli()	taskENTER_CRITICAL()
#   define sti()	taskEXIT_CRITICAL()
#else
#include <define.h>
/**************************************************************************************
 * Current priority level is stored in bit<7:5> of Status Register (SR). 
 * Setting Interrupt Priority Level (IPL) to 7 disables all interrupts (except traps).
 **************************************************************************************/
#   define IPL      ( 0x00e0 )
#   define cli()    SR |= IPL       //Set IPL to 7
#   define sti()    SR &= ~IPL      //Set IPL to 0
#endif

/***************************************************************************************
 * DISI_PROTECT(X)
 * This MARCO does not belong to POSIX standard
 * This function is used to work around the silicon errata for dspic30 devices
 * When a low priority interrupt is negated by a high priority interrupt,
 * an AddressError will occur which will reset the target. 
 * To work around the problem, it is suggested by Microchip to protect  
 * 1) the clearing of Interrput Flag
 * 2) the disabling of Interrupt Enable
 * 3) the lowering of Interrupt Priority 
 * 4) the modification of IPL in Status Register to 1-6
 ***************************************************************************************/
#define DISI_PROTECT(X){ \
__asm__ volatile ("DISI #0x1FFF");\
X; \
DISICNT = 0; }

#endif /* SYSTEM_H_ */
