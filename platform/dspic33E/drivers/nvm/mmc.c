/**
 * \file
 * MMC Card Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

/*
 * Copyright (C) 2012  Dennis Tsang <dennis@amonics.com>
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
#include <asm/delay.h>
#include <spi/spi.h>
#include "mmc.h"

//#define MMC_DEBUG                               1


#if (_VOLUMES > 0)
#define VOLUME_MMC                              0
#endif /* _VOLUMES > 0 */


/** keep disk status information */
static DSTATUS mmc_status = STA_NOINIT;
/** keep card type */
static UINT mmc_card_type;
/** store start time for timeout */
static clock_t timer = 0;


/**
 * \brief Wait for MMC to be ready
 * \retval 1 Ready
 * \retval 0 Timeout (500ms)
 * \remarks MMC is considered ready if reply is 0xFF
 */
static int
mmc_wait_ready (void)
{
  __u8 data;
  //timeout 500ms
  timer = clock ();
  do
    {
      data = spi_xchg (0xFF);
    }
  while ((data != 0xFF) && (((clock_t)(clock () - timer)) < 500));

  return (data == 0xFF) ? 1 : 0;
}


/**
 * \brief Deselect MMC by putting CS# = HIGH
 */
static void
mmc_deselect (void)
{
  spi_cs_deassert ();
  //Dummy clock (force DO to high-impedance for multiple slave SPI)
  spi_xchg (0xFF);
}


/**
 * \brief Select MMC by putting CS# = LOW
 * \retval 1 Successful
 * \retval 0 Timeout
 */
static int
mmc_select (void)
{
  spi_cs_assert ();
  //Dummy clock (force DO to high-impedance for multiple slave SPI)
  spi_xchg (0xFF);

  //OK
  if (mmc_wait_ready ()) return 1;
  //Timeout
  mmc_deselect ();
  return 0;
}


/**
 * \brief Send a command to the MMC card
 * \param cmd Command ID
 * \param arg Argument (Command Specific)
 * \retval 0xFF cannot select card
 * \remarks See section 7.3.1.1
 * \li All the SD Memory Card commands are 6 bytes long. Bit 47 = Start Bit [0], Bit 0 = End bit [1]
 */
static __u8
mmc_send_cmd (__u8 cmd, __u32 arg)
{
  __u8 n;       //use for CRC byte or counter
  __u8 ans;

  //ACMD<n> must be preceded by CMD55
  if (cmd & 0x80)
    {
      cmd &= 0x7F;
      ans = mmc_send_cmd (CMD55_APP_CMD, 0);
      if (ans > 1) return ans;
    }

  //Select the card and wait for ready
  mmc_deselect ();
  if (!mmc_select ()) return 0xFF;

  //Send command packet
  spi_xchg (0x40 | cmd);                                //Start + Command index
  spi_xchg ((__u8)(arg >> 24));                         //Argument[31..24]
  spi_xchg ((__u8)(arg >> 16));                         //Argument[23..16]
  spi_xchg ((__u8)(arg >> 8));                          //Argument[15..8]
  spi_xchg ((__u8)arg);                                 //Argument[7..0]
  n = 0x01;                                             //Dummy CRC + Stop
  if (cmd == CMD0_GO_IDLE_STATE) n = 0x95;              //Valid CRC for CMD0(0) + Stop (See section 7.2.2)
  if (cmd == CMD8_SEND_IF_COND) n = 0x87;               //Valid CRC for CMD8(0x1AA) + Stop
  spi_xchg (n);

  //Receive command response
  if (cmd == CMD12_STOP_TRANSMISSION) spi_xchg (0xFF);  //Skip a stuff byte on stop to read
  //Wait for a valid response in timeout of 10 attempts
  n = 10;
  do
    {
      ans = spi_xchg (0xFF);
    }
  while ((ans & 0x80) && --n);

  //Return with the response value
  return ans;
}


/**
 * /brief Read Data Block
 * /param buf Data buffer to store received data
 * /param count Byte count (must be multiple of 4)
 * /retval 1 OK
 * /retval 0 Fail
 */
