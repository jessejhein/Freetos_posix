/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup flash_eeprom Flash-emulated EEPROM
 * @{
 */

/**
 * \file
 * Flash-emulated EEPROM Driver
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
#include <nvm_data.h>
#include "rtsp.h"
#include <fcntl.h>
#include <errno.h>

/** store pointer to EEPROM */
static unsigned int flash_eeprom_pointer = 0;
/** indicate EEPROM module is busy */
static unsigned int flash_eeprom_busy = 0;
/** store io setting */
static int flash_eeprom_io_flag;
/** temporary storage for read/write operation */
static unsigned char image_eeprom[FLASH_EEPROM_SIZE];


int 
flash_eeprom_open (int flags)
{
  flash_eeprom_io_flag = flags;
  return 0;
}


int 
flash_eeprom_write (unsigned char* buf, int count)
{
  //Perform Write if write operation is enabled    
  if (flash_eeprom_io_flag & O_RDWR || flash_eeprom_io_flag & O_WRONLY)
    {
      if (flash_eeprom_busy == 1)
        {
          errno = EAGAIN;
          return -1;
        }
      else
        {
          flash_eeprom_busy = 1;
            
          unsigned int nvmAdru = __builtin_tblpage (&flash_eeprom_data[0]);
          unsigned int nvmAdr  = __builtin_tbloffset (&flash_eeprom_data[0]);
#ifdef MPLAB_DSPIC33_PORT
          nvmAdr = nvmAdr & 0xFC00; // Get the Flash Page Aligned address
#endif /* MPLAB_DSPIC33_PORT */
            
          flashPageRead (nvmAdru, nvmAdr, (int*)image_eeprom);

          int i;           
          for (i = 0; (i < count) && (flash_eeprom_pointer < FLASH_EEPROM_SIZE); i++, flash_eeprom_pointer++)
            {
              image_eeprom[flash_eeprom_pointer] = buf[i];
            }
            
          flashPageErase (nvmAdru, nvmAdr);
          flashPageWrite (nvmAdru, nvmAdr, (int*)image_eeprom);
          flash_eeprom_busy = 0;
        
          return i;
        }
    }
  //Error, raise error flag
  else
    {
      errno = EBADF;  //io not opened for writing
      return -1;
    }    
}


int 
flash_eeprom_read (unsigned char* buf, int count)
{
  //Perform Read if read operation is enabled
  if (flash_eeprom_io_flag & O_RDWR || flash_eeprom_io_flag & O_RDONLY)
    {
      if (flash_eeprom_busy == 1)
        {
          errno = EAGAIN;
          return -1;
        }
      else
        {
          flash_eeprom_busy = 1;

          unsigned int nvmAdru = __builtin_tblpage (&flash_eeprom_data[0]);
          unsigned int nvmAdr  = __builtin_tbloffset (&flash_eeprom_data[0]);
#ifdef MPLAB_DSPIC33_PORT
          nvmAdr = nvmAdr & 0xFC00; // Get the Flash Page Aligned address
#endif /* MPLAB_DSPIC33_PORT */
          flashPageRead (nvmAdru, nvmAdr, (int*)image_eeprom);
            
          int i;
          for (i = 0; (i < count) && (flash_eeprom_pointer < FLASH_EEPROM_SIZE); i++, flash_eeprom_pointer++)
            {
              buf[i] = image_eeprom[flash_eeprom_pointer];
            }
            
          flash_eeprom_busy = 0;   
          return i;
        }
    }
  //Error, raise error flag
  else
    {
      errno = EBADF;  //io not opened for writing
      return -1;
    }
}


int 
flash_eeprom_lseek (int offset, int whence)
{
  if (flash_eeprom_busy > 0)
    {
      errno = EAGAIN;
      return -1;
    }

  flash_eeprom_pointer = offset;
  return flash_eeprom_pointer;
}

/** @} */
/** @} */
