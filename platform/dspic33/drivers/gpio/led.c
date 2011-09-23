/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup led LED
 * @{
 */

/**
 * \file
 * LED Driver
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
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <time.h>


#ifdef BUZZER
#define IO_MAX                  (LED_NUM + 1)
#else /* no BUZZER */
#define IO_MAX                  LED_NUM
#endif /* no BUZZER */

/** led status */
static __u8 led_status[IO_MAX];
/** read/write IO flag */
static __u8 led_io_flag;
/** current channel selected for writing */
static __u8 led_ch_select;
/** period of pulse delay (in number of led control period) */
static __u8 led_pulse_period;
/** block status for channel, upto 16 channels */
static __u16 led_block_status;


int 
led_open (int flags)
{
  led_io_flag = (__u8) flags;
  led_config ();
  return 0;
}


int 
led_write (__u8* buf)
{
  //Perform Write if write operation is enabled
  if ((led_io_flag & O_RDWR) || (led_io_flag & O_WRONLY))
    {
      if (led_ch_select < IO_MAX)
        {
          //check for blocking
          __u16 mask = (0x0001 << led_ch_select);
          if (led_block_status & mask) return 0;
          //no blocking
#ifdef BUZZER
          //for direct control of buzzer to provide immediate reponse
          if ((led_ch_select == LED_NUM) && ((*buf == LED_OFF) || (*buf == LED_ON)))
            {
              if (*buf == LED_OFF)
                {
                  led_off (led_ch_select);
                }
              else
                {
                  led_on (led_ch_select);
                }
            }
          else
#endif /* BUZZER */
            {
              led_status[led_ch_select] = *buf;
            }
          return 1;
        }
      //exceed maximum offset
      else
        {
          errno = EFBIG;
          return -1;
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
led_ioctl (int request, unsigned char* argp)
{
  switch (request)
    {
      //Select current channel for writing
      case LED_SELECT_CH:
        {
          led_ch_select = argp[0];
          break;
        }
      //Block the channel
      case LED_BLOCK_CH:
        {
          __u16 mask = (0x0001 << argp[0]);
          if (argp[1] == 0) led_block_status &= ~mask;
          else 
            {
              led_block_status |= mask;
              //turn off led if block
              led_status[argp[0]] = LED_OFF;
            }
          break;
        }
      //Set pulse period to turn on
      case LED_SET_PULSE_PERIOD:
        {
          led_pulse_period = argp[0];
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


// this process wants to use coroutine_st instead of multi-thread when using FreeRTOS
#ifdef FREERTOS_SCHED 
#undef FREERTOS_SCHED
#undef start_process
#undef end_process
#include <coroutine_st.h>
#define start_process()                 scrBegin
#define end_process()                   scrFinish((void*)0)
#endif /* FREERTOS_SCHED */
#include <unistd.h>
//-----------------------------------------------------------------------------------------------

/**
 * \remarks implemented by coroutine
 */
void*
led_ctrl (void* arg)
{
  //perform this in regular interval defined by LED_CTRL_INTERVAL
  static __u8 pulse_period_counter[IO_MAX];
  /** start time for control */
  static __u8 start_time = 0;

  start_process ();

  start_time = (__u8) clock ();
  while (((__u8)((__u8)clock () - start_time)) < LED_CTRL_INTERVAL) usleep(0);
  
  //check all IOs
  static __u8 k;
  for (k = 0; k < IO_MAX; k++)
    {
      // LED OFF
      if (led_status[k] == LED_OFF)
        {
          if (pulse_period_counter[k] == 0)
            {
              led_off (k);
              //go to IDLE state
              led_status[k] = LED_IDLE;
            }
          else
            {
              //wait for next interval
              pulse_period_counter[k]--;
            }
        }
      // LED ON
      else if (led_status[k] == LED_ON)
        {
          if (pulse_period_counter[k] == 0)
            {
              led_on (k);
              //go to IDLE state
              led_status[k] = LED_IDLE;
            }
          else
            {
              //wait for next interval
              pulse_period_counter[k]--;
            }
        }
      // POSITIVE PULSE
      else if (led_status[k] == LED_POS_PULSE)
        {
          //set up pulse period
          pulse_period_counter[k] = led_pulse_period;
          led_on (k);
          //turn off led next time
          led_status[k] = LED_OFF;
        }
      // NEGATIVE PULSE
      else if (led_status[k] == LED_NEG_PULSE)
        {
          //set up pulse period
          pulse_period_counter[k] = led_pulse_period;
          led_off (k);
          //turn on led next time
          led_status[k] = LED_ON;
        }
    }

  end_process ();
}

/** @} */
/** @} */