static int
mmc_read_datablock (__u8* buf, __u16 count)
{
  __u8 token;

  //timeout 100ms
  timer = clock ();

  //Wait for data packet in timeout of 100ms
  do
    {
      token = spi_xchg (0xFF);
    }
  while ((token == 0xFF) && (((clock_t)(clock () - timer)) < 100));

  //If not valid data token, return with error
  if (token != 0xFE) return 0;

  //Receive the data block into buffer
  spi_read (buf, count);
  //Discard CRC
  spi_xchg (0xFF);
  spi_xchg (0xFF);

  //Return with success
  return 1;
}


/**
 * /brief Write Data Block
 * /param buff 512 byte data block to be transmitted
 * /param token Data token
 * /retval 1 OK
 * /retval 0 Fail
 */
static int
mmc_write_datablock (const __u8* buf, __u8 token)
{
  __u8 ans;
  if (!mmc_wait_ready ()) return 0;

  //Send a token
  spi_xchg (token);

  //Not StopTran token
  if (token != 0xFD)
    {
      //Transmit the data block to the MMC
      spi_write (buf, 512);
      //Discard CRC
      spi_xchg (0xFF);
      spi_xchg (0xFF);

      //Receive a data response
      ans = spi_xchg (0xFF);
      //If not accepted, return with error
      if ((ans & 0x1F) != 0x05)
        return 0;
    }
  return 1;
}


//--------------------------------------------------------------------------------------------------------


void
mmc_check_card_insert (void)
{
  if (mmc_card_detect ())
    mmc_status &= ~STA_NODISK;
  else
    mmc_status |= (STA_NODISK | STA_NOINIT);
}


DSTATUS
disk_initialize (BYTE drv)
{
  switch (drv)
    {
      case VOLUME_MMC:
        {
          BYTE n, cmd, ty, ocr[4];
          //No card in the socket
          if (mmc_status & STA_NODISK) return mmc_status;

          spi_open ();
          // Set initial preferred speed to maximum driver strength: 400kHz
#ifdef MMC_DEBUG
          __u32 speed =
#endif /* MMC_DEBUG */
          spi_set_clk_speed (400000UL);
#ifdef MMC_DEBUG
          printf ("[disk init] SPI CLK SPEED = %ld\n", speed);
#endif /* MMC_DEBUG */

          //80 dummy clocks
          for (n = 10; n; n--) spi_xchg (0xFF);

          ty = 0;
          //Enter Idle state
          if (mmc_send_cmd (CMD0_GO_IDLE_STATE, 0) == 1)
            {
              //timeout is 1000ms
              timer = clock ();

              //Use CMD 8 to check SD version
              //Section 7.3.2.6
              //This response token is sent by the card when a SEND_IF_COND command (CMD8) is received.
              //The response length is 5 bytes. The structure of the first (MSB) byte is identical to response type R1.
              //The other four bytes contain the card operating voltage information and echo back of check pattern in
              //argument and are specified by the same definition as R7 response in SD mode. (Refer to Section 4.9).
              // Bit   39    32 31    28 27    12 11     8 7      0
              //      |0       |        |        |        |        |
              //         R1     cmd ver  reserved voltage echo-back
              if (mmc_send_cmd (CMD8_SEND_IF_COND, 0x1AA) == 1)
                {
                  //Get trailing return value of R7 response
                  for (n = 0; n < 4; n++) ocr[n] = spi_xchg (0xFF);
                  //Check whether the card can work at vdd range of 2.7 - 3.6V
                  if (ocr[2] == 0x01 && ocr[3] == 0xAA)
                    {
                      //Wait for leaving idle state (ACMD41 with HCS bit(Bit30): 0 means Ready; 1 means Busy)
                      while ( (((clock_t)(clock() - timer)) < 1000) && mmc_send_cmd (ACMD41_SD_SEND_OP_COND, 0x40000000) );

                      //Check CCS bit in the OCR
                      if ( (((clock_t)(clock() - timer)) < 1000) && mmc_send_cmd (CMD58_READ_OCR, 0) == 0)
                        {
                          //CCS=0 means that the card is SDSC. CCS=1 means that the card is SDHC or SDXC.
                          for (n = 0; n < 4; n++) ocr[n] = spi_xchg (0xFF);
                          ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;
#ifdef MMC_DEBUG
                          if (ocr[0] & 0x40) printf ("[disk init] MMC Card Type = SDv2 (SDHC/SDXC)\n");
                          else printf ("[disk init] MMC Card Type = SDv2 (SDSC)\n");
#endif /* MMC_DEBUG */
                        }
                    }
                }
              //No response to CMD8
              //Use ACMD41 (arg = 0x0) to check for SDv1
              else
                {
                  //0 means Ready; 1 means Busy
                  if (mmc_send_cmd (ACMD41_SD_SEND_OP_COND, 0) <= 1)
                    {
                      ty = CT_SD1;
                      cmd = ACMD41_SD_SEND_OP_COND;
#ifdef MMC_DEBUG
                      printf ("[disk init] MMC Card Type = SDv1\n");
#endif /* MMC_DEBUG */
                    }
                  else
                    {
                      ty = CT_MMC;
                      cmd = CMD1_SEND_OP_COND;
#ifdef MMC_DEBUG
                      printf ("[disk init] MMC Card Type = SDv3\n");
#endif /* MMC_DEBUG */
                    }

                  //Wait for leaving idle state
                  while ((((clock_t)(clock() - timer)) < 1000) && mmc_send_cmd (cmd, 0));
                  //Set read/write block length to 512 (for SDSC)
                  //SDHC and SDXC Cards only support 512-byte block length
                  if (!(((clock_t)(clock() - timer)) < 1000) || mmc_send_cmd (CMD16_SET_BLOCK_LEN, 512) != 0)
                    ty = 0;
                }
            }
          mmc_card_type = ty;
          mmc_deselect ();

          //Initialisation succeeded
          if (ty)
            {
              mmc_status &= ~STA_NOINIT;
              //maximum throughput 25MHz
#ifdef MMC_DEBUG
              speed =
#endif /* MMC_DEBUG */
              spi_set_clk_speed (2000000UL);
#ifdef MMC_DEBUG
              printf ("[disk init] SPI CLK SPEED = %ld\n", speed);
#endif /* MMC_DEBUG */
            }
          //Initialisation failed
          else
            {
              spi_close ();
              mmc_status |= STA_NOINIT;
            }
          return mmc_status;
        }
    }
  return STA_NOINIT;
}


