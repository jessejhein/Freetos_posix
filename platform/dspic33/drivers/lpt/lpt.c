/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup lpt LPT
 * @{
 */

/**
 * \file
 * Parallel Port Driver
 * \author Hui Ning, Sam <sam_hui@amonics.com>
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
#ifdef LPT_MOD

#include <define.h>
#include <errno.h>
#include <fcntl.h>
#include <cirbuf.h>
#include "lpt.h"


/*----- types/defines -------------------------------------------------------*/
/** structure for parallel ports */
typedef struct
{
  __u8 flags;
  /** buffer to store data from parallel port */
  __u8 lpt_buf[LPT_BUF_SIZE];
  /** write pointer of circular buffer */
  __u8 lpt_wr;
  /** read pointer of circular buffer */
  __u8 lpt_rd;
} LPT_PORT_T;

/*----- Internal global(s) --------------------------------------------------*/
static LPT_PORT_T lpt_port[NO_OF_LPT];

/*----- Internal Function(s)-------------------------------------------------*/
static void lpt1_outb (__u8 value, int port)
{
  LPT1_ISR_IF = 0;      // clear interrupt flag
  LPT1_ISR_IE = 0;      // disable interrupt source

  TRISD &= 0xFBFF;      // set RD10 as output
  LATD |= 0x0400;       // trigger interrupt

  //set up data bus for output
  bus_data_config(0);
  //perform a write
  bus_data_write(value);
  //set up I/O bus
  bus_set_io_write(1);

  Nop(); Nop();
  Nop(); Nop();
  Nop(); Nop();
  Nop(); Nop();
  LATD &= 0xFBFF;
  TRISD |= 0x0400;      // reset RD10 as input

  LPT1_ISR_IF = 0;      // clear interrupt flag
  LPT1_ISR_IE = 1;      // enable interrupt source
}

static __u8 lpt1_inb (int port)
{
  __u8 data;

  //set up data bus for input
  bus_data_config(1);
  //set up I/O bus
  bus_set_io_write(0);
  //perform a read
  data = bus_data_read();
  //restore data port
  Nop(); Nop();
  Nop(); Nop();
  Nop(); Nop();
  Nop(); Nop();
  bus_data_config(0);

  return data;
}


/*----- Public Function(s)---------------------------------------------------*/
int lpt_open (int device, int flags)
{
  switch (device)
  {
    case 0:
      // Configure Interrupt for LPT1
      LPT1_ISR_IF = 0;  // clear interrupt flag
      LPT1_ISR_IE = 1;  // enable interrupt source
      break;

#if (NP_OF_LPT == 2)
    case 1:
      // Configure Interrupt for LPT2

#endif  /* NO_OF_LPT */
    default:
      return -1;
  }

  lpt_port[device].flags = (__u8)flags;
  lpt_port[device].lpt_wr= 0;
  lpt_port[device].lpt_rd= 0;

  return 0;
}


//-------------------------------------------------------------------------------------
int lpt_write (int device, __u8* buf, __u16 count)
{
  switch (device)
  {
    case 0:     //LPT1
      if ( (lpt_port[device].flags & O_RDWR) || (lpt_port[device].flags & O_WRONLY) )
        {
          //perform write operation
          __u16 wr_cnt;

          //perform output/write
          for (wr_cnt=0; wr_cnt<count; wr_cnt++)
            {
#ifdef MONITOR_PORT
              printf("write: %c\n", *buf);
#endif
              lpt1_outb(*buf, device);
              buf++;
            }
#ifdef MONITOR_PORT
          printf("%d byte(s) wrote\n", wr_cnt);
#endif

          return wr_cnt;
        }
      break;

#if (NP_OF_LPT == 2)
    case 1:     //LPT2
      break;
#endif  /* NO_OF_LPT */
  }

  return 0;
}


int lpt_read (int device, __u8* buf, __u16 count)
{
  switch (device)
  {
    case 0:
#if (NP_OF_LPT == 2)
    case 1:
#endif  /* NO_OF_LPT */
      if ( (lpt_port[device].flags & O_RDWR) || (lpt_port[device].flags & O_RDONLY) )
        {
          //perform read operation
          __u8 next;
          __u16 rd_cnt;

          //perform read
          for (rd_cnt=0; rd_cnt<count; rd_cnt++)
            {
              next = cirbuf_rd(lpt_port[device].lpt_wr, lpt_port[device].lpt_rd, LPT_BUF_SIZE);
              if (next == CIRBUF_RD_EMPTY) break;

              //copy 1 byte from the circular buffer when data is available
              *buf = lpt_port[device].lpt_buf[ lpt_port[device].lpt_rd ];
#ifdef MONITOR_PORT
              printf("read: %c\n", *buf);
#endif
              lpt_port[device].lpt_rd = next;
              buf++;
            }
#ifdef MONITOR_PORT
          if (rd_cnt) printf("%d byte(s) read\n", rd_cnt);
#endif

          return rd_cnt;
        }
      break;
  }

  return 0;
}


//-------------------------------------------------------------------------------------
int lpt_close (int device)
{
  switch (device)
  {
    case 0:
      LPT1_ISR_IE = 0;      // disable interrupt source for LPT1
      break;

#if (NP_OF_LPT == 2)
    case 1:
      break;
#endif  /* NO_OF_LPT */
  }
  return 0;
}





//-------------------------------------------------------------------------------------
void lpt1_isr (void)
{
  __u8 data;
  __u8 next;

  LPT1_ISR_IF = 0;      //Clear the INT3 interrupt flag or else
                        //the CPU will keep vectoring back to the ISR

  data = lpt1_inb(0);

  next = cirbuf_wr(lpt_port[0].lpt_wr, lpt_port[0].lpt_rd, LPT_BUF_SIZE);
  if (next != CIRBUF_WR_FULL)
    {
      //write the byte to LPT1 buffer if it is not full
      lpt_port[0].lpt_buf[lpt_port[0].lpt_wr] = data;
      lpt_port[0].lpt_wr = next;
    }
}

#endif /* LPT_MOD */

/** @} */
/** @} */
