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


struct UART_DATA
{
  //setting
  int io_flag;
  speed_t speed;

  //RX
  __u8 rx_buf[MAX_UART_RX_BUF];
  __u8 rx_wr;
  __u8 rx_rd;

  //TX
  __u8 tx_buf[MAX_UART_RX_BUF];
  __u8 tx_wr;
  __u8 tx_rd;
  __u8 tx_done;
};
static struct UART_DATA uart[NO_OF_UART];


/*
 * Local Functions
 */
static void uart2_init (__u32 speed);
#if (NO_OF_UART > 1)
static void uart1_init (__u32 speed);
#endif /* NO_OF_UART > 1 */


int
uart_open (int device, int flags)
{
  switch (device)
    {
      case 0:
        {
          //==========Primary UART = U2ART: RS232================
          uart[0].rx_wr = 0;
          uart[0].rx_rd = 0;
          //=====================================================
          uart[0].tx_wr = 0;
          uart[0].tx_rd = 0;
          uart[0].tx_done = 1;
          //=====================================================
          uart[0].io_flag = flags;
          uart[0].speed = UART_BAUDRATE0;
          uart2_init (GEN_UART_BAUDRATE (uart[0].speed));
          //=====================================================
          break;
        }
#if (NO_OF_UART > 1)
      case 1:
        {
          //========Secondary UART = U1ART: RS485================
          uart[1].rx_wr = 0;
          uart[1].rx_rd = 0;
          //=====================================================
          uart[1].tx_wr = 0;
          uart[1].tx_rd = 0;
          uart[1].tx_done = 1;
          //=====================================================
          uart[1].io_flag = flags;
          uart[1].speed = UART_BAUDRATE1;
          uart1_init (GEN_UART_BAUDRATE (uart[1].speed));
          //=====================================================
          break;
        }
#endif /* NO_OF_UART > 1 */
      default:
        return -1;
    }
  return 0;
}


//-------------------------------------------------------------------------------------
static void 
uart2_init (__u32 speed)
{
#if (UARTA_RS485 > 0)
  RS485_CTL_EN ();
#endif /* UARTA_RS485 */
  //=============================================================================
  // Configure Baud rate
  U2BRG = speed;
  //==============================================================================
  // Configure Mode
  // +-- 8N1, no loopback, no wake in sleep mode, continue in idle mode, no autobaud
  U2MODE = 0x0000;
#if ((UART_DATABIT0 == 8) && (UART_PARITYBIT0 == 2))
  //8E
  U2MODEbits.PDSEL = 1;
#elif ((UART_DATABIT0 == 8) && (UART_PARITYBIT0 == 1))
  //8O
  U2MODEbits.PDSEL = 2;
#elif ((UART_DATABIT0 == 9) && (UART_PARITYBIT0 == 0))
  //9N
  U2MODEbits.PDSEL = 2;
#endif /* BIT and PARITY */

#if (UART_STOPBIT0 == 2)
  U2MODEbits.STSEL = 1;
#endif /* UART_STOPBIT0 */
  //==============================================================================
  // Configure Interrupt
  _U2RXIF = 0;                //Clear RX interrupt flags
  _U2RXIE = 1;                //Receive interrupt: 0 disable, 1 enable 
  U2STAbits.URXISEL0 = 0;     //RX Interrupt when a character is received
  U2STAbits.URXISEL1 = 0;
  _U2TXIF = 0;                //Clear TX interrupt flags
  _U2TXIE = 1;                //Transmit interrupt: 0 disable, 1 enable
  U2STAbits.UTXISEL0 = 1;     //TX Interrupt when all TX is completed
  U2STAbits.UTXISEL1 = 0;
  //==============================================================================
  // Enable U2ART module
  U2MODEbits.UARTEN = 1;
  //==============================================================================
  // Enable Transmit
  U2STAbits.UTXEN = 1;
}


