/**
 * \file
 * Interrupt handler when dsPic encounters fatal errors
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

#include <define.h>
#include <unistd.h>

/** variables to store program counter: order matters */
__u16 StkAddrLo;
__u16 StkAddrHi;


#ifdef DEBUG_TRAP_IN_NVM
/**
 * \brief handle trap error routine
 */
static void
handle_trap_error (__u8 err)
{
  //save error type
  if (lseek (fd_nvm, TRAP_ERR_SA, SEEK_SET) == TRAP_ERR_SA)
    {
      while (write (fd_nvm, &err, TRAP_ERR_LEN) != TRAP_ERR_LEN);
    }
  //save address
  if (lseek (fd_nvm, TRAP_ADDR_LO_SA, SEEK_SET) == TRAP_ADDR_LO_SA)
    {
      while (write (fd_nvm, &StkAddrLo, TRAP_ADDR_LO_LEN) != TRAP_ADDR_LO_LEN);
    }
  if (lseek (fd_nvm, TRAP_ADDR_HI_SA, SEEK_SET) == TRAP_ADDR_HI_SA)
    {
      while (write (fd_nvm, &StkAddrHi, TRAP_ADDR_HI_LEN) != TRAP_ADDR_HI_LEN);
    }
}
#endif /* DEBUG_TRAP_IN_NVM */


/**
 * \brief Oscillator Fail Interrupt
 */
void _TRAP_IRQ 
_OscillatorFail (void)
{
  _OSCFAIL = 0;
#ifdef DEBUG_TRAP_IN_NVM
  handle_trap_error ('O');
#endif /* DEBUG_TRAP_IN_NVM */
}


/**
 * \brief Address Error Interrupt
 */
void _TRAP_IRQ 
_AddressError (void)
{
  _ADDRERR = 0;
#ifdef DEBUG_TRAP_IN_NVM
  handle_trap_error ('A');
#endif /* DEBUG_TRAP_IN_NVM */
  
  while (1);
}


/**
 * \brief Stack Error Interrupt
 */
void _TRAP_IRQ 
_StackError (void)
{
  _STKERR = 0;
#ifdef DEBUG_TRAP_IN_NVM
  handle_trap_error ('S');
#endif /* DEBUG_TRAP_IN_NVM */
  
  while (1);
}


/**
 * \brief Math Interrupt
 */
void _TRAP_IRQ 
_MathError (void)
{
  _MATHERR = 0;
#ifdef DEBUG_TRAP_IN_NVM
  handle_trap_error ('M');
#endif /* DEBUG_TRAP_IN_NVM */

  while (1);
}

#ifdef MPLAB_DSPIC33_PORT
/**
 * \brief DMA Error Interrupt
 */
void _TRAP_IRQ 
_DMACError (void)
{
  INTCON1bits.DMACERR = 0;
#ifdef DEBUG_TRAP_IN_NVM
  handle_trap_error ('D');
#endif /* DEBUG_TRAP_IN_NVM */

  while (1);
}
#endif /* MPLAB_DSPIC33_PORT */