DSTATUS
disk_status (BYTE drv)
{
  switch (drv)
    {
      case VOLUME_MMC:
        {
          return mmc_status;
        }
    }
  return STA_NOINIT;
}


DRESULT
disk_read (BYTE drv, BYTE *buf, DWORD sector, BYTE count)
{
  switch (drv)
    {
      case VOLUME_MMC:
        {
          if (!count) return RES_PARERR;
          if (mmc_status & STA_NOINIT) return RES_NOTRDY;

          //Convert to byte address if needed
          if (!(mmc_card_type & CT_BLOCK)) sector *= 512;

          //Read Single Block
          if (count == 1)
            {
              if ((mmc_send_cmd (CMD17_READ_SINGLE_BLOCK, sector) == 0) && mmc_read_datablock (buf, 512))
                count = 0;
#ifdef MMC_DEBUG
              printf ("[disk read] Single Block Read: Sector = %ld\n", sector);
#endif /* MMC_DEBUG */
            }
          //Read Multiple Block
          else
            {
              if (mmc_send_cmd (CMD18_READ_MULTIPLE_BLOCKS, sector) == 0)
                {
                  do
                    {
                      if (!mmc_read_datablock (buf, 512)) break;
                      buf += 512;
                    }
                  while (--count);
                  //stop transmission
                  mmc_send_cmd (CMD12_STOP_TRANSMISSION, 0);
#ifdef MMC_DEBUG
                  printf ("[disk read] Multiple Block Read: From sector = %ld\n", sector);
#endif /* MMC_DEBUG */
                }
            }
          mmc_deselect ();

          return count ? RES_ERROR : RES_OK;
        }
    }
  return RES_PARERR;
}


