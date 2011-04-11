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

/** read/write io flag */
static int led_io_flag;
/** current channel selected for writing */
static unsigned int led_ch_select;
/** block status for channel, upto 16 ch */
static unsigned int led_block_status;
/** led status */
static unsigned char led_status[IO_MAX];
/** start time for control */
static clock_t start_time = 0;
/** period of pulse delay (in number of led control period) */
static unsigned char pulse_period;


int 
led_open (int flags)
{
  led_io_flag = flags;
  io_config ();
  return 0;
}


int 
led_write (unsigned char *buf)
{
  //Perform write if write operation is enabled
  if ((led_io_flag & O_WRONLY) == O_WRONLY)
    {
      if (led_ch_select < IO_MAX)
        {
          //check for blocking
          unsigned int mask = (0x0001 << led_ch_select);
          if (led_block_status & mask) return 0;
          //no blocking
#ifdef BUZZER
          //for direct control of buzzer to provide immediate reponse
          if ((led_ch_select == LED_NUM) && ((*buf == LED_OFF) || (*buf == LED_ON)))
            {
              if (*buf == LED_OFF)
                {
                  io_off (led_ch_select);
                }
              else
                {
                  io_on (led_ch_select);
                }
            }
          else
#endif /* BUZZER */
            {
              led_status[led_ch_select] = *buf;
            }
          return 1;
        }
      else
        {
          errno = EFBIG; //exceed maximum offset
          return -1;
        }
    }
  //Error, raise error flag
  else
    {
      errno = EBADF;  //io not opened for reading
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
      case LED_BLOCK_CH:
        {
          unsigned int mask = (0x0001 << argp[0]);
          if (argp[1] == 0) led_block_status &= ~mask;
          else 
            {
              led_block_status |= mask;
              //turn off led if block
              led_status[argp[0]] = LED_OFF;
            }
          break;
        }
      case LED_SET_PULSE_PERIOD:
        {
          pulse_period = argp[0];
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
  start_process ();

  //perform this in regular interval defined by LED_CTRL_INTERVAL
  static unsigned char pulse_period_counter[IO_MAX];
  start_time = clock ();
  while (((clock_t) (clock () - start_time)) < LED_CTRL_INTERVAL) usleep(0);
  
  //check all IOs
  static int k;
  for (k = 0; k < IO_MAX; k++)
    {
      // LED OFF
      if (led_status[k] == LED_OFF)
        {
          if (pulse_period_counter[k] == 0)
            {
              io_off (k);
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
              io_on (k);
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
          pulse_period_counter[k] = pulse_period;
          io_on (k);
          //turn off led next time
          led_status[k] = LED_OFF;
        }
      // NEGATIVE PULSE
      else if (led_status[k] == LED_NEG_PULSE)
        {
          //set up pulse period
          pulse_period_counter[k] = pulse_period;
          io_off (k);
          //turn on led next time
          led_status[k] = LED_ON;
        }
    }

  end_process();
}

/** @} */
/** @} */
