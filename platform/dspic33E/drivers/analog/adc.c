/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup adc ADC
 * @{
 */

/**
 * \file
 * ADC Driver
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
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>


/** store IO setting */
static __u8 adc_io_flag;
/** indicate the current selected channel for read() */
static __u8 adc_ch_select = 0;
/** Indicate if RAM data is ready for output */
static __u8 adc_data_ready = 0;
/** keeps the total number of channel being scan */
static __u8 adc_on_ch_num = 0;
/** bitwise ADC status: 1=on, 0=off */
static __u32 adc_ch_status = 0;
/** Store most updated data, index of array referenced by channel ID */
static __u16 adc_queue[ADC_QSIZE][ADC_MAX_CH];
/** pointer to the most updated sample */
static __u16 adc_queue_ptr = ADC_QSIZE - 1;
//--------------------------------------------------------------------------------------
#if (ADC_MAX_HW_CH > 0)
/** Buffer A in Ping Pong mode, reside in DMA aligned at 64 boundary (assume max 32 channels (i.e. 64 bytes) */
static _DMA(64) unsigned int adc_bufA[ADC_MAX_HW_CH];
/** Buffer B in Ping Pong mode, reside in DMA aligned at 64 boundary (assume max 32 channels (i.e. 64 bytes) */
static _DMA(64) unsigned int adc_bufB[ADC_MAX_HW_CH];
#endif /* ADC_MAX_HW_CH */
//--------------------------------------------------------------------------------------


/**
 * \brief get the ADC channel status
 * \param ch channel id
 * \retval 0 not turned on
 * \retval >1 turned on
 */
#define GET_CH_STATUS(ch)               ((0x00000001 << ch) & adc_ch_status)


/**
 * \brief set the ADC channel to on state
 * \param ch channel id
 */
#define SET_CH_STATUS_ON(ch)            (adc_ch_status |= (0x00000001 << ch))


int
adc_open (int flags)
{
  adc_io_flag = (__u8) flags;
  //===========================================================================
  // Disable ADC Interrupt (Use DMA)
  _AD1IF = 0;
  _AD1IE = 0;
  //===========================================================================
  // Configure Sample and Hold
  //  +-- Use MUX A only (no interleaving)
  //  +-- Set CH0 S/H -ve to VRef-
  AD1CHS0bits.CH0NA = 0;
  //===========================================================================
  // ADCCON4:
  //  +-- Each buffer contains 1 word
  AD1CON4bits.DMABL = 0;
  AD1CON4bits.ADDMAEN = 1;
  //===========================================================================
  // ADCCON3:
  //  +--A/D Conversion Clock Source = Use system clock
  //  +--Set Auto Sample Time = # of TAD
  //  +--A/D Conversion Clock Select ADCS<5:0>= (TAD/TCY)-1
  AD1CON3bits.ADRC = 0;
  AD1CON3bits.SAMC = ADC_ACQ_TIME;
  AD1CON3bits.ADCS = ADC_ADCS;
  //===========================================================================
  // ADCCON2:
  //  +--Default: Use MUX A, No splitting of Buffer
  //  +--Scan Input Selections
  //  +--Voltage Reference Configuration AVDD and AVSS
  AD1CON2bits.CSCNA = 1;
  AD1CON2bits.VCFG = 0;
  //===========================================================================
  // ADCCON1:
  //  +--Default: continue in idle mode, integer format
  //  +--Enable ADC, Conversion Trigger Source Auto, Auto sampling on
  AD1CON1bits.FORM = ADC_OUTPUT_FORMAT;
  AD1CON1bits.SSRC = 7;        //auto covert, using internal clock source
  AD1CON1bits.ASAM = 1;        //auto setting of SAMP bit
  AD1CON1bits.AD12B = 1;       //12-bit, 1-channel ADC operation
  AD1CON1bits.ADDMABM = 0;     //DMA buffers are built in scatter/gather mode
  AD1CON1bits.ADON = 0;

  //===========================================================================
  // DMA0 Configuration:
  DMA0CONbits.AMODE = 2;        // Configure DMA for Peripheral Indirect mode
  DMA0CONbits.MODE  = 2;        // Configure DMA for Continuous Ping-Pong mode
  DMA0PAD = (volatile int)&ADC1BUF0;    // Point DMA to ADC1BUF0
  DMA0REQ = 13;                 // Select ADC1 as DMA Request source
  //DMA0STAL = __builtin_dmaoffset(&adc_bufA);
  //DMA0STAH = __builtin_dmapage(&adc_bufA);
  DMA0STAL = (unsigned int)&adc_bufA;
  DMA0STAH = 0x0000;
  //DMA0STBL = __builtin_dmaoffset(&adc_bufB);
  //DMA0STBH = __builtin_dmapage(&adc_bufB);
  DMA0STBL = (unsigned int)&adc_bufB;
  DMA0STBH = 0x0000;
  _DMA0IF = 0;                  // Clear the DMA interrupt flag bit
  _DMA0IE = 1;                  // Set the DMA interrupt enable bit
  DMA0CONbits.CHEN = 1;         //Enable DMA
  return 0;
}


/**
 * \brief ADC Interrupt Service Routine: keep ADC16Ptr up-to-date
 * \remarks This routine requires about 30us for execution
 */
void _IRQ 
_DMA0Interrupt (void)
{
  //update DMA pointer
  static unsigned int* adc_buf_ptr = 0;
  adc_buf_ptr = (adc_buf_ptr == adc_bufB)? adc_bufA : adc_bufB;

  //update queue pointer
  adc_queue_ptr++;
  if (adc_queue_ptr == ADC_QSIZE) adc_queue_ptr = 0;

  //copy selected channel data to queue
  int channel;
  for (channel = 0; channel < ADC_MAX_HW_CH; channel++)
    {
      if (GET_CH_STATUS (channel))
        {
          int local_buf_id = adc_parse_ch (channel);
          if ((local_buf_id != -1) && (local_buf_id < ADC_MAX_CH))
            {
              adc_queue[adc_queue_ptr][local_buf_id] = adc_buf_ptr[channel];
            }
        }
    }
  adc_data_ready = 1;

  //Clear DMA Channel 0 Interrupt
  _DMA0IF = 0;
}



static int 
adc_read_channel (__u16* buf, int count)
{
  int num_of_samples = count / 2;
  int queue_index = adc_queue_ptr;
  int i;
  for (i = 0; (i < num_of_samples) && (i < ADC_QSIZE); i++)
    {
      buf[i] = adc_queue[queue_index][adc_ch_select];
      if (--queue_index < 0) queue_index = ADC_QSIZE - 1;
    }
  return 2 * i;
}


int 
adc_read (__u16* buf, int count)
{
  //Perform Read if read operation is enabled
  if ((adc_io_flag & O_RDWR) || !(adc_io_flag & O_WRONLY))
    {
      //Non-Blocking IO
      if (adc_io_flag & O_NONBLOCK)
        {
          if (adc_data_ready == 1)
            {
              return adc_read_channel (buf, count);
            }
          //data is not ready
          errno = EAGAIN;
          return -1;
        }
      //Blocking IO
      else
        {
          while (adc_data_ready == 0);
          return adc_read_channel (buf, count);
        }
    }
  //IO not opened for reading
  else
    {
      errno = EROFS;
      return -1;
    }
}


int 
adc_ioctl (int request, unsigned char* argp)
{ 
  switch (request)
    {
      //ADD channels to current set
      case ADC_ADD_CH:
        {
          __u8 ch_id = (__u16) argp[0];
          //Select current channel for reading
          int local_buf_id = adc_parse_ch (ch_id);
          if (local_buf_id == -1) return -1;
          adc_ch_select = (__u8) local_buf_id;

          //add channel if not in scan list
          if (GET_CH_STATUS (ch_id) == 0)
            {
              cli ();
              SET_CH_STATUS_ON (ch_id);
              adc_add_ch (ch_id);

              //increment interrupt counter
              adc_on_ch_num++;
              AD1CON2bits.SMPI = adc_on_ch_num - 1;
              DMA0CNT = adc_on_ch_num - 1;
              //First data not ready yet, until interrupt occurs
              adc_data_ready = 0;
              //Turn on the A/D converter if not already done so
              if (AD1CON1bits.ADON == 0) AD1CON1bits.ADON = 1;
              sti ();
            }
          break;
        }
      //request code not recognised
      default:
        {
          return -1;
        }
    }
  return 0;
}

/** @} */
/** @} */
