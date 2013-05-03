/**
 * \file
 * SPI Driver
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

#include <define.h>
#include "spi.h"


void
spi_open (void)
{
  spi_pin_init();               // SPI not active by default

  IFS2bits.SPI2IF = 0;          // Clear the Interrupt Flag
  IEC2bits.SPI2IE = 0;          // Disable the Interrupt

  //SPI CON2
  SPI2CON2bits.FRMEN  = 0;      // Framed SPI2 support disabled
  SPI2CON2bits.SPIFSD = 0;      // Frame sync pulse output (master)

  //SPI CON1
  SPI2CON1bits.MSTEN  = 1;      // Master mode
  SPI2CON1bits.DISSCK = 0;      // Internal SPI clock is enabled
  SPI2CON1bits.DISSDO = 0;      // SDOx pin is controlled by the module
  SPI2CON1bits.MODE16 = 0;      // Communication is byte-wide (8 bits)
  SPI2CON1bits.SMP    = 0;      // Input data is sampled at the middle of data output time
  SPI2CON1bits.CKE    = 1;      // transition from active clock state to Idle clock state
  SPI2CON1bits.SSEN   = 0;      // SSx pin is not used by the module. Pin controlled by port function
  SPI2CON1bits.CKP    = 0;      // Idle state for clock is a low level; active state is a high level

  //SPI STAT
  SPI2STATbits.SPIROV = 0;      // Clear Receive Overflow
  SPI2STATbits.SPISIDL = 0;     // Continue module operation in Idle mode

  spi_set_clk_speed (400000UL); // Set initial speed to 400kHz
}


void
spi_close (void)
{
  //Disable the STATbit 15
  SPI2STATbits.SPIEN = 0;
}


__u8
spi_xchg (__u8 data)
{
  SPI2BUF = (__u8) data;
  while (!SPI2STATbits.SPIRBF);
  //Receive complete, SPIxRXB is full
  return (__u8) SPI2BUF;
}


int
spi_write (const __u8 *buf, __u16 count)
{
  __u16 i;
  __u8 dummy;
  for (i = 0; i < count; i++)
    {
      SPI2BUF = (__u8) buf[i];
      while (!SPI2STATbits.SPIRBF);
      //Receive complete, SPIxRXB is full
      dummy = SPI2BUF;
    }
  return i;
}


int
spi_read (__u8 *buf, __u16 count)
{
  __u16 i;
  for (i = 0; i < count; i++)
    {
      SPI2BUF = (__u8) 0xFF;
      while (!SPI2STATbits.SPIRBF);
      //Receive complete, SPIxRXB is full
      buf[i] = SPI2BUF;
    }
  return i;
}


/**
 * \remarks speed = Fcy / factor
 * \verbatim
       Sec |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |
    Pri    |     |     |     |     |     |     |     |     |
    -------+-----+ --- + --- + --- + --- + --- + --- + --- +
    1      | --- |   2 |   3 |   4 |   5 |   6 |   7 |   8 |
    4      | --- | --- |  12 |  16 |  20 |  24 |  28 |  32 |
    16     | --- | --- |  48 |  64 |  80 |  96 | 112 | 128 |
    64     | --- | --- | 192 | 256 | 320 | 384 | 448 | 512 |
   \endverbatim */
