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

#include <define.h>
#include <fcntl.h>
#include <errno.h>
#include <cirbuf.h>
#include <termios.h>
#include <sys/ioctl.h>


/*
 * Local Variables 
 */
static unsigned char uart_rx_buf[NO_OF_UART][MAX_UART_RX_BUF];
/** UART RX pointers */
struct UART_Rx
{
  unsigned char wr;
  unsigned char rd;
};
static struct UART_Rx uart_rx[NO_OF_UART];
static unsigned char uart_tx_buf[NO_OF_UART][MAX_UART_TX_BUF];
/** UART TX pointers */
struct UART_Tx
{
  unsigned char wr;                             
  unsigned char rd;
  unsigned char tx_complete_flag;
};
static struct UART_Tx uart_tx[NO_OF_UART];
static int uart_io_flag[NO_OF_UART];

//keep the baud rate
static speed_t uart_speed[NO_OF_UART];

/*
 * Local Functions
 */
static unsigned int uart2_autobaud (void);
static void uart2_init (__u32 speed);
#if (NO_OF_UART > 1)
static unsigned int uart1_autobaud (void);
static void uart1_init (__u32 speed);
#endif /* NO_OF_UART > 1 */


int 
uart_open (unsigned char device, int flags)
{
  uart_io_flag[device] = flags;
    
#if (NO_OF_UART > 1)
  switch (device) 
    {
      case 0 : 
        {
#endif /* NO_OF_UART > 1 */
          //==========Primary UART = U2ART: RS232================
          uart_rx[0].wr = 0;
          uart_rx[0].rd = 0;
          //=====================================================
          uart_tx[0].wr = 0;
          uart_tx[0].rd = 0;
          uart_tx[0].tx_complete_flag = 1;
          //=====================================================
          uart_speed[0] = UART_BAUDRATE0;
          uart2_init (GEN_UART_BAUDRATE(uart_speed[0]));
          //=====================================================
#if (NO_OF_UART > 1)
          break;
        }
      case 1 :
        {
          //========Secondary UART = U1ART: RS485================
          uart_rx[1].wr = 0;
          uart_rx[1].rd = 0;
          //=====================================================
          uart_tx[1].wr = 0;
          uart_tx[1].rd = 0;
          uart_tx[1].tx_complete_flag = 1;
          //=====================================================
          uart_speed[1] = UART_BAUDRATE1;
          uart1_init (GEN_UART_BAUDRATE(uart_speed[1]));
          //=====================================================
          break;
        }
    }
#endif /* NO_OF_UART > 1 */
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
  __u32 u2brg = speed;
#if (AUTO_BAUD_DECT > 0)
  u2brg = uart2_autobaud ();
#endif /* AUTO_BAUD_DECT */
  U2BRG = u2brg;
  //==============================================================================
  // Configure Mode
  // +-- 8N1, no loopback, no wake in sleep mode, continue in idle mode, no autobaud
  U2MODE = 0x0000;
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
  __u32 u1brg = speed;
#if (AUTO_BAUD_DECT > 0)
  u1brg = uart1_autobaud ();
#endif /* AUTO_BAUD_DECT */
  U1BRG = u1brg;					
  //==============================================================================
  // Configure Mode
  // +-- 8N1, no loopback, no wake in sleep mode, continue in idle mode, no autobaud
  U1MODE = 0x0000;
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


#if (AUTO_BAUD_DECT > 0)
//-------------------------------------------------------------------------------------
static 
unsigned int uart2_autobaud (void)
{
  U2MODEbits.ABAUD = 1;   //Enable Autobaud detect from U2RX (from IC2 if 0)
  U2MODEbits.UARTEN = 1;  //U2ART enable
  //Timer 3 Config==========================================================
  PR3 = 0xFFFF;           // Set counter to maximum
  _T3IF = 0;              // Clear interrupt flag
  _T3IE = 0;              // Disable interrupt
  T3CONbits.TON = 1;      // Start the timer, TMR3 count up
  //Input Capture Config====================================================
  IC2CONbits.ICM = 3;     //Detect rising	
  _IC2IF = 0;             //Clear interrupt flag
  _IC2IE = 0;             //Disable interrupt
  //Start Auto baud detection===============================================
  //Expected character = 'U' (0x55)
  //			  _   _   _   _   _
  //			_|S|_|1|_|1|_|1|_|1|_
  //           <--------------->
  //========================================================================
  cli();                  //Disable Global Interrupt
  while(!_IC2IF);         //1st rising edge detected
  TMR3 = 0;               //Clear TMR3 to start count up
  _IC2IF = 0;             //Clear interrupt flag
  while(!_IC2IF);         //2nd rising edge detected
  _IC2IF = 0;             //Clear interrupt flag
  while(!_IC2IF);         //3rd rising edge detected
  _IC2IF = 0;             //Clear interrupt flag
  while(!_IC2IF);         //4th rising edge detected
  _IC2IF = 0;             //Clear interrupt flag
  while(!_IC2IF);         //5th rising edge detected
  _IC2IF = 0;             //Clear interrupt flag
  T3CONbits.TON = 0;      //Stop the timer
  sti();                  //Enable Global Interrupt
  //Compute value for BRG register==========================================
  // BRG 	= (FCY/(16*baudrate))-1
  //		= (FCY/(16*FCY/(TMR3/8)))-1
  //		= (TMR3/128) - 1
  // add 0x04 for rounding
  unsigned int time;
  time = ((TMR3+0x40)>>7)-1;			
  //========================================================================
  return time;   
}


//-------------------------------------------------------------------------------------
#if (NO_OF_UART > 1)
static unsigned int 
uart1_autobaud (void)
{
  U1MODEbits.ABAUD = 1;   //Enable Autobaud detect from U2RX (from IC1 if 0)
  U1MODEbits.UARTEN = 1;  //U1ART enable
  //Timer 3 Config==========================================================
  PR3 = 0xFFFF;           // Set counter to maximum
  _T3IF = 0;              // Clear interrupt flag
  _T3IE = 0;              // Disable interrupt
  T3CONbits.TON = 1;      // Start the timer, TMR3 count up
  //Input Capture Config====================================================
  IC1CONbits.ICM = 3;     //Detect rising 
  _IC1IF = 0;             //Clear interrupt flag
  _IC1IE = 0;             //Disable interrupt
  //Start Auto baud detection===============================================
  //Expected character = 'U' (0x55)
  //        _   _   _   _   _
  //      _|S|_|1|_|1|_|1|_|1|_
  //           <--------------->
  //========================================================================
  cli();                  //Disable Global Interrupt
  while(!_IC1IF);         //1st rising edge detected
  TMR3 = 0;               //Clear TMR3 to start count up
  _IC1IF = 0;             //Clear interrupt flag
  while(!_IC1IF);         //2nd rising edge detected
  _IC1IF = 0;             //Clear interrupt flag
  while(!_IC1IF);         //3rd rising edge detected
  _IC1IF = 0;             //Clear interrupt flag
  while(!_IC1IF);         //4th rising edge detected
  _IC1IF = 0;             //Clear interrupt flag
  while(!_IC1IF);         //5th rising edge detected
  _IC1IF = 0;             //Clear interrupt flag
  T3CONbits.TON = 0;      //Stop the timer
  sti();                  //Enable Global Interrupt
  //Compute value for BRG register==========================================
  // BRG  = (FCY/(16*baudrate))-1
  //    = (FCY/(16*FCY/(TMR3/8)))-1
  //    = (TMR3/128) - 1
  // add 0x04 for rounding
  unsigned int time;
  time = ((TMR3+0x40)>>7)-1;      
  //========================================================================
  return time;   
}
#endif /* NO_OF_UART > 1 */
#endif /* AUTO_BAUD_DECT > 0 */


/**
 * \brief Transmit Interrupt Handler for U2ART (RS232)
 * \remarks TX Interrupt when all TX is completed
 * \remarks First interrupt is set manually in uart_write
 */
void _IRQ 
_U2TXInterrupt (void)
{
#ifdef MPLAB_DSPIC33_PORT
  _U2TXIF = 0;
#else
  DISI_PROTECT(_U2TXIF = 0);
#endif

  unsigned char next_data_pos;
  next_data_pos = pre_rd_cir254buf (    (unsigned char)uart_tx[0].wr,
                                        (unsigned char)uart_tx[0].rd,
                                        MAX_UART_TX_BUF);
  if (next_data_pos != 255)
    {
      //Data ready in circular TX buffer
      U2TXREG = (uart_tx_buf[0][(unsigned char)uart_tx[0].rd] & 0xFF);  //send next byte...
      uart_tx[0].rd = (unsigned char) next_data_pos;                    //update rd pointer
    } 
  else 
    {
      //Transmission has completed
      uart_tx[0].tx_complete_flag = 1;          // change to empty of tx
#if (UARTA_RS485 > 0)
      while (!U2STAbits.TRMT);                  //wait for transmit register to empty, i.e. the byte has been sent
      RS485_TX_OFF ();                          //Set transmit pin to Low, i.e. back to receive mode
#endif		
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
#ifdef MPLAB_DSPIC33_PORT
  _U1TXIF = 0;
#else
  DISI_PROTECT(_U1TXIF = 0);
#endif

  unsigned char next_data_pos;
  next_data_pos = pre_rd_cir254buf (    (unsigned char)uart_tx[1].wr,
                                        (unsigned char)uart_tx[1].rd,
                                        MAX_UART_TX_BUF);
  if (next_data_pos != 255)
    {
      //Data ready in circular TX buffer
      U1TXREG = (uart_tx_buf[1][(unsigned char)uart_tx[1].rd] & 0xFF);  //send next byte...
      uart_tx[1].rd = (unsigned char) next_data_pos;                    //update rd pointer
    } 
  else 
    {
      //Transmission has completed
      uart_tx[1].tx_complete_flag = 1;          // change to empty of tx
#if (UARTB_RS485 > 0)
      while (!U1STAbits.TRMT);                  //wait for transmit register to empty, i.e. the byte has been sent
      RS485_TX_OFF ();                          //Set transmit pin to Low, i.e. back to receive mode
#endif		
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
  unsigned char next_data_pos;
  while (U2STAbits.URXDA)
    {
      //Data is received======================================================
      next_data_pos = pre_wr_cir254buf (uart_rx[0].wr, uart_rx[0].rd, MAX_UART_RX_BUF);
      if (next_data_pos != 255)
        {
          //If buffer is not full
          uart_rx_buf[0][uart_rx[0].wr] = (unsigned char) U2RXREG;
          uart_rx[0].wr = next_data_pos;
        } 
      else
        {
          //When buffer is full, still remove data from register, but the incoming data is lost
          next_data_pos = (unsigned char) U2RXREG;      //Read the data from buffer
        }
    }
  //Rx Buffer Overflow: 
  //If this bit is not cleared, the UART module cannot receive more data
  if (U2STAbits.OERR) U2STAbits.OERR = 0;

#ifdef MPLAB_DSPIC33_PORT
  _U2RXIF = 0;
#else
  DISI_PROTECT(_U2RXIF = 0);          //Clear Interrupt Flag
#endif
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
  unsigned char next_data_pos;
  //Data is received======================================================
  while (U1STAbits.URXDA)
    {
      next_data_pos = pre_wr_cir254buf (uart_rx[1].wr, uart_rx[1].rd, MAX_UART_RX_BUF);
      if (next_data_pos != 255)
        {
          //If buffer is not full
          uart_rx_buf[1][uart_rx[1].wr] = (unsigned char) U1RXREG;
          uart_rx[1].wr = next_data_pos;
        }
      else
        {
          //When buffer is full, still remove data from register, but the incoming data is lost
          next_data_pos = (unsigned char) U1RXREG; 			//Read the data from buffer
        }
    }
  //Rx Buffer Overflow: 
  //If this bit is not cleared, the UART module cannot receive more data
  if (U1STAbits.OERR) U1STAbits.OERR = 0;
    
#ifdef MPLAB_DSPIC33_PORT
  _U1RXIF = 0;
#else
  DISI_PROTECT(_U1RXIF = 0);          //Clear Interrupt Flag
#endif
}
#endif /* NO_OF_UART > 1 */


int 
uart_write (unsigned char device, unsigned char *buf, int count)
{
  //Perform write if write operation is enabled
  if (uart_io_flag[device] & O_RDWR || uart_io_flag[device] & O_WRONLY)
    {
      //If transmit has not completed, wait for it to finish
      while (uart_tx[device].tx_complete_flag == 0);
      
      //start new transmission
      uart_tx[device].tx_complete_flag = 0;
        
      int next_data_pos;
      int byte = 0;
      for (; byte < count; byte++)
        {
          next_data_pos = pre_wr_cir254buf (    (unsigned char)uart_tx[device].wr,
                                                (unsigned char)uart_tx[device].rd,
                                                MAX_UART_TX_BUF);
          if (next_data_pos != 255)
            {
              //Valid data is available
              uart_tx_buf[device][uart_tx[device].wr] = (unsigned char) buf[byte];    //copy the char to tx_buf
              uart_tx[device].wr = next_data_pos;                                     //increment the ptr
            } 
          else break;
        }

      //Raise Interrupt flag to initiate transmission
      if (device == 0)
        {
#if (UARTA_RS485 > 0)
          RS485_TX_ON ();       //Set transmit pin to High
#endif
          _U2TXIF = 1;          //Start interrupt
        } 
      else
        {
#if (UARTB_RS485 > 0)
          RS485_TX_ON ();       //Set transmit pin to High
#endif
          _U1TXIF = 1;          //Start interrupt
        }
      return byte;        
    }
  //Error, raise error flag
  else
    {
      errno = EBADF;  //io not opened for writing
      return -1;
    }  
}


int 
uart_read (unsigned char device, unsigned char *buf)
{
  //Perform read if read operation is enabled
  if (uart_io_flag[device] & O_RDWR || uart_io_flag[device] & O_RDONLY)
    {
      int next_data_pos;
      next_data_pos = pre_rd_cir254buf (uart_rx[device].wr, uart_rx[device].rd, MAX_UART_RX_BUF);
      //Copy 1 byte when data is available
      if (next_data_pos != 255)
        {
          *buf = uart_rx_buf[device][uart_rx[device].rd];     //copy the stored data to buf
          uart_rx[device].rd = next_data_pos;                 //update the ptr
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
      errno = EBADF;  //io not opened for reading
      return -1;
    }    
}


int
uart_close (unsigned char device)
{
  switch (device) 
    {
      case 0:
        {
          while (!U2STAbits.TRMT);              //Wait for TX to complete
#if (UARTA_RS485 > 0)
          RS485_TX_OFF ();                      //Set TX pin to Low, i.e. back to receive mode
#endif /* UARTA_RS485 */
    
#ifdef MPLAB_DSPIC33_PORT
          _U2TXIF = 0;                          //Clear TX interrupt flag
#else
          DISI_PROTECT(_U2TXIF = 0);
#endif
          _U2TXIE = 0;                          //Disable TX interrupt
          U2STAbits.UTXEN = 0;                  //Disable TX
          uart_tx[0].tx_complete_flag = 1;      //Set TX as completed
                    
          while (!U2STAbits.RIDLE);             //Wait for RX to complete
#ifdef MPLAB_DSPIC33_PORT
          _U2RXIF = 0;                          //Clear RX interrupt flag
#else
          DISI_PROTECT(_U2RXIF = 0);
#endif
          _U2RXIE = 0;                          //Disable RX interrupt
          U2MODEbits.UARTEN = 0;                //Disable RX
          break;
        }
#if (NO_OF_UART > 1)        
      case 1:
        {
          while (!U1STAbits.TRMT);              //Wait for TX to complete
#if (UARTB_RS485 > 0)
          RS485_TX_OFF ();                      //Set TX pin to Low, i.e. back to receive mode
#endif /* UARTB_RS485 */

#ifdef MPLAB_DSPIC33_PORT
          _U1TXIF = 0;                          //Clear TX interrupt flag
#else
          DISI_PROTECT(_U1TXIF = 0);
#endif
          _U1TXIE = 0;                          //Disable TX interrupt
          U1STAbits.UTXEN = 0;                  //Disable TX
          uart_tx[1].tx_complete_flag = 1;      //Set TX as completed

          while (!U1STAbits.RIDLE);             //Wait for RX to complete
          
#ifdef MPLAB_DSPIC33_PORT
          _U1RXIF = 0;                          //Clear RX interrupt flag
#else
          DISI_PROTECT(_U1RXIF = 0);
#endif
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
          if (*((unsigned long*)argp) == 0) return 0;

          switch (device)
            {
              case 0:
                {     
                  uart_speed[0] = *((unsigned long*)argp);
                  uart2_init (GEN_UART_BAUDRATE(uart_speed[0]));
                  break;
                }
#if (NO_OF_UART > 1)        
              case 1:
                {
                  uart_speed[1] = *((unsigned long*)argp);
                  uart1_init (GEN_UART_BAUDRATE(uart_speed[1]));
                  break;
                }
#endif /* NO_OF_UART > 1 */  
              default:
                return -1;                      //UART id not recognised
            }
        }
      //get baud rate
      case UART_GET_BAUDRATE:
        {
          switch (device)
            {
              case 0:
                {
                  *((unsigned long*)argp) = uart_speed[0];
                  break;          
                }
#if (NO_OF_UART > 1)        
              case 1:
                {
                  *((unsigned long*)argp) = uart_speed[1];
                  break;
                }
#endif /* NO_OF_UART > 1 */  
              default:
                return -1;                      //UART id not recognised
            }
   
        } 
      default:
        return -1;      //request code not recognised
    }
  return 0;
}

/** @} */
/** @} */