//-------------------------------------------------------------------------------------
#if (NO_OF_UART > 1)
static void 
uart1_init (__u32 speed)
{
#if (UARTB_RS485 > 0)
  RS485_CTL_EN ();
#endif /* UARTB_RS485 */
  //=============================================================================
  // Configure Baud rate
  U1BRG = speed;
  //==============================================================================
  // Configure Mode
  // +-- 8N1, no loopback, no wake in sleep mode, continue in idle mode, no autobaud
  U1MODE = 0x0000;
#if ((UART_DATABIT1 == 8) && (UART_PARITYBIT1 == 2))
  //8E
  U1MODEbits.PDSEL = 1;
#elif ((UART_DATABIT1 == 8) && (UART_PARITYBIT1 == 1))
  //8O
  U1MODEbits.PDSEL = 2;
#elif ((UART_DATABIT1 == 9) && (UART_PARITYBIT1 == 0))
  //9N
  U1MODEbits.PDSEL = 2;
#endif /* BIT and PARITY */

#if (UART_STOPBIT1 == 2)
  U1MODEbits.STSEL = 1;
#endif /* UART_STOPBIT1 */

  //==============================================================================
  // Configure Interrupt
  _U1RXIF = 0;                //Clear RX interrupt flags
  _U1RXIE = 1;                //Receive interrupt: 0 disable, 1 enable 
  U1STAbits.URXISEL0 = 0;     //RX Interrupt when a character is received
  U1STAbits.URXISEL1 = 0;
  _U1TXIF = 0;                //Clear TX interrupt flags
  _U1TXIE = 1;                //Transmit interrupt: 0 disable, 1 enable
  U1STAbits.UTXISEL0 = 1;     //TX Interrupt when all TX is completed
  U1STAbits.UTXISEL1 = 0;
  //==============================================================================
  // Enable U2ART module
  U1MODEbits.UARTEN = 1;
  //==============================================================================
  // Enable Transmit
  U1STAbits.UTXEN = 1;
}
#endif /* NO_OF_UART > 1 */


/**
 * \brief Transmit Interrupt Handler for U2ART (RS232)
 * \remarks TX Interrupt when all TX is completed
 * \remarks First interrupt is set manually in uart_write
 */
void _IRQ
_U2TXInterrupt (void)
{
  _U2TXIF = 0;

  __u8 next_data_pos;
  next_data_pos = pre_rd_cir254buf (uart[0].tx_wr, uart[0].tx_rd, MAX_UART_TX_BUF);
  if (next_data_pos != 255)
    {
      //Data ready in circular TX buffer
      U2TXREG = (uart[0].tx_buf[uart[0].tx_rd] & 0xFF); //send next byte...
      uart[0].tx_rd = next_data_pos;                    //update RD pointer
    }
  else
    {
      //Transmission has completed
      uart[0].tx_done = 1;
#if (UARTA_RS485 > 0)
      while (!U2STAbits.TRMT);                  //wait for transmit register to empty, i.e. the byte has been sent
      RS485_TX_OFF ();                          //Set transmit pin to Low, i.e. back to receive mode
#endif /* UARTA_RS485 */
    }
}


#if (NO_OF_UART > 1)
/**
 * \brief Transmit Interrupt Handler for U1ART (RS485)
 * \remarks TX Interrupt when all TX is completed
 * \remarks First interrupt is set manually in uart_write
 */
void _IRQ
_U1TXInterrupt (void)
{
  _U1TXIF = 0;

  __u8 next_data_pos;
  next_data_pos = pre_rd_cir254buf (uart[1].tx_wr, uart[1].tx_rd, MAX_UART_TX_BUF);
  if (next_data_pos != 255)
    {
      //Data ready in circular TX buffer
      U1TXREG = (uart[1].tx_buf[uart[1].tx_rd] & 0xFF); //send next byte...
      uart[1].tx_rd = next_data_pos;                    //update RD pointer
    }
  else
    {
      //Transmission has completed
      uart[1].tx_done = 1;
#if (UARTB_RS485 > 0)
      while (!U1STAbits.TRMT);                  //wait for transmit register to empty, i.e. the byte has been sent
      RS485_TX_OFF ();                          //Set transmit pin to Low, i.e. back to receive mode
#endif /* UARTB_RS485 */
    }
}
#endif /* NO_OF_UART > 1 */


