/**
 * \file
 * SD Card Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

/*
 * Copyright (C) 2011  Dennis Tsang <dennis@amonics.com>
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
#include "sd.h"

#if (_VOLUMES > 0)
#define VOLUME_MMC                              0
#endif /* _VOLUMES > 0 */


static int mmc_is_opened = 0;
static __u8 SDHC_flag;
static __u8 cardType;
static __u8 mmc_buffer[_MAX_SS];


/**
 * \brief to send a command to SD card
 * \param cmd 8-bit command value
 * \param arg 32-bit command argument
 * \return response byte
 */
static __u8
sd_sendCommand (__u8 cmd, __u32 arg)
{
  __u8 response, retry = 0, status;

  //SD card accepts byte address while SDHC accepts block address in multiples of 512
  //so, if it's SD card we need to convert block address into corresponding byte address by
  //multiplying it with 512. which is equivalent to shifting it left 9 times
  //following 'if' loop does that
  if (SDHC_flag == 0)
    {
      if( cmd == READ_SINGLE_BLOCK     ||
          cmd == READ_MULTIPLE_BLOCKS  ||
          cmd == WRITE_SINGLE_BLOCK    ||
          cmd == WRITE_MULTIPLE_BLOCKS ||
          cmd == ERASE_BLOCK_START_ADDR||
          cmd == ERASE_BLOCK_END_ADDR )
        {
          arg = arg << 9;
        }
    }

  spi_cs_assert ();

  // SD Card Command Format
  // Frame 7 = 0
  // Frame 6 = 1
  // Command (6 bits)
  // Address (32 bits)
  // Frame 0 = 1
  spi_send_byte (cmd | 0x40); //send command, first two bits always '01'
  spi_send_byte (arg >> 24);
  spi_send_byte (arg >> 16);
  spi_send_byte (arg >> 8);
  spi_send_byte (arg);

  //it is compulsory to send correct CRC for CMD8 (CRC=0x87) & CMD0 (CRC=0x95)
  //for remaining commands, CRC is ignored in SPI mode
  if (cmd == SEND_IF_COND)
    {
      spi_send_byte (0x87);
    }
  else
    {
      spi_send_byte (0x95);
    }

  //wait response
  while ((response = spi_rcv_byte ()) == 0xFF)
    {
      if (retry++ > 0xfe)
        {
          break;         //time out error
        }
    }

  if (response == 0x00 && cmd == 58) //checking response of CMD58
    {
      status = spi_rcv_byte() & 0x40;     //first byte of the OCR register (bit 31:24)
      if (status == 0x40) SDHC_flag = 1;  //we need it to verify SDHC card
      else SDHC_flag = 0;
      spi_rcv_byte (); //remaining 3 bytes of the OCR register are ignored here
      spi_rcv_byte (); //one can use these bytes to check power supply limits of SD
      spi_rcv_byte ();
    }

  spi_cs_deassert ();

  // Following any command, the SD Card needs 8 clocks to finish up its work.
  spi_send_byte (0xFF);

  mdelay (2);

  return response;
}


/**
 * \brief Initialise the SD/SDHC card in SPI mode
 * \return 0 if no error, otherwise the response byte will be sent
 */
static __u8
sd_init (void)
{
  __u8 i, response, SD_version;
  __u16 retry = 0;

  //80 clock pulses spent before sending the first command
  for (i = 0; i < 16; i++)
    {
      spi_send_byte (0xFF);
    }

  spi_cs_assert ();

  //Set SD Card to Idle State
  do
    {
      response = sd_sendCommand (GO_IDLE_STATE, 0);
      retry++;
      if (retry > 0xFE)
        {
          return 1;   //time out, card not detected
        }
    }
  while (response != 0x01);

  spi_send_byte (0xFF);
  spi_send_byte (0xFF);

  //default set to SD compliance with ver2.x;
  //this may change after checking the next command
  SD_version = 2;
  retry = 0;

  //Check power supply status, mandatory for SDHC card
  do
    {
      response = sd_sendCommand (SEND_IF_COND, 0x000001AA);
      retry++;
      if (retry > 0xfe)
        {
          SD_version = 1;
          cardType = 1;
          break;
        }
    }
  while (response != 0x01);
  retry = 0;

  do
    {
      response = sd_sendCommand (APP_CMD,0);                            //CMD55, must be sent before sending any ACMD command
      response = sd_sendCommand (SD_SEND_OP_COND, 0x40000000);          //ACMD41
      retry++;
      if (retry > 0xFE)
        {
          return 2;  //time out, card initialisation failed
        }
    }
  while (response != 0x00);
  retry = 0;
  SDHC_flag = 0;

  if (SD_version == 2)
    {
      do
        {
          response = sd_sendCommand (READ_OCR, 0);
          retry++;
          if (retry > 0xFE)
            {
              cardType = 0;//time out
              break;
            }
        }
      while (response != 0x00);
      if (SDHC_flag == 1)
        {
          cardType = 2;
        }
      else
        {
          cardType = 3;
        }
    }

  sd_sendCommand (CRC_ON_OFF, OFF);             //disable CRC; default - CRC disabled in SPI mode
  sd_sendCommand (SET_BLOCK_LEN, _MAX_SS);      //set block size to 512; default size is 512
  return 0;
}


/**
 * \brief to erase specified no. of blocks of SD card
 * \return unsigned char; will be 0 if no error,otherwise the response byte will be sent
 * \remarks not tested
 */
