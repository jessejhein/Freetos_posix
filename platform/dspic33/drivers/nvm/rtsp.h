/**
 * \file
 * RTSP (Code implemented by assembly)
 * \author Dennis Tsang <dennis@amonics.com>
 */

/***************************************************************************************
 * Concept for RTSP (Real Time Self Programming)
 * 
 * Flash must be ERASED in step of PAGE
 * Flash must be WRITTEN in step of ROW
 * 
 * For dsPic33: 
 *   Each PAGE has 8 ROWS of flash 
 *   Each ROW corresponds to 64 instructions or 192 bytes (high byte of upper word is phathom)
 **************************************************************************************/
 
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

#ifndef __RTSP_H__
#define __RTSP_H__ 

/************************************************************************
 * ===API Usage Examples===
 */ 
#if 0

void
foo (int* buf, int count)
{
  int image_buf[NVM_PAGE_SIZE];
  unsigned int nvmAdru = __builtin_tblpage (&nvm_data[0]);
  unsigned int nvmAdr  = __builtin_tbloffset (&nvm_data[0]);
#ifdef MPLAB_DSPIC33_PORT
  nvmAdr = nvmAdr & 0xFC00; // Get the Flash Page Aligned address
#endif

  flashPageRead (nvmAdru, nvmAdr, (int*)image_buf);
        
  for (i = 0; (i < count) && (nvm_pointer < NVM_SIZE); i++, nvm_pointer++)
    {
      image_buf[nvm_pointer] = buf[i];
    }

  flashPageErase (nvmAdru, nvmAdr);

  flashPageWrite (nvmAdru, nvmAdr, (int*)image_buf);
}

#endif
 /************************************************************************/


/**
 * \brief erase a page of flash specified by nvmAdru and nvmAdr
 * \param nvmAdru Selects the upper 8bits of the location to erase flash
 * \param nvmAdr Selects the location to erase in flash
 * \retval -1 error
 * \retval 0 OK
 * \remarks
 * \n nvmAdru = __builtin_tblpage(&array[0])
 * \n nvmAdr = __builtin_tbloffset(&array[0])
 * \n nvmAdr must be aligned to a page boundary
 */
extern int flashPageErase (unsigned int nvmAdru, unsigned int nvmAdr);


/**
 * \brief read a page of flash specified by nvmAdru and nvmAdr
 * \param nvmAdru Selects the upper 8bits of the location to read flash
 * \param nvmAdr Selects the location to read in flash
 * \param buf destination buffer
 * \retval -1 error
 * \retval 0 OK
 * \remarks
 * \n nvmAdru = __builtin_tblpage(&array[0])
 * \n nvmAdr = __builtin_tbloffset(&array[0])
 * \n nvmAdr must be aligned to a page boundary
 */
extern int flashPageRead (unsigned int nvmAdru, unsigned int nvmAdr, int *buf);


/**
 * \brief write a page of flash specified by nvmAdru and nvmAdr
 * \param nvmAdru Selects the upper 8bits of the location to write flash
 * \param nvmAdr Selects the location to write in flash
 * \param buf source buffer
 * \retval -1 error
 * \retval 0 OK
 * \remarks
 * \n nvmAdru = __builtin_tblpage(&array[0])
 * \n nvmAdr = __builtin_tbloffset(&array[0])
 * \n nvmAdr must be aligned to a page boundary
 */
extern int flashPageWrite (unsigned int nvmAdru, unsigned int nvmAdr, int *buf);

#endif /* __RTSP_H__ */

