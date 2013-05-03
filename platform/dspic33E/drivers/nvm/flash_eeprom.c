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
 * \author Sam Hui <dennis@amonics.com>
 * \remarks Copyright (c) 2013 Amonics for dsPic33E
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
#include <fcntl.h>
#include <errno.h>
#include <nvm_data.h>
#include "rtsp.h"

/** temporary storage for read/write operation */
/*static*/ __u8 image_eeprom[FLASH_EEPROM_SIZE];
/** store IO setting */
static __u8 flash_eeprom_io_flag;
/** indicate EEPROM module is busy */
static __u8 flash_eeprom_busy = 0;
/** store pointer to EEPROM */
static __u16 flash_eeprom_pointer = 0;


int 
flash_eeprom_open (int flags)
{
  flash_eeprom_io_flag = (__u8) flags;
  return 0;
}


int 
flash_eeprom_write (__u8* buf, int count)
{
  //Perform Write if write operation is enabled    
  if ((flash_eeprom_io_flag & O_RDWR) || (flash_eeprom_io_flag & O_WRONLY))
    {
      if (flash_eeprom_busy == 1)
        {
          errno = EAGAIN;
          return -1;
        }
      else
        {
          flash_eeprom_busy = 1;

          __u16 nvmAdru = __builtin_tblpage (&flash_eeprom_data[0]);
          __u16 nvmAdr  = __builtin_tbloffset (&flash_eeprom_data[0]);
#ifdef MPLAB_DSPIC33_PORT
          //page starts at 2Kwords boundary, i.e. 0x0000, 0x0800, 0x1000, 0x1800 and etc for 33E
          nvmAdr = nvmAdr & 0xF800; // Get the Flash Page Aligned address
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
  //IO not opened for writing
  else
    {
      errno = EBADF;
      return -1;
    }    
}


int 
flash_eeprom_read (__u8* buf, int count)
{
  //Perform Read if read operation is enabled
  if ((flash_eeprom_io_flag & O_RDWR) || !(flash_eeprom_io_flag & O_WRONLY))
    {
      if (flash_eeprom_busy == 1)
        {
          errno = EAGAIN;
          return -1;
        }
      else
        {
          flash_eeprom_busy = 1;

          __u16 nvmAdru = __builtin_tblpage (&flash_eeprom_data[0]);
          __u16 nvmAdr  = __builtin_tbloffset (&flash_eeprom_data[0]);
#ifdef MPLAB_DSPIC33_PORT
          //page starts at 2Kwords boundary, i.e. 0x0000, 0x0800, 0x1000, 0x1800 and etc for 33E
          nvmAdr = nvmAdr & 0xF800; // Get the Flash Page Aligned address
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
  //IO not opened for reading
  else
    {
      errno = EBADF;
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