/**
 * \brief Receive Interrupt Handler for U2ART (RS232)
 * \remarks RX Interrupt when a character is received
 * \remarks Read until the RX buffer is emptied
 */
void _IRQ
_U2RXInterrupt (void)
{
  __u8 next_data_pos;
  while (U2STAbits.URXDA)
    {
      //Data is received======================================================
      next_data_pos = pre_wr_cir254buf (uart[0].rx_wr, uart[0].rx_rd, MAX_UART_RX_BUF);
      if (next_data_pos != 255)
        {
          //If buffer is not full
          uart[0].rx_buf[uart[0].rx_wr] = (__u8) U2RXREG;
          uart[0].rx_wr = next_data_pos;
        }
      else
        {
          //When buffer is full, still remove data from register, but the incoming data is lost
          next_data_pos = (__u8) U2RXREG;
        }
    }
  //Rx Buffer Overflow:
  //If this bit is not cleared, the UART module cannot receive more data
  if (U2STAbits.OERR) U2STAbits.OERR = 0;

  _U2RXIF = 0;
}


/**
 * \brief Receive Interrupt Handler for U1ART (RS485)
 * \remarks RX Interrupt when a character is received
 * \remarks Read until the RX buffer is emptied
 */
#if (NO_OF_UART > 1)
void _IRQ
_U1RXInterrupt (void)
{
  __u8 next_data_pos;
  //Data is received======================================================
  while (U1STAbits.URXDA)
    {
      next_data_pos = pre_wr_cir254buf (uart[1].rx_wr, uart[1].rx_rd, MAX_UART_RX_BUF);
      if (next_data_pos != 255)
        {
          //If buffer is not full
          uart[1].rx_buf[uart[1].rx_wr] = (__u8) U1RXREG;
          uart[1].rx_wr = next_data_pos;
        }
      else
        {
          //When buffer is full, still remove data from register, but the incoming data is lost
          next_data_pos = (__u8) U1RXREG;
        }
    }
  //Rx Buffer Overflow:
  //If this bit is not cleared, the UART module cannot receive more data
  if (U1STAbits.OERR) U1STAbits.OERR = 0;

  _U1RXIF = 0;
}
#endif /* NO_OF_UART > 1 */


int
uart_write (int device, __u8* buf, __u16 count)
{
  //Perform write if write operation is enabled
  if (uart[device].io_flag & O_RDWR || uart[device].io_flag & O_WRONLY)
    {
      //If transmit has not completed, wait for it to finish
      while (uart[device].tx_done == 0);

      //start new transmission
      uart[device].tx_done = 0;

      __u8 next_data_pos;
      __u16 byte;
      for (byte = 0; byte < count; byte++)
        {
          next_data_pos = pre_wr_cir254buf (uart[device].tx_wr, uart[device].tx_rd, MAX_UART_TX_BUF);
          if (next_data_pos != 255)
            {
              //Valid data is available
              uart[device].tx_buf[uart[device].tx_wr] = buf[byte];      //copy the char to tx_buf
              uart[device].tx_wr = next_data_pos;                       //increment the ptr
            }
          else break;
        }

      //Raise Interrupt flag to initiate transmission
      if (device == 0)
        {
#if (UARTA_RS485 > 0)
          RS485_TX_ON ();       //Set transmit pin to High
#endif /* UARTA_RS485 */
          _U2TXIF = 1;          //Start interrupt
        }
      else
        {
#if (UARTB_RS485 > 0)
          RS485_TX_ON ();       //Set transmit pin to High
#endif /* UARTB_RS485 */
          _U1TXIF = 1;          //Start interrupt
        }
      return byte;
    }
  //Error, raise error flag
  else
    {
      //IO not opened for writing
      errno = EBADF;
      return -1;
    }
}


