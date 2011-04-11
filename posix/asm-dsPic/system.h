/**
 * \addtogroup posix POSIX
 * @{
 * 
 * Implementation of POSIX API using FreeRTOS API
 */

/**
 * \defgroup system System
 * @{
 * 
 * System routines
 */

/**
 * \file
 * System routines
 * \author Dennis Tsang <dennis@amonics.com>
 */

/*
 * Copyright (C) 2007-2011  Dennis Tsang <dennis@amonics.com>
 *
 * This file is part of freertos_posix.
 *
 * freertos_posix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * freertos_posix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with freertos_posix.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

/* interrupt priority bits */
#define IPL           ( 0x00e0 )

/**
 * \brief CLear Interrupt/Disable Interrupt
 * \remarks
 * Current priority level is stored in bit<7:5> of Status Register (SR). 
 * Setting Interrupt Priority Level (IPL) to 7 disables all interrupts (except traps).
 */
#define cli()         SR |= IPL       //Set IPL to 7


/**
 * \brief SeT Interrupt/Enable Interrupt
 * \remarks
 * Current priority level is stored in bit<7:5> of Status Register (SR). 
 * Setting Interrupt Priority Level (IPL) to 7 disables all interrupts (except traps).
 */
#define sti()         SR &= ~IPL      //Set IPL to 0


/***************************************************************************************
 * DISI_PROTECT(X)
 * This MARCO does not belong to POSIX standard
 * This function is used to work around the silicon errata for dspic30 devices
 * When a low priority interrupt is negated by a high priority interrupt,
 * an AddressError will occur which will reset the target. 
 * To work around the problem, it is suggested by Microchip to protect  
 * 1) the clearing of Interrupt Flag
 * 2) the disabling of Interrupt Enable
 * 3) the lowering of Interrupt Priority 
 * 4) the modification of IPL in Status Register to 1-6
 ***************************************************************************************/
#define DISI_PROTECT(X) \
{ \
  __asm__ volatile ("DISI #0x1FFF"); \
  X; \
  DISICNT = 0; \
}

#endif /* SYSTEM_H_ */

/** @} */
/** @} */