DRESULT
disk_write (BYTE drv, const BYTE *buf, DWORD sector, BYTE count)
{
  switch (drv)
    {
      case VOLUME_MMC:
        {
          if (!count) return RES_PARERR;
          if (mmc_status & STA_NOINIT) return RES_NOTRDY;
          if (mmc_status & STA_PROTECT) return RES_WRPRT;

          //Convert to byte address if needed
          if (!(mmc_card_type & CT_BLOCK)) sector *= 512;

          //Write Single Block
          if (count == 1)
            {
              if ( (mmc_send_cmd (CMD24_WRITE_SINGLE_BLOCK, sector) == 0) && mmc_write_datablock (buf, 0xFE) )
                count = 0;
#ifdef MMC_DEBUG
              printf ("[disk write] Single Block Write: Sector = %ld\n", sector);
#endif /* MMC_DEBUG */
            }
          //Write Multiple Block
          else
            {
              if (mmc_card_type & CT_SDC) mmc_send_cmd (ACMD23_SET_WR_BLK_ERASE_COUNT, count);
              if (mmc_send_cmd (CMD25_WRITE_MULTIPLE_BLOCKS, sector) == 0)
                {
                  do
                    {
                      if (!mmc_write_datablock (buf, 0xFC)) break;
                      buf += 512;
                    }
                  while (--count);
                  //STOP_TRAN token
                  if (!mmc_write_datablock (0, 0xFD))
                    count = 1;
                }
#ifdef MMC_DEBUG
                  printf ("[disk write] Multiple Block Written: From sector = %ld\n", sector);
#endif /* MMC_DEBUG */
            }
          mmc_deselect ();
          return count ? RES_ERROR : RES_OK;
        }
    }
  return RES_PARERR;
}


