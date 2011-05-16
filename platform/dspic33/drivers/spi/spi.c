/**
 * \file
 * SPI Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

#include <define.h>

void
spi_initialze (void)
{
  TRISG &= 0xFDFF;              // Set SS# (G9) to output
  LATG |= 0x0200;               // Set to High by Default (not active)

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
  SPI2CON1bits.CKE    = 0;      // transition from active clock state to Idle clock state
  SPI2CON1bits.SSEN   = 0;      // SSx pin is not used by the module. Pin controlled by port function
  SPI2CON1bits.CKP    = 0;      // Idle state for clock is a low level; active state is a high level
  SPI2CON1bits.SPRE   = 2;      // Secondary prescale 6:1
  SPI2CON1bits.PPRE   = 1;      // Primary prescale   16:1
                                // SCKx frequency is 416.67kHz.

  SPI2STATbits.SPIROV = 0;      // Clear Receive Overflow

  //SPI STAT
  SPI2STATbits.SPISIDL = 0;     // Continue module operation in Idle mode
  SPI2STATbits.SPIEN   = 1;     // Enables module and configures SCKx, SDOx, SDIx and SSx as serial port pins
}


void
spi_set_clk_speed (int pri_prescale, int sec_prescale)
{
  //Disable SPI
  SPI2STATbits.SPIEN = 0;
  //Secondary prescale
  SPI2CON1bits.SPRE = sec_prescale;
  //Primary prescale
  SPI2CON1bits.PPRE = pri_prescale;
  //Re-enable SPI
  SPI2STATbits.SPIEN = 1;
}


void
spi_send_byte (__u8 input)
{
  //Send the byte
  SPI2BUF = input;

  //Wait for the byte to be sent
  while (SPI2STATbits.SPITBF);
}


__u8
spi_rcv_byte (void)
{
  //Wait for the byte to be received
  unsigned char data;
  if (SPI2STATbits.SPIRBF)
    {
      //already have some data to return, don't initiate a read
      data = SPI2BUF;
      SPI2STATbits.SPIROV = 0;
      return data;
    }

  //We don't have any data to read yet, so initiate a read
  SPI2BUF = 0xFF;                       // write dummy data to initiate an SPI read
  while (SPI2STATbits.SPITBF);          // wait until the data is finished reading
  data = SPI2BUF;
  SPI2STATbits.SPIROV = 0;
  return data;
}


void
spi_cs_assert (void)
{
  //Set G9 to low
  LATG &= 0xFDFF;
}


void
spi_cs_deassert (void)
{
  //Set G9 to high
  LATG |= 0x0200;
}


void
spi_disable (void)
{
  //Disable the STATbit 15
  SPI2STATbits.SPIEN = 0;
}


void
spi_enable (void)
{
  // Enable the STATbit 15
  SPI2STATbits.SPIEN = 1;
}

