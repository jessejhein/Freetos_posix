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

#include <define.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <time.h>
#include <asm/delay.h>
#include <asm/system.h>


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
  LEDCONFIG ();
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

  static unsigned char pulse_period_counter[IO_MAX];
  start_time = clock ();
  while (((clock_t) (clock () - start_time)) < LED_CTRL_INTERVAL) usleep(0);
  
  static int k;
  for (k = 0; k < IO_MAX; k++)
    {
      if (led_status[k] == LED_OFF)
        {
          if (pulse_period_counter[k] == 0)
            {
              io_off (k);
              led_status[k] = LED_IDLE;
            }
          else
            {
              pulse_period_counter[k]--;
            }
        }
      else if (led_status[k] == LED_ON)
        {
          if (pulse_period_counter[k] == 0)
            {
              io_on (k);
              led_status[k] = LED_IDLE;
            }
          else
            {
              pulse_period_counter[k]--;
            }
        }
      else if (led_status[k] == LED_POS_PULSE)
        {
          pulse_period_counter[k] = pulse_period;
          io_on (k);
          //turn off led next time
          led_status[k] = LED_OFF;
        }
      else if (led_status[k] == LED_NEG_PULSE)
        {
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
