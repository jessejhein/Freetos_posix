/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup kb Keyboard
 * @{
 */

/**
 * \file
 * Keyboard Driver
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
#include <time.h>
#include <cirbuf.h>


/** buffer to store key events */
static __u8 kb_buf[MAX_KB_BUF];
/** write pointer of circular buffer */
static __u8 kb_wr = 0;
/** read pointer of circular buffer */
static __u8 kb_rd = 0;
/** IO flag for keyboard */
static __u8 kb_io_flag;
//------------------------------------------------------------------------
/** mutual exclusion for keys: only 1 key can be pressed/hold at a time */
static __u8 kb_pkey_is_pressing;
/** structure for key used in reentrant coroutine */
struct KB_KEY_T
{
  __u8 id;
  __u8 save_time;
  __u16 scrLine;
};
#ifdef KB_PUSH_KEY
/** push key item */
static struct KB_KEY_T kb_push_key[TOTAL_PUSH_KEY];
#endif /* KB_PUSH_KEY */

#ifdef KB_FN_KEY
/** function key item */
static struct KB_KEY_T kb_fn_key[TOTAL_FN_KEY];
#endif /* KB_FN_KEY */
//------------------------------------------------------------------------
typedef struct
{
  union
    {
      /** access configuration byte */
      __u8 config;
      /** access bits: LSB first */
      struct
        {
          /** logic of A pin: */
          unsigned logicA:1;
          /** logic of B pin: */
          unsigned logicB:1;
          /** status: corresponding to the 4 transitions */
          unsigned state:2;
          /** indicate key is clockwise/anti-clockwise */
          unsigned anticlockwise:1;
          unsigned reserved:3;
        } bits;
    };
  /** keeps the time for repeated clockwise key */
  __u8 clockwise_time;
  /** keeps the time for repeated anti-clockwise key */
  __u8 anticlockwise_time;
} KB_RKEY_T;
#ifdef KB_ROTATE_KEY
__u8 kb_clockwise_cnt;
__u8 kb_anticlockwise_cnt;
static KB_RKEY_T kb_rkey;
#endif /* KB_ROTATE_KEY */
//------------------------------------------------------------------------


/**
 * \remarks TRIS should be set correctly for keyboard
 */
int 
kb_open (int flags)
{
  kb_io_flag = (__u8) flags;
  kb_config ();

#ifdef KB_PUSH_KEY
  int i;
  for (i = 0; i < TOTAL_PUSH_KEY; i++)
    {
      kb_push_key[i].id = (BASE_PUSH_KEY + i);
    }
#endif /* KB_PUSHKEY */

#ifdef KB_FN_KEY
  int j;
  for (j = 0; j < TOTAL_FN_KEY; j++)
    {
      kb_fn_key[j].id = (BASE_FN_KEY + j);
    }
#endif /* KB_FN_KEY */
  return 0;
}


int 
kb_read (__u8* buf)
{
  //Perform Read if read operation is enabled
  if ((kb_io_flag & O_RDWR) || !(kb_io_flag & O_WRONLY))
    {
      __u8 next = cirbuf_rd (kb_wr, kb_rd, MAX_KB_BUF);
      //Copy 1 byte when data is available
      if (next != CIRBUF_RD_EMPTY)
        {
          *buf = kb_buf[kb_rd];
          kb_rd = next;
          return 1;
        }
      //No data can be copied
      else
        {
          return 0;
        }      
    }
  //IO not opened for reading
  else
    {
      errno = EBADF;
      return -1;
    }    
}


/**
 * \brief insert key id into circular buffer
 * \param key_id key id for writing
 * \retval 0 indicating no data is written (buffer is full)
 * \retval 1 indicating 1 byte has been written
 */
static int 
kb_write (__u8 key_id)
{
  __u8 next = cirbuf_wr (kb_wr, kb_rd, MAX_KB_BUF);
  if (next != CIRBUF_WR_FULL)
    {
      kb_buf[kb_wr] = key_id;
      kb_wr = next;
      return 1;
    }
  return 0;
}


#ifdef KB_ROTATE_KEY
/**
 * \brief detect rotary keys by change interrupt
 * \remarks priority is given to push key
 *
 * Principle of ROTARY KEY
 * \verbatim
                      Transitions
                  <-1-><-2-><-3-><-4->
  A               +---------+
                  | 1    1  | 0    0
      ------------+         +---------------------------
         
  B                    +---------+
                    0  | 1    1  | 0
      -----------------+         +---------------------
   \endverbatim 
 * or vice versa
 * 
 * Valid sequence:
 * +------------+----------------------------+
 * | State (BA) | 00 -> 01 -> 11 -> 10 -> 00 | Clockwise
 * +------------+----------------------------+
 * | State (BA) | 00 -> 10 -> 11 -> 01 -> 00 | Anti-clockwise
 * +------------+----------------------------+
 *
 */
void _IRQ 
_CNInterrupt (void)
{
  //check A status (clockwise)
  if (kb_rkey_state (BASE_ROTARY_KEY) == 1)
    kb_rkey.bits.logicA = 1;
  else
    kb_rkey.bits.logicA = 0;

  //check B status (anti-clockwise)
  if (kb_rkey_state (BASE_ROTARY_KEY + 1) == 1)
    kb_rkey.bits.logicB = 1;
  else
    kb_rkey.bits.logicB = 0;

  //determine which transition
  switch (kb_rkey.bits.state)
    {
      //Change from [BA = 00]
      case 0:
        {
          //clockwise
          if ((kb_rkey.config & 0x03) == 0x01)
            {
              kb_rkey.bits.anticlockwise = 0;
              //goto next state for further checking
              kb_rkey.bits.state = 1;
            }
          //anti-clockwise
          else if ((kb_rkey.config & 0x03) == 0x02)
            {
              kb_rkey.bits.anticlockwise = 1;
              //goto next state for further checking
              kb_rkey.bits.state = 1;
            }
          break;
        }
      //Change from [BA = #aa]
      case 1:
        {
          //valid, proceed to next state
          if ((kb_rkey.config & 0x03) == 0x03)
            kb_rkey.bits.state = 2;
          //reset
          else
            kb_rkey.bits.state = 0;
          break;
        }
      //Change from [BA = 11]
      case 2:
        {
          //valid, if logic is reversed
          if ( (((kb_rkey.config & 0x03) == 0x02) && (kb_rkey.bits.anticlockwise == 0))
            || (((kb_rkey.config & 0x03) == 0x01) && (kb_rkey.bits.anticlockwise == 1)) )
            kb_rkey.bits.state = 3;
          //reset
          else
            kb_rkey.bits.state = 0;
          break;
        }
      //Change from [BA = a#a]
      case 3:
        {
          //valid, return to normal
          if ((kb_rkey.config & 0x03) == 0x00)
            {
              if (kb_pkey_is_pressing == 0)
                {
                  kb_write (BASE_ROTARY_KEY + kb_rkey.bits.anticlockwise);
                  //clockwise
                  if (kb_rkey.bits.anticlockwise == 0)
                    {
                      //if within the KB_SCAN_PERIOD, there are multiple key hits
                      //save in counter
                      if (((__u8)((__u8)clock () - kb_rkey.clockwise_time)) < KB_SCAN_PERIOD)
                        kb_clockwise_cnt++;
                      else
                        kb_clockwise_cnt = 0;
                      kb_rkey.clockwise_time = (__u8) clock ();
                    }
                  //anti-clockwise
                  else
                    {
                      //if within the KB_SCAN_PERIOD, there are multiple key hits
                      //save in counter
                      if (((__u8)((__u8)clock () - kb_rkey.anticlockwise_time)) < KB_SCAN_PERIOD)
                        kb_anticlockwise_cnt++;
                      else
                        kb_anticlockwise_cnt = 0;
                      kb_rkey.anticlockwise_time = (__u8) clock ();
                    }
                }
            }
          kb_rkey.bits.state = 0;
          break;
        }
    }

  //clear interrupt flag
  _CNIF = 0;
}
#endif /* KB_ROTATE_KEY */


/**
 * \brief the following section uses reentrant coroutine instead of task
 */
#define USE_COROUTINE_REENTRANT         1
#include <unistd.h>
/******************************************************************/
#define ARG                             ((struct KB_KEY_T*) arg)
/**
 * Principle of PUSH key
 * \verbatim
 
    -----------|||                        |||----------------------------------
               |||------------------------|||
      
    <Scan for key>|<----- T ----->|<----- T ----->|<----- T ----->|<Scan for key>
 
                 /|\             /|\             /|\             /|\
                  |               |               |               |
                press           hold           release         new scan
   \endverbatim
 * 
 * T: scan period
 */
static void*
check_key (struct KB_KEY_T *arg)
{
  start_process ();
  
  if (kb_pkey_is_pressing == 0)
    {
      if (kb_key_press (arg->id))
        {
          kb_pkey_is_pressing = 1;
          kb_write (arg->id);

          while (1)
            {
              //time lag to test for hold/release
              arg->save_time = (__u8) clock ();
              while (((__u8)((__u8)clock () - arg->save_time)) < KB_SCAN_PERIOD) usleep (0);

              static __u8 hold_cnt = 0;
              //check for hold/release
              if (kb_key_press (arg->id))
                {
                  //hold
                  if (++hold_cnt > 10)
                    {
                      kb_write (arg->id);
                    }
                }
              else
                {
                  //release
                  kb_pkey_is_pressing = 0;
                  hold_cnt = 0;
                  kb_write (arg->id | 0x80);
                  break;
                }
            }
        }
    }

  end_process ();
}


//------------------------------------------------------------------------------
#ifdef KB_PUSH_KEY
void 
kb_check_push_key (void)
{
  //Perform checking if read operation is enabled
  if ((kb_io_flag & O_RDWR) || !(kb_io_flag & O_WRONLY))
    {
      unsigned char i;
      for (i = 0; i < TOTAL_PUSH_KEY; i++)
        {
          check_key (&kb_push_key[i]);
        }
    }
}
#endif /* KB_PUSH_KEY */


//------------------------------------------------------------------------------
#ifdef KB_FN_KEY
void
kb_check_fn_key (void)
{
  //Perform checking if read operation is enabled
  if ((kb_io_flag & O_RDWR) || !(kb_io_flag & O_WRONLY))
    {
      unsigned char i;
      for (i = 0; i < TOTAL_FN_KEY; i++)
        {
          check_key (&kb_fn_key[i]);
        }
    }
}
#endif /* KB_FN_KEY */

/** @} */
/** @} */
