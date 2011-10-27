/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup uart UART
 * @{
 */

/**
 * \file
 * UART Driver
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
#include <cirbuf.h>
#include <termios.h>
#include <sys/ioctl.h>


//------------------------------------------------------------------------
#if (UART0_RX_BUF_SIZE == 0)
#define UART0_RX_ENABLE                         0
#else /* UART0_RX_BUF_SIZE > 0 */
#define UART0_RX_ENABLE                         1
#endif /* UART0_RX_BUF_SIZE > 0 */
#if (UART0_TX_BUF_SIZE == 0)
#define UART0_TX_ENABLE                         0
#else /* UART0_TX_BUF_SIZE > 0 */
#define UART0_TX_ENABLE                         1
#endif /* UART0_TX_BUF_SIZE > 0 */
//------------------------------------------------------------------------
#if (UART1_RX_BUF_SIZE == 0)
#define UART1_RX_ENABLE                         0
#else /* UART1_RX_BUF_SIZE > 0 */
#define UART1_RX_ENABLE                         1
#endif /* UART1_RX_BUF_SIZE > 0 */
#if (UART1_TX_BUF_SIZE == 0)
#define UART1_TX_ENABLE                         0
#else /* UART1_TX_BUF_SIZE > 0 */
#define UART1_TX_ENABLE                         1
#endif /* UART1_TX_BUF_SIZE > 0 */
//------------------------------------------------------------------------
/** factor applied to speed to save memory */
#define BAUDRATE_FACTOR                         (9600)
//------------------------------------------------------------------------


#if (UART0_RX_ENABLE == 1)
/** UART0 RX buffer */
__u8 uart0_rx_buf[UART0_RX_BUF_SIZE];
#endif /* UART0_RX_ENABLE */
#if (UART0_TX_ENABLE == 1)
/** UART0 TX buffer */
__u8 uart0_tx_buf[UART0_TX_BUF_SIZE];
#endif /* UART0_TX_ENABLE */

#if (UART1_RX_ENABLE == 1)
/** UART1 RX buffer */
__u8 uart1_rx_buf[UART1_RX_BUF_SIZE];
#endif /* UART1_RX_ENABLE */
#if (UART1_TX_ENABLE == 1)
/** UART1 TX buffer */
__u8 uart1_tx_buf[UART1_TX_BUF_SIZE];
#endif /* UART1_TX_ENABLE */


/** data structure to store UART configuration */
struct UART_DATA
{
  //setting
  tcflag_t cflag;
  __u8 io_flag;
  //RX
  __u8 rx_wr;
  __u8 rx_rd;
  __u8 rx_buf_size;
  __u8* rx_buf;
  //TX
  __u8 tx_done;
  __u8 tx_wr;
  __u8 tx_rd;
  __u8 tx_buf_size;
  __u8* tx_buf;
};
static struct UART_DATA uart[NO_OF_UART];


/*
 * Local Functions
 */
static void uart0_init (void);
#if (NO_OF_UART > 1)
static void uart1_init (void);
#endif /* NO_OF_UART > 1 */


int
uart_open (int device, int flags)
{
  switch (device)
    {
      case 0:
        {
          //=====================================================
#if (UART0_RX_ENABLE == 1)
          uart[0].rx_wr = 0;
          uart[0].rx_rd = 0;
          uart[0].rx_buf = &uart0_rx_buf[0];
          uart[0].rx_buf_size = UART0_RX_BUF_SIZE;
#endif /* UART0_RX_ENABLE */
          //=====================================================
#if (UART0_TX_ENABLE == 1)
          uart[0].tx_wr = 0;
          uart[0].tx_rd = 0;
          uart[0].tx_done = 1;
          uart[0].tx_buf = &uart0_tx_buf[0];
          uart[0].tx_buf_size = UART0_TX_BUF_SIZE;
#endif /* UART0_TX_ENABLE */
          //=====================================================
          uart[0].io_flag = (__u8) flags;
          //=====================================================
          uart[0].cflag = 0;
          //8-bit
          uart[0].cflag &= ~CSIZE;
          uart[0].cflag |= CS8;
          //Parity bit
#if (UART0_PARITYBIT == 2)
          uart[0].cflag |= PARENB;
          uart[0].cflag &= ~PARODD;
#elif (UART0_PARITYBIT == 1)
          uart[0].cflag |= PARENB;
          uart[0].cflag |= PARODD;
#else /* NO_PARITY */
          uart[0].cflag &= ~PARENB;
#endif /* NO_PARITY */
          //Stop bit
#if (UART0_STOPBIT == 2)
          uart[0].cflag |= CSTOPB;
#else /** 1_STOP_BIT */
          uart[0].cflag &= ~CSTOPB;
#endif /** 1_STOP_BIT */
          uart[0].cflag |= (0x1F & (__u16)(UART0_BAUDRATE / BAUDRATE_FACTOR));
          uart0_init ();
          //=====================================================
          break;
        }
#if (NO_OF_UART > 1)
      case 1:
        {
          //=====================================================
#if (UART1_RX_ENABLE == 1)
          uart[1].rx_wr = 0;
          uart[1].rx_rd = 0;
          uart[1].rx_buf = &uart1_rx_buf[0];
          uart[1].rx_buf_size = UART1_RX_BUF_SIZE;
#endif /* UART1_RX_ENABLE */
          //=====================================================
#if (UART1_TX_ENABLE == 1)
          uart[1].tx_wr = 0;
          uart[1].tx_rd = 0;
          uart[1].tx_done = 1;
          uart[1].tx_buf = &uart1_tx_buf[0];
          uart[1].tx_buf_size = UART1_TX_BUF_SIZE;
#endif /* UART1_TX_ENABLE */
          //=====================================================
          uart[1].io_flag = (__u8) flags;
          //=====================================================
          uart[1].cflag = 0;
          //8-bit
          uart[1].cflag &= ~CSIZE;
          uart[1].cflag |= CS8;
          //Parity bit
#if (UART1_PARITYBIT == 2)
          uart[1].cflag |= PARENB;
          uart[1].cflag &= ~PARODD;
#elif (UART1_PARITYBIT == 1)
          uart[1].cflag |= PARENB;
          uart[1].cflag |= PARODD;
#else /* NO_PARITY */
          uart[1].cflag &= ~PARENB;
#endif /* NO_PARITY */
          //Stop bit
#if (UART1_STOPBIT == 2)
          uart[1].cflag |= CSTOPB;
#else /** 1_STOP_BIT */
          uart[1].cflag &= ~CSTOPB;
#endif /** 1_STOP_BIT */
          uart[1].cflag |= (0x1F & (__u16)(UART1_BAUDRATE / BAUDRATE_FACTOR));
          uart1_init ();
          //=====================================================
          break;
        }
#endif /* NO_OF_UART > 1 */
      default:
        {
          return -1;
        }
    }
  return 0;
}


//-------------------------------------------------------------------------------------
static void 
uart0_init (void)
{
  UART_A_RS485_CTL_EN ();
  //=============================================================================
  // Configure Baud rate
  __u32 speed = (uart[0].cflag & 0x1F) * (__u32)BAUDRATE_FACTOR;
  U2BRG = GEN_UART_BAUDRATE (speed);
  //==============================================================================
  // Configure Mode [Default: 8N1]
  U2MODE = 0x0000;
  if (uart[0].cflag & PARENB)
    {
      //Odd Parity
      if (uart[0].cflag & PARODD) U2MODEbits.PDSEL = 2;
      //Even Parity
      else U2MODEbits.PDSEL = 1;
    }
  if (uart[0].cflag & CSTOPB)
    {
      //2 Stop bits
      U2MODEbits.STSEL = 1;
    }
  //==============================================================================
  // Configure Interrupt
#if (UART0_RX_ENABLE == 1)
  _U2RXIF = 0;                //Clear RX interrupt flags
  _U2RXIE = 1;                //Receive interrupt: 0 disable, 1 enable
  U2STAbits.URXISEL0 = 0;     //RX Interrupt when a character is received
  U2STAbits.URXISEL1 = 0;
#endif /* UART0_RX_ENABLE */
#if (UART0_TX_ENABLE == 1)
  _U2TXIF = 0;                //Clear TX interrupt flags
  _U2TXIE = 1;                //Transmit interrupt: 0 disable, 1 enable
  U2STAbits.UTXISEL0 = 1;     //TX Interrupt when all TX is completed
  U2STAbits.UTXISEL1 = 0;
#endif /* UART0_TX_ENABLE */
  //==============================================================================
  // Enable U2ART module
  U2MODEbits.UARTEN = 1;
#if (UART0_TX_ENABLE == 1)
  //==============================================================================
  // Enable Transmit
  U2STAbits.UTXEN = 1;
#endif /* UART0_TX_ENABLE */
}


//-------------------------------------------------------------------------------------
#if (NO_OF_UART > 1)
static void 
uart1_init (void)
{
  UART_B_RS485_CTL_EN ();
  //=============================================================================
  // Configure Baud rate
  __u32 speed = (uart[1].cflag & 0x1F) * (__u32)BAUDRATE_FACTOR;
  U1BRG = GEN_UART_BAUDRATE (speed);
  //==============================================================================
  // Configure Mode [Default: 8N1]
  U1MODE = 0x0000;
  if (uart[1].cflag & PARENB)
    {
      //Odd Parity
      if (uart[1].cflag & PARODD) U1MODEbits.PDSEL = 2;
      //Even Parity
      else U1MODEbits.PDSEL = 1;
    }
  if (uart[1].cflag & CSTOPB)
    {
      //2 Stop bits
      U1MODEbits.STSEL = 1;
    }
  //==============================================================================
  // Configure Interrupt
#if (UART1_RX_ENABLE == 1)
  _U1RXIF = 0;                //Clear RX interrupt flags
  _U1RXIE = 1;                //Receive interrupt: 0 disable, 1 enable 
  U1STAbits.URXISEL0 = 0;     //RX Interrupt when a character is received
  U1STAbits.URXISEL1 = 0;
#endif /* UART1_RX_ENABLE */
#if (UART1_TX_ENABLE == 1)
  _U1TXIF = 0;                //Clear TX interrupt flags
  _U1TXIE = 1;                //Transmit interrupt: 0 disable, 1 enable
  U1STAbits.UTXISEL0 = 1;     //TX Interrupt when all TX is completed
  U1STAbits.UTXISEL1 = 0;
#endif /* UART1_TX_ENABLE */
  //==============================================================================
  // Enable U2ART module
  U1MODEbits.UARTEN = 1;
#if (UART1_TX_ENABLE == 1)
  //==============================================================================
  // Enable Transmit
  U1STAbits.UTXEN = 1;
#endif /* UART1_TX_ENABLE */
}
#endif /* NO_OF_UART > 1 */


//-------------------------------------------------------------------------------------
#if (UART0_TX_ENABLE == 1)
/**
 * \brief Transmit Interrupt Handler for U2ART
 * \remarks TX Interrupt when all TX is completed
 * \remarks First interrupt is set manually in uart_write
 */
void _IRQ
_U2TXInterrupt (void)
{
  _U2TXIF = 0;

  __u8 next = cirbuf_rd (uart[0].tx_wr, uart[0].tx_rd, uart[0].tx_buf_size);
  //data available to transmit
  if (next != CIRBUF_RD_EMPTY)
    {
      U2TXREG = (uart[0].tx_buf[uart[0].tx_rd] & 0xFF);
      uart[0].tx_rd = next;
    }
  //no more data to transmit
  else
    {
      while (!U2STAbits.TRMT);          //wait until the last transmission has ended
      UART_A_RS485_TX_OFF ();           //Set transmit pin to Low, i.e. back to receive mode
      uart[0].tx_done = 1;
    }
}
#endif /* UART0_TX_ENABLE */


#if (NO_OF_UART > 1)
#if (UART1_TX_ENABLE == 1)
/**
 * \brief Transmit Interrupt Handler for U1ART
 * \remarks TX Interrupt when all TX is completed
 * \remarks First interrupt is set manually in uart_write
 */
void _IRQ
_U1TXInterrupt (void)
{
  _U1TXIF = 0;

   __u8 next = cirbuf_rd (uart[1].tx_wr, uart[1].tx_rd, uart[1].tx_buf_size);
  //data available to transmit
  if (next != CIRBUF_RD_EMPTY)
    {
      U1TXREG = (uart[1].tx_buf[uart[1].tx_rd] & 0xFF);
      uart[1].tx_rd = next;
    }
  //no more data to transmit
  else
    {
      while (!U1STAbits.TRMT);          //wait until the last transmission has ended
      UART_B_RS485_TX_OFF ();           //Set transmit pin to Low, i.e. back to receive mode
      uart[1].tx_done = 1;
    }
}
#endif /* UART1_TX_ENABLE */
#endif /* NO_OF_UART > 1 */


//-------------------------------------------------------------------------------------
#if (UART0_RX_ENABLE == 1)
/**
 * \brief Receive Interrupt Handler for U2ART
 * \remarks RX Interrupt when a character is received
 * \remarks Read until the RX buffer is emptied
 */
void _IRQ
_U2RXInterrupt (void)
{
  //1 or more bytes received
  while (U2STAbits.URXDA)
    {
      __u8 next = cirbuf_wr (uart[0].rx_wr, uart[0].rx_rd, uart[0].rx_buf_size);
      //If buffer is not full
      if (next != CIRBUF_WR_FULL)
        {
          uart[0].rx_buf[uart[0].rx_wr] = (__u8) U2RXREG;
          uart[0].rx_wr = next;
        }
      //When buffer is full, still remove data from register, but the incoming data is lost
      else
        {
          next = (__u8) U2RXREG;
        }
    }
  //Rx Buffer Overflow:
  //If this bit is not cleared, the UART module cannot receive more data
  if (U2STAbits.OERR) U2STAbits.OERR = 0;

  _U2RXIF = 0;
}
#endif /* UART0_RX_ENABLE */



#if (NO_OF_UART > 1)
#if (UART1_RX_ENABLE == 1)
/**
 * \brief Receive Interrupt Handler for U1ART (RS485)
 * \remarks RX Interrupt when a character is received
 * \remarks Read until the RX buffer is emptied
 */
void _IRQ
_U1RXInterrupt (void)
{
  //1 or more bytes received
  while (U1STAbits.URXDA)
    {
      __u8 next = cirbuf_wr (uart[1].rx_wr, uart[1].rx_rd, uart[1].rx_buf_size);
      //If buffer is not full
      if (next != CIRBUF_WR_FULL)
        {
          uart[1].rx_buf[uart[1].rx_wr] = (__u8) U1RXREG;
          uart[1].rx_wr = next;
        }
      //When buffer is full, still remove data from register, but the incoming data is lost
      else
        {
          next = (__u8) U1RXREG;
        }
    }
  //Rx Buffer Overflow:
  //If this bit is not cleared, the UART module cannot receive more data
  if (U1STAbits.OERR) U1STAbits.OERR = 0;

  _U1RXIF = 0;
}
#endif /* UART0_RX_ENABLE */
#endif /* NO_OF_UART > 1 */


//-------------------------------------------------------------------------------------
int
uart_write (int device, __u8* buf, __u16 count)
{
  //Perform write if write operation is enabled
  if ((uart[device].io_flag & O_RDWR) || (uart[device].io_flag & O_WRONLY))
    {
      //If transmit has not completed, wait for it to finish
      while (!uart[device].tx_done);

      //copy data to buffer
      __u16 byte;
      for (byte = 0; byte < count; byte++)
        {
           __u8 next = cirbuf_wr (uart[device].tx_wr, uart[device].tx_rd, uart[device].tx_buf_size);
          //Buffer is not full
          if (next != CIRBUF_WR_FULL)
            {
              uart[device].tx_buf[uart[device].tx_wr] = buf[byte];
              uart[device].tx_wr = next;
            }
          else break;
        }

      //Raise Interrupt flag to initiate transmission
      if (device == 0)
        {
#if (UART0_TX_ENABLE == 1)
          uart[device].tx_done = 0;     //start new transmission
          UART_A_RS485_TX_ON ();        //Set transmit pin to High
          _U2TXIF = 1;                  //Start interrupt
#endif /* UART0_TX_ENABLE */
        }
      else
        {
#if (UART1_TX_ENABLE == 1)
          uart[device].tx_done = 0;     //start new transmission
          UART_B_RS485_TX_ON ();        //Set transmit pin to High
          _U1TXIF = 1;                  //Start interrupt
#endif /* UART1_TX_ENABLE */
        }
      return byte;
    }
  //IO not opened for writing
  else
    {
      errno = EBADF;
      return -1;
    }
}


//-------------------------------------------------------------------------------------
int
uart_read (int device, __u8* buf)
{
  //Perform read if read operation is enabled
  if ((uart[device].io_flag & O_RDWR) || !(uart[device].io_flag & O_WRONLY))
    {
       __u8 next = cirbuf_rd (uart[device].rx_wr, uart[device].rx_rd, uart[device].rx_buf_size);
      //Copy 1 byte when data is available
      if (next != CIRBUF_RD_EMPTY)
        {
          *buf = uart[device].rx_buf[uart[device].rx_rd];
          uart[device].rx_rd = next;
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


//-------------------------------------------------------------------------------------
int
uart_close (int device)
{
  switch (device)
    {
      case 0:
        {
#if (UART0_TX_ENABLE == 1)
          //Wait for TX to complete
          while (!U2STAbits.TRMT);
          UART_A_RS485_TX_OFF ();               //Set TX pin to Low, i.e. back to receive mode
          _U2TXIF = 0;                          //Clear TX interrupt flag
          _U2TXIE = 0;                          //Disable TX interrupt
          U2STAbits.UTXEN = 0;                  //Disable TX
          uart[0].tx_done = 1;                  //Set TX as completed
#endif /* UART0_TX_ENABLE */

#if (UART0_RX_ENABLE == 1)
          //Wait for RX to complete
          while (!U2STAbits.RIDLE);
          _U2RXIF = 0;                          //Clear RX interrupt flag
          _U2RXIE = 0;                          //Disable RX interrupt
#endif /* UART0_RX_ENABLE */
          U2MODEbits.UARTEN = 0;
          break;
        }
#if (NO_OF_UART > 1)
      case 1:
        {
#if (UART1_TX_ENABLE == 1)
          //Wait for TX to complete
          while (!U1STAbits.TRMT);
          UART_B_RS485_TX_OFF ();               //Set TX pin to Low, i.e. back to receive mode
          _U1TXIF = 0;                          //Clear TX interrupt flag
          _U1TXIE = 0;                          //Disable TX interrupt
          U1STAbits.UTXEN = 0;                  //Disable TX
          uart[1].tx_done = 1;                  //Set TX as completed
#endif /* UART1_TX_ENABLE */

#if (UART1_RX_ENABLE == 1)
          //Wait for RX to complete
          while (!U1STAbits.RIDLE);
          _U1RXIF = 0;                          //Clear RX interrupt flag
          _U1RXIE = 0;                          //Disable RX interrupt
#endif /* UART1_RX_ENABLE */
          U1MODEbits.UARTEN = 0;
          break;
        }
#endif /* NO_OF_UART > 1 */
      default:
        {
          return -1;
        }
    }
  return 0;
}


//-------------------------------------------------------------------------------------
int
uart_ioctl (int device, int request, void* argp)
{
  switch (request)
    {
      //set option
      case UART_SET_OPTION:
        {
          switch (device)
            {
              case 0:
                {
                  uart[0].cflag = ((struct termios*)argp)->c_cflag;
                  uart0_init ();
                  break;
                }
#if (NO_OF_UART > 1)
              case 1:
                {
                  uart[1].cflag = ((struct termios*)argp)->c_cflag;
                  uart1_init ();
                  break;
                }
#endif /* NO_OF_UART > 1 */
              //UART id not recognised
              default:
                return -1;
            }
        }
      //get baud rate
      case UART_GET_OPTION:
        {
          switch (device)
            {
              case 0:
                {
                  ((struct termios*)argp)->c_cflag = uart[0].cflag;
                  break;
                }
#if (NO_OF_UART > 1)
              case 1:
                {
                  ((struct termios*)argp)->c_cflag = uart[1].cflag;
                  break;
                }
#endif /* NO_OF_UART > 1 */
              //UART id not recognised
              default:
                return -1;
            }
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