DRESULT
disk_ioctl (BYTE drv, BYTE cmd, void* buf)
{
  DRESULT res;
  DWORD csz;
  __u8 n, csd[16], *ptr = buf;

  switch (drv)
    {
      case VOLUME_MMC:
        {
          if (mmc_status & STA_NOINIT) return RES_NOTRDY;
          res = RES_ERROR;

          switch (cmd)
            {
              //Make sure that the disk drive has finished pending write process.
              //When the disk I/O module has a write back cache, flush the dirty sector immediately.
              //This command is not used in read-only configuration.
              /**
               * This command is called during fflush() or fclose()
               */
              case CTRL_SYNC:
                {
                  if (mmc_select ())
                    {
                      mmc_deselect ();
                      res = RES_OK;
                    }
#ifdef MMC_DEBUG
                  printf ("[disk ioctl] Control Sync\n");
#endif /* MMC_DEBUG */
                  break;
                }
              //Returns number of available sectors on the drive into the DWORD variable pointed by Buffer.
              //This command is used by only f_mkfs function to determine the volume size to be created.
              case GET_SECTOR_COUNT:
                {
                  //Get number of sectors on the disk (WORD)
                  if ((mmc_send_cmd (CMD9_SEND_CSD, 0) == 0) && mmc_read_datablock (csd, 16))
                    {
                      if ((csd[0] >> 6) == 1)
                        {
                          //SDv2?
                          csz = csd[9] + ((WORD)csd[8] << 8) + ((DWORD)(csd[7] & 63) << 16) + 1;
                          *(DWORD*)buf = csz << 10;
                        }
                      else
                        {
                          //SDv1 or MMCv3
                          n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
                          csz = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
                          *(DWORD*)buf = csz << (n - 9);
                        }
                      res = RES_OK;
                    }
#ifdef MMC_DEBUG
                  printf ("[disk ioctl] Get Sector Count = %ld\n", *(DWORD*)buf);
#endif /* MMC_DEBUG */
                  break;
                }
              //Returns sector size of the drive into the WORD variable pointed by Buffer.
              //This command is not used in fixed sector size configuration, _MAX_SS is 512.
              case GET_SECTOR_SIZE:
                {
                  //Get sectors on the disk (WORD)
                  *(WORD*)buf = 512;
                  res = RES_OK;
#ifdef MMC_DEBUG
                  printf ("[disk ioctl] Get Sector Size = %d\n", *(WORD*)buf);
#endif /* MMC_DEBUG */
                  break;
                }
              //Returns erase block size of the flash memory in unit of sector into the DWORD variable pointed by Buffer.
              //The allowable value is 1 to 32768 in power of 2.
              //Return 1 if the erase block size is unknown or disk devices.
              //This command is used by only f_mkfs function and it attempts to align data area to the erase block boundary.
              case GET_BLOCK_SIZE :
                {
                  //Get erase block size in unit of sectors (DWORD)
                  if (mmc_card_type & CT_SD2)
                    {
                      //SDv2?
                      if (mmc_send_cmd (ACMD13_SD_STATUS, 0) == 0)
                        {
                          //Read SD status
                          spi_xchg (0xFF);
                          if (mmc_read_datablock (csd, 16))
                            {
                              //Read partial block
                              for (n = 64 - 16; n; n--) spi_xchg (0xFF);       //Purge trailing data
                              *(DWORD*)buf = 16UL << (csd[10] >> 4);
                              res = RES_OK;
                            }
                        }
                    }
                  else
                    {
                      //SDv1 or MMCv3
                      if ((mmc_send_cmd (CMD9_SEND_CSD, 0) == 0) && mmc_read_datablock (csd, 16))
                        {
                          //Read CSD
                          if (mmc_card_type & CT_SD1)
                            {
                              //SDv1
                              *(DWORD*)buf = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
                            }
                          else
                            {
                              //MMCv3
                              *(DWORD*)buf = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
                            }
                          res = RES_OK;
                        }
                    }
#ifdef MMC_DEBUG
                  printf ("[disk ioctl] Get Block Size = %ld\n", *(DWORD*)buf);
#endif /* MMC_DEBUG */
                  break;
                }
              //Get card type flags (1 byte)
              case MMC_GET_TYPE:
                {
                  *ptr = mmc_card_type;
                  res = RES_OK;
#ifdef MMC_DEBUG
                  printf ("[disk ioctl] Get Card Type = %d\n", (int)*ptr);
#endif /* MMC_DEBUG */
                  break;
                }
              //Receive CSD as a data block (16 bytes)
              case MMC_GET_CSD:
                {
                  //READ_CSD
                  if ((mmc_send_cmd (CMD9_SEND_CSD, 0) == 0) && mmc_read_datablock (buf, 16))
                    res = RES_OK;
#ifdef MMC_DEBUG
                  printf ("[disk ioctl] Get CSD\n");
#endif /* MMC_DEBUG */
                  break;
                }
              //Receive CID as a data block (16 bytes)
              case MMC_GET_CID:
                {
                  //READ_CID
                  if ((mmc_send_cmd (CMD10_SEND_CID, 0) == 0) && mmc_read_datablock (buf, 16))
                    res = RES_OK;
#ifdef MMC_DEBUG
                  printf ("[disk ioctl] Get CID\n");
#endif /* MMC_DEBUG */
                  break;
                }
              //Receive OCR as an R3 response (4 bytes)
              case MMC_GET_OCR:
                {
                  if (mmc_send_cmd (CMD58_READ_OCR, 0) == 0)
                    {
                      //READ_OCR
                      for (n = 0; n < 4; n++)
                        *((BYTE*)buf + n) = spi_xchg (0xFF);
                      res = RES_OK;
                    }
#ifdef MMC_DEBUG
                  printf ("[disk ioctl] Get OCR\n");
#endif /* MMC_DEBUG */
                  break;
                }
              //Receive SD status as a data block (64 bytes)
              case MMC_GET_SDSTAT:
                {
                  if ((mmc_card_type & CT_SD2) && mmc_send_cmd (ACMD13_SD_STATUS, 0) == 0)
                    {
                      //SD_STATUS
                      spi_xchg (0xFF);
                      if (mmc_read_datablock (buf, 64))
                        res = RES_OK;
                    }
#ifdef MMC_DEBUG
                  printf ("[disk ioctl] Get SD Status\n");
#endif /* MMC_DEBUG */
                  break;
                }
              default:
                {
                  res = RES_PARERR;
                }
            }
            mmc_deselect ();
            return res;
        }
    }
  return RES_PARERR;
}


DWORD
get_fattime (void)
{
  //1980/01/01 00:00:00
  //DWORD res = 0x00210000;

  //return an invalid time, assume no real time clock support
  return 0;
}