__u32
spi_set_clk_speed (__u32 speed)
{
  __u32 ans;

  //Disable SPI
  SPI2STATbits.SPIEN = 0;

  float factor = ((float) SYSTEM_CLK_HZ / speed);

  //minimum speed
  if (factor > 448)
    {
      SPI2CON1bits.PPRE = PPRE_64_1;  SPI2CON1bits.SPRE = SPRE_8_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 512UL);
    }
  else if (factor > 384)
    {
      SPI2CON1bits.PPRE = PPRE_64_1;  SPI2CON1bits.SPRE = SPRE_7_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 448UL);
    }
  else if (factor > 320)
    {
      SPI2CON1bits.PPRE = PPRE_64_1;  SPI2CON1bits.SPRE = SPRE_6_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 384UL);
    }
  else if (factor > 256)
    {
      SPI2CON1bits.PPRE = PPRE_64_1;  SPI2CON1bits.SPRE = SPRE_5_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 320UL);
    }
  else if (factor > 192)
    {
      SPI2CON1bits.PPRE = PPRE_64_1;  SPI2CON1bits.SPRE = SPRE_4_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 256UL);
    }
  else if (factor > 128)
    {
      SPI2CON1bits.PPRE = PPRE_64_1;  SPI2CON1bits.SPRE = SPRE_3_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 192UL);
    }
  //---------------------------------------------------------------
  else if (factor > 112)
    {
      SPI2CON1bits.PPRE = PPRE_16_1;  SPI2CON1bits.SPRE = SPRE_8_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 128UL);
    }
  else if (factor > 96)
    {
      SPI2CON1bits.PPRE = PPRE_16_1;  SPI2CON1bits.SPRE = SPRE_7_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 112UL);
    }
  else if (factor > 80)
    {
      SPI2CON1bits.PPRE = PPRE_16_1;  SPI2CON1bits.SPRE = SPRE_6_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 96UL);
    }
  else if (factor > 64)
    {
      SPI2CON1bits.PPRE = PPRE_16_1;  SPI2CON1bits.SPRE = SPRE_5_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 80UL);
    }
  else if (factor > 48)
    {
      SPI2CON1bits.PPRE = PPRE_16_1;  SPI2CON1bits.SPRE = SPRE_4_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 64UL);
    }
  else if (factor > 32)
    {
      SPI2CON1bits.PPRE = PPRE_16_1;  SPI2CON1bits.SPRE = SPRE_3_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 48UL);
    }
  //---------------------------------------------------------------
  else if (factor > 28)
    {
      SPI2CON1bits.PPRE = PPRE_4_1;   SPI2CON1bits.SPRE = SPRE_8_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 32UL);
    }
  else if (factor > 24)
    {
      SPI2CON1bits.PPRE = PPRE_4_1;   SPI2CON1bits.SPRE = SPRE_7_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 28UL);
    }
  else if (factor > 20)
    {
      SPI2CON1bits.PPRE = PPRE_4_1;   SPI2CON1bits.SPRE = SPRE_6_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 24UL);
    }
  else if (factor > 16)
    {
      SPI2CON1bits.PPRE = PPRE_4_1;   SPI2CON1bits.SPRE = SPRE_5_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 20UL);
    }
  else if (factor > 12)
    {
      SPI2CON1bits.PPRE = PPRE_4_1;   SPI2CON1bits.SPRE = SPRE_4_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 16UL);
    }
  else if (factor > 8)
    {
      SPI2CON1bits.PPRE = PPRE_4_1;   SPI2CON1bits.SPRE = SPRE_3_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 12UL);
    }
  //---------------------------------------------------------------
  else if (factor > 7)
    {
      SPI2CON1bits.PPRE = PPRE_1_1;   SPI2CON1bits.SPRE = SPRE_8_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 8UL);
    }
  else if (factor > 6)
    {
      SPI2CON1bits.PPRE = PPRE_1_1;   SPI2CON1bits.SPRE = SPRE_7_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 7UL);
    }
  else if (factor > 5)
    {
      SPI2CON1bits.PPRE = PPRE_1_1;   SPI2CON1bits.SPRE = SPRE_6_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 6UL);
    }
  else if (factor > 4)
    {
      SPI2CON1bits.PPRE = PPRE_1_1;   SPI2CON1bits.SPRE = SPRE_5_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 5UL);
    }
  else if (factor > 3)
    {
      SPI2CON1bits.PPRE = PPRE_1_1;   SPI2CON1bits.SPRE = SPRE_4_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 4UL);
    }
  else if (factor > 2)
    {
      SPI2CON1bits.PPRE = PPRE_1_1;   SPI2CON1bits.SPRE = SPRE_3_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 3UL);
    }
  //maximum speed
  else
    {
      SPI2CON1bits.PPRE = PPRE_1_1;   SPI2CON1bits.SPRE = SPRE_2_1;
      ans = ((__u32)SYSTEM_CLK_HZ / 2UL);
    }

  //Enable SPI
  SPI2STATbits.SPIEN = 1;
  return ans;
}
