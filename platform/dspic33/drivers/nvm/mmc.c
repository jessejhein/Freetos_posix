/**
 * \addtogroup drivers Drivers
 * @{
 *
 * Implementation of Drivers for FatFs
 */

/**
 * \defgroup file_system File System
 * @{
 * Contains
 * \li middle layer [disk_xxx()] interfacing hardware layer
 */

/**
 * \file
 * Low level disk I/O module skeleton for FatFs
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

#include <fcntl.h>
#include <stdio.h>
#include <diskio.h>
#include <ff.h>

#if (_VOLUMES > 0)
#define VOLUME_MMC                              0
#endif /* _VOLUMES > 0 */

static int mmc_is_opened = 0;

/**
 * \brief Initialise disk drive
 * \param drv
 * \return
 */
DSTATUS
disk_initialize (BYTE drv)
{
  switch (drv)
    {
      case VOLUME_MMC:
        {
          if (mmc_is_opened == 0)
            {
              mmc_is_opened = 1;
              return 0;
            }
          return 0;
        }
    }
  return STA_NOINIT;
}


/**
 * \brief Get disk status
 * \param drv
 * \return
 */
DSTATUS
disk_status (BYTE drv)
{
  switch (drv)
    {
      case VOLUME_MMC:
        {
          if (mmc_is_opened != 0) return 0;
          break;
        }
    }
  return STA_NOINIT;
}


/**
 * \brief Read sector(s)
 * \param drv
 * \param buff
 * \param sector
 * \param count
 * \return
 */
DRESULT
disk_read (BYTE drv, BYTE *buff, DWORD sector, BYTE count)
{
  switch (drv)
    {
      case VOLUME_MMC:
        {
          return RES_OK;
        }
    }
  return RES_PARERR;
}


/**
 * \brief Write sector(s)
 * \param drv
 * \param buff
 * \param sector
 * \param count
 * \return
 */
DRESULT
disk_write (BYTE drv, const BYTE *buff, DWORD sector, BYTE count)
{
  switch (drv)
    {
      case VOLUME_MMC:
        {
          return RES_OK;
        }
    }
  return RES_PARERR;
}


/**
 * \brief Control device dependent features
 * \param drv
 * \param ctrl
 * \param buff
 * \return
 */
DRESULT
disk_ioctl (BYTE drv, BYTE ctrl, void *buff)
{
  switch (drv)
    {
      case VOLUME_MMC:
        {
          switch (ctrl)
            {
              case CTRL_SYNC: break;
              case GET_SECTOR_SIZE:
                {
                  //*((WORD*)(buff)) = ;
                  break;
                }
              case GET_SECTOR_COUNT:
                {
                  //*((DWORD*)(buff)) = (DWORD) (I2C_EEPROM_SIZE / FATFS_SECTOR_SIZE);
                  break;
                }
              case GET_BLOCK_SIZE:
                {
                  *((DWORD*)(buff)) = (DWORD) 1;
                  break;
                }
              default:
                return RES_PARERR;
            }
          return RES_OK;
        }
    }
  return RES_PARERR;
}


/**
 * \brief Get current time
 * \return
 * \remarks
 * \verbatim
    bit31:25
      Year from 1980 (0..127)
    bit24:21
      Month (1..12)
    bit20:16
      Day in month(1..31)
    bit15:11
      Hour (0..23)
    bit10:5
      Minute (0..59)
    bit4:0
      Second / 2 (0..29)
    \endverbatim
 */
DWORD
get_fattime (void)
{
  //1980/01/01 00:00:00
  //DWORD res = 0x00210000;

  //return an invalid time, assume no real time clock support
  return 0;
}


/** @} */
/** @} */
