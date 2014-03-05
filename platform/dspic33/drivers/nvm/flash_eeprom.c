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
 * \remarks Copyright (c) 2013 Amonics for dsPic33F
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


static inline unsigned char
flash_eeprom_cal_checksum (void)
{
  unsigned char checksum = 0;
  int i;
  for (i = 0; i < FLASH_EEPROM_SIZE-1; i++)
    {
      checksum += image_eeprom[i];
    }
  return checksum;
}

//#define NVM_BACKUP

#ifdef NVM_BACKUP
#define NVM_PRI_COPY    0
#define NVM_SEC_COPY    1

// allocate space for secondary copy of NVM
unsigned char __attribute__((space(prog), aligned(1024))) flash_eeprom_data_copy[FLASH_EEPROM_SIZE];

/**
 * @brief check if NVM content is corrupted
 * @return 0 if not corrupt, 1 if corrupted
 * @remarks the NVM content MUST have been read into buffer image_eeprom[]
 */
static inline int
is_flash_eeprom_corrupted (void)
{
  if (flash_eeprom_cal_checksum () == image_eeprom[FLASH_EEPROM_SIZE-1]) return 0;
  else return 1;
}

/**
 * @brief copy NVM content from one page to another
 * @param src - source page, dest - destination page (NVM_PRI_COPY or NVM_SEC_COPY)
 */
static void
flash_eeprom_copy (int src, int dest)
{
  __u16 nvmAdru;
  __u16 nvmAdr;

  /* read source copy */
  if (src == NVM_PRI_COPY)
    {
      nvmAdru = __builtin_tblpage (flash_eeprom_data);
      nvmAdr  = __builtin_tbloffset (flash_eeprom_data);
    }
  else
    {
      nvmAdru = __builtin_tblpage (flash_eeprom_data_copy);
      nvmAdr  = __builtin_tbloffset (flash_eeprom_data_copy);
    }
#ifdef MPLAB_DSPIC33_PORT
  nvmAdr = nvmAdr & 0xFC00; // Get the Flash Page Aligned address
#endif /* MPLAB_DSPIC33_PORT */
  flashPageRead (nvmAdru, nvmAdr, (int*)image_eeprom);
  // source copy corrupted, don't copy
  if ( is_flash_eeprom_corrupted() ) return;

  /* recover destination copy */
  if (dest == NVM_PRI_COPY)
    {
      nvmAdru = __builtin_tblpage (flash_eeprom_data);
      nvmAdr  = __builtin_tbloffset (flash_eeprom_data);
    }
  else
    {
      nvmAdru = __builtin_tblpage (flash_eeprom_data_copy);
      nvmAdr  = __builtin_tbloffset (flash_eeprom_data_copy);
    }
#ifdef MPLAB_DSPIC33_PORT
  nvmAdr = nvmAdr & 0xFC00; // Get the Flash Page Aligned address
#endif /* MPLAB_DSPIC33_PORT */
  flashPageErase (nvmAdru, nvmAdr);
  flashPageWrite (nvmAdru, nvmAdr, (int*)image_eeprom);
}

/**
 * @brief perform NVM content validation at power up
 * @remarks if the NVM content primary copy is good, update the secondary copy accordingly
 * @remarks if the NVM content primary copy is corrupted, recover its content from the secondary copy and reset
 */
static void
flash_eeprom_init_check (void)
{
  flash_eeprom_busy = 1;

  /* read primary copy of NVM */
  __u16 nvmAdru = __builtin_tblpage (flash_eeprom_data);
  __u16 nvmAdr  = __builtin_tbloffset (flash_eeprom_data);
#ifdef MPLAB_DSPIC33_PORT
  nvmAdr = nvmAdr & 0xFC00; // Get the Flash Page Aligned address
#endif /* MPLAB_DSPIC33_PORT */
  flashPageRead (nvmAdru, nvmAdr, (int*)image_eeprom);
  if ( is_flash_eeprom_corrupted() )
    {
      /* primary copy is corrupted, recover from secondary copy and reset to take effect */
      flash_eeprom_copy (NVM_SEC_COPY, NVM_PRI_COPY);
      reset ();
    }
  else
    {
      /* primary copy is OK, backup to or initialise secondary copy */
      flash_eeprom_copy (NVM_PRI_COPY, NVM_SEC_COPY);
    }
  flash_eeprom_busy = 0;
}
#endif /* NVM_BACKUP */


int
flash_eeprom_open (int flags)
{
  flash_eeprom_io_flag = (__u8) flags;
#ifdef NVM_BACKUP
  flash_eeprom_init_check ();
#endif /* NVM_BACKUP */

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
          nvmAdr = nvmAdr & 0xFC00; // Get the Flash Page Aligned address
#endif /* MPLAB_DSPIC33_PORT */
          flashPageRead (nvmAdru, nvmAdr, (int*)image_eeprom);
#ifdef NVM_BACKUP
          if ( is_flash_eeprom_corrupted() )
            {
              /* primary copy is corrupted, recover from secondary copy */
              flash_eeprom_copy (NVM_SEC_COPY, NVM_PRI_COPY);
              flashPageRead (nvmAdru, nvmAdr, (int*)image_eeprom);
            }
#endif /* NVM_BACKUP */

          int i;           
          __u16 ptr = flash_eeprom_pointer;
          for (i = 0; (i < count) && (flash_eeprom_pointer < FLASH_EEPROM_SIZE-1); i++, flash_eeprom_pointer++)
            {
              image_eeprom[flash_eeprom_pointer] = buf[i];
            }
          image_eeprom[FLASH_EEPROM_SIZE-1] = flash_eeprom_cal_checksum ();

          flashPageErase (nvmAdru, nvmAdr);
          flashPageWrite (nvmAdru, nvmAdr, (int*)image_eeprom);

#ifdef NVM_BACKUP
          flashPageRead(nvmAdru, nvmAdr, (int*)image_eeprom);
          if ( is_flash_eeprom_corrupted() )
            {
              /* primary copy is corrupted, recover from secondary copy */
              flash_eeprom_copy (NVM_SEC_COPY, NVM_PRI_COPY);
              /* the write is consider fail, restore flash pointer and return error */
              flash_eeprom_pointer = ptr;
              errno = EAGAIN;
              flash_eeprom_busy = 0;
              i = -1;
            }
          else
            {
              /* primary copy is OK, backup to secondary copy */
              flash_eeprom_copy (NVM_PRI_COPY, NVM_SEC_COPY);
            }
#endif /* NVM_BACKUP */

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
          nvmAdr = nvmAdr & 0xFC00; // Get the Flash Page Aligned address
#endif /* MPLAB_DSPIC33_PORT */
          flashPageRead (nvmAdru, nvmAdr, (int*)image_eeprom);
#ifdef NVM_BACKUP
          if ( is_flash_eeprom_corrupted() )
            {
              /* primary copy is corrupted, recover from secondary copy */
              flash_eeprom_copy (NVM_SEC_COPY, NVM_PRI_COPY);
              flashPageRead (nvmAdru, nvmAdr, (int*)image_eeprom);
            }
#endif /* NVM_BACKUP */

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
