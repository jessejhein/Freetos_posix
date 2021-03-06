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
#include <unistd.h>
#include <cirbuf.h>
#include "lpt.h"


/*----- types/defines -------------------------------------------------------*/
//#define LPT_DEBUG_WR    1
//#define LPT_DEBUG_RD    1

/** structure for parallel ports */
typedef struct
{
  /** accessing mode */
  __u8 flags;
  /** buffer to store data from parallel port */
  __u8 lpt_buf[LPT_BUF_SIZE];
  /** write pointer of circular buffer */
  __u8 lpt_wr;
  /** read pointer of circular buffer */
  __u8 lpt_rd;
  /** flag to indicate if is inside LPT receive ISR */
  __u8 in_lpt_rx_isr;
} LPT_PORT_T;

/*----- Internal global(s) --------------------------------------------------*/
static LPT_PORT_T lpt_port;

/*----- Internal Function(s)-------------------------------------------------*/
static void
lpt1_outb (__u8 value)
{
  //wait until RX interrupt in process is finished
  while (lpt_port.in_lpt_rx_isr) ;
  lpt_set_interrupt_pin(1);

  //set up data bus for output
  bus_data_config(0);
  //perform a write
  bus_data_write(value);
  //set up I/O bus
  bus_set_io_write(1);

  udelay(150);
  lpt_set_interrupt_pin(0);
}

static __u8
lpt1_inb (void)
{
  __u8 data;

  //set up data bus for input
  bus_data_config(1);
  //set up I/O bus
  bus_set_io_write(0);
  //perform a read
  data = bus_data_read();
  //restore data port
  udelay(0);
  bus_data_config(0);

  return data;
}


/*----- Public Function(s)---------------------------------------------------*/
int
lpt_open (int flags)
{
  LPT_CLR_INT();
  LPT_ENABLE_INT();

  lpt_port.flags = (__u8)flags;
  lpt_port.lpt_wr= 0;
  lpt_port.lpt_rd= 0;
  lpt_port.in_lpt_rx_isr= 0;

  return 0;
}


//-------------------------------------------------------------------------------------
int
lpt_write (__u8* buf, __u16 count)
{
  __u16 wr_cnt = 0;

  if ( (lpt_port.flags & O_RDWR) || (lpt_port.flags & O_WRONLY) )
    {
      //perform output/write
      for (wr_cnt=0; wr_cnt<count; wr_cnt++)
        {
#ifdef LPT_DEBUG_WR
          printf("wr: 0x%02x\n", *buf);
#endif  /* LPT_DEBUG_WR */
          lpt1_outb(*buf);
          buf++;
        }

#ifdef LPT_DEBUG_WR
      printf("%d of %d byte(s) wrote\n", wr_cnt, count);
#endif  /* LPT_DEBUG_WR */
    }

  return wr_cnt;
}


int
lpt_read (__u8* buf, __u16 count)
{
  __u16 rd_cnt = 0;

  if ( (lpt_port.flags & O_RDWR) || (lpt_port.flags & O_RDONLY) )
    {
      //perform read operation
      __u8 next;

      //wait until RX interrupt in process is finished
      while (lpt_port.in_lpt_rx_isr) ;

      LPT_DISABLE_INT();
      //perform read
      for (rd_cnt=0; rd_cnt<count; rd_cnt++)
        {
          next = cirbuf_rd(lpt_port.lpt_wr, lpt_port.lpt_rd, LPT_BUF_SIZE);
          if (next == CIRBUF_RD_EMPTY) break;

          //copy 1 byte from the circular buffer when data is available
          *buf = lpt_port.lpt_buf[ lpt_port.lpt_rd ];
          lpt_port.lpt_rd = next;

#ifdef LPT_DEBUG_RD
          printf("rd: 0x%02x\n", *buf);
#endif  /* LPT_DEBUG_RD */
          buf++;
        }
      LPT_ENABLE_INT();

#ifdef LPT_DEBUG_RD
      if (rd_cnt) printf("%d byte(s) read\n", rd_cnt);
#endif  /* LPT_DEBUG_RD */
    }

  return rd_cnt;
}


//-------------------------------------------------------------------------------------
int
lpt_close (void)
{
  LPT_DISABLE_INT();
  return 0;
}





//-------------------------------------------------------------------------------------
void
lpt1_isr (void)
{
  __u8 data;
  __u8 next;

  lpt_port.in_lpt_rx_isr = 1;   //now in ISR

  //Clear the interrupt flag or else
  //the CPU will keep vectoring back to the ISR
  LPT_CLR_INT();

  data = lpt1_inb();

  next = cirbuf_wr(lpt_port.lpt_wr, lpt_port.lpt_rd, LPT_BUF_SIZE);
  if (next != CIRBUF_WR_FULL)
    {
      //write the byte to LPT1 buffer if it is not full
      lpt_port.lpt_buf[lpt_port.lpt_wr] = data;
      lpt_port.lpt_wr = next;
    }

  lpt_port.in_lpt_rx_isr = 0;   //now leave ISR
}

#endif /* LPT_MOD */

/** @} */
/** @} */