int
uart_read (int device, __u8* buf)
{
  //Perform read if read operation is enabled
  if (uart[device].io_flag & O_RDWR || uart[device].io_flag & O_RDONLY)
    {
      __u8 next_data_pos;
      next_data_pos = pre_rd_cir254buf (uart[device].rx_wr, uart[device].rx_rd, MAX_UART_RX_BUF);
      //Copy 1 byte when data is available
      if (next_data_pos != 255)
        {
          *buf = uart[device].rx_buf[uart[device].rx_rd];     //copy the stored data to buf
          uart[device].rx_rd = next_data_pos;                 //update the ptr
          return 1;
        }
      //No data can be copied
      else
        {
          return 0;
        }
    }
  //Error, raise error flag
  else
    {
      //IO not opened for reading
      errno = EBADF;
      return -1;
    }
}


int
uart_close (int device)
{
  switch (device)
    {
      case 0:
        {
          //Wait for TX to complete
          while (!U2STAbits.TRMT);
#if (UARTA_RS485 > 0)
          RS485_TX_OFF ();                      //Set TX pin to Low, i.e. back to receive mode
#endif /* UARTA_RS485 */
          _U2TXIF = 0;                          //Clear TX interrupt flag
          _U2TXIE = 0;                          //Disable TX interrupt
          U2STAbits.UTXEN = 0;                  //Disable TX
          uart[0].tx_done = 1;                  //Set TX as completed

          //Wait for RX to complete
          while (!U2STAbits.RIDLE);
          _U2RXIF = 0;                          //Clear RX interrupt flag
          _U2RXIE = 0;                          //Disable RX interrupt
          U2MODEbits.UARTEN = 0;                //Disable RX
          break;
        }
#if (NO_OF_UART > 1)
      case 1:
        {
          //Wait for TX to complete
          while (!U1STAbits.TRMT);
#if (UARTB_RS485 > 0)
          RS485_TX_OFF ();                      //Set TX pin to Low, i.e. back to receive mode
#endif /* UARTB_RS485 */
          _U1TXIF = 0;                          //Clear TX interrupt flag
          _U1TXIE = 0;                          //Disable TX interrupt
          U1STAbits.UTXEN = 0;                  //Disable TX
          uart[1].tx_done = 1;                  //Set TX as completed

          //Wait for RX to complete
          while (!U1STAbits.RIDLE);
          _U1RXIF = 0;                          //Clear RX interrupt flag
          _U1RXIE = 0;                          //Disable RX interrupt
          U1MODEbits.UARTEN = 0;                //Disable RX
          break;
        }
#endif /* NO_OF_UART > 1 */
      default:
        return -1;
    }
  return 0;
}


int
uart_ioctl (int device, int request, void* argp)
{
  __u32 brg;

  switch (request)
    {
      //set baud rate
      case UART_SET_BAUDRATE:
        {
          //Set UART in idle mode (hang up)
          if (*((__u32*)argp) == 0) return 0;

          switch (device)
            {
              case 0:
                {
                  uart[0].speed = *((__u32*)argp);
                  uart2_init (GEN_UART_BAUDRATE (uart[0].speed));
                  break;
                }
#if (NO_OF_UART > 1)
              case 1:
                {
                  uart[1].speed = *((__u32*)argp);
                  uart1_init (GEN_UART_BAUDRATE (uart[1].speed));
                  break;
                }
#endif /* NO_OF_UART > 1 */
              //UART id not recognised
              default:
                return -1;
            }
        }
      //get baud rate
      case UART_GET_BAUDRATE:
        {
          switch (device)
            {
              case 0:
                {
                  *((__u32*)argp) = uart[0].speed;
                  break;
                }
#if (NO_OF_UART > 1)
              case 1:
                {
                  *((__u32*)argp) = uart[1].speed;
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
        return -1;
    }
  return 0;
}

/** @} */
/** @} */