static __u8
sd_erase (__u32 startBlock, __u32 totalBlocks)
{
  __u8 response;
  response = sd_sendCommand (ERASE_BLOCK_START_ADDR, startBlock); //send starting block address
  if (response != 0x00) //check for SD status: 0x00 - OK (No flags set)
    return response;

  response = sd_sendCommand (ERASE_BLOCK_END_ADDR,(startBlock + totalBlocks - 1)); //send end block address
  if(response != 0x00)
    return response;

  response = sd_sendCommand (ERASE_SELECTED_BLOCKS, 0); //erase all selected blocks
  if (response != 0x00)
    return response;

  return 0; //normal return
}


/**
 * \brief to read a single block from SD card
 * \return unsigned char; will be 0 if no error,otherwise the response byte will be sent
 */
__u8
sd_readSingleBlock (__u32 startBlock)
{
  __u8 response;
  __u16 i, retry = 0;

  response = sd_sendCommand (READ_SINGLE_BLOCK, startBlock); //read a Block command
  if (response != 0x00) //check for SD status: 0x00 - OK (No flags set)
                        //reason maybe supply voltage not stable
    return response;

  spi_cs_assert ();

  retry = 0;
  while (spi_rcv_byte() != 0xfe)  //wait for start block token 0xfe (0x11111110)
    if (retry++ > 0xfffe)
      {
        spi_cs_deassert ();         //return if time-out
        return 1;
      }

  //read 512 bytes
  for (i = 0; i < _MAX_SS; i++)
    {
      mmc_buffer[i] = spi_rcv_byte ();
    }

  //receive incoming CRC (16-bit), CRC is ignored here
  spi_rcv_byte ();
  spi_rcv_byte ();
  //extra 8 clock pulses
  spi_rcv_byte ();

  spi_cs_deassert ();

  mdelay (2);

  return 0;
}

/**
 * \brief to write to a single block of SD card
 * \return unsigned char; will be 0 if no error,otherwise the response byte will be sent
 * \remarks not fully debug
 * \remarks error also can write the data into SD card,but can't run to IO control function
 */
__u8
sd_writeSingleBlock (__u32 startBlock)
{
  __u8 response;
  __u16 i, retry = 0;
  response = sd_sendCommand (WRITE_SINGLE_BLOCK, startBlock); //write a Block command

  if (response != 0x00)  //check for SD status: 0x00 - OK (No flags set)
                         //reason maybe supply voltage not stable
    {
      return response;
    }

  spi_cs_assert ();

  spi_send_byte (0xfe);          //Send start block token 0xfe (0x11111110)

  for (i = 0; i < _MAX_SS; i++)    //send 512 bytes data
    spi_send_byte (mmc_buffer[i]);

  spi_send_byte (0xff);          //transmit dummy CRC (16-bit), CRC is ignored here
  spi_send_byte (0xff);

  response = spi_rcv_byte ();

  //We need to ignore CRC in SPI mode, otherwise error in writing
  /* if ((response & 0x1f) != 0x05) //response= 0xXXX0AAA1 ; AAA='010' - data accepted
    {                              //AAA='101'-data rejected due to CRC error
      spi_cs_deassert ();             //AAA='110'-data rejected due to write error
      printf ("data rejected 0x%x\n", response);
      return response;
    } */

  while (!spi_rcv_byte())         //wait for SD card to complete writing and get idle
    {
      if (retry++ > 0xfffe)
        {
          spi_cs_deassert ();
          return 1;
        }
    }

  spi_cs_deassert ();
  spi_send_byte (0xff);            //just spend 8 clock cycle delay before reasserting the CS line
  spi_cs_assert ();                  //re-asserting the CS line to verify if card is still busy

  while (!spi_rcv_byte())          //wait for SD card to complete writing and get idle
    {
      if (retry++ > 0xfffe)
       {
         spi_cs_deassert ();
         return 1;
       }
    }
  spi_cs_deassert ();
  return 0;
}


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
              //delay for VCC stabilisation
              mdelay (500);

              spi_initialze ();

              cardType = 0;

              unsigned char error;
              unsigned int i;
              for (i = 0; i < 10; i++)
                {
                  error = sd_init ();
                  if (!error) break;
                }

              //error = 1 SD card not detected
              //error = 2 Card Initialisation failed..
              if (error)
                {
                  return STA_NOINIT;
                }

              //spi_set_clk_speed (5 , 2); //3.333MHZ
              mdelay (1);   //some delay
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
          unsigned char error;

          /**
           * \brief to read a block
           */
          error = sd_readSingleBlock (sector);

          if (error)
            {
              return RES_ERROR;
            }
          else
            {
              unsigned int i;
              for (i = 0; i < _MAX_SS; i++)
                {
                  buff[i] = mmc_buffer[i];
                  //?if (mmc_buffer[i] == '~') break;
                  //?spi_send_byte (mmc_buffer[i]);
                }
            }

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
 * \remarks
 * this function is not stable.
 * in a write failed option, response = 0x04.
 */
DRESULT
disk_write (BYTE drv, const BYTE *buff, DWORD sector, BYTE count)
{
  switch (drv)
    {
      case VOLUME_MMC:
        {
          unsigned int i;
          for (i = 0; i < _MAX_SS; i++)    //send 512 bytes data
            {
              mmc_buffer[i] = buff[i];
            }

          unsigned char error;

          /**
           * \brief to write a block
           */
          error = sd_writeSingleBlock (sector);

          if (error)
            {
              return RES_ERROR;
            }
          else
            {
              return RES_OK;
            }
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
