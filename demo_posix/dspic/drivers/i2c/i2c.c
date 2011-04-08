/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup i2c I2C
 * @{
 */

/**
 * \file
 * I2C Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

#include <define.h>
#include <sys/ioctl.h>


/** Number of counts to wait for an acknowledgement */
#define ACK_TIMEOUT       0x0F
#define i2cIdle()         while(I2C1CONbits.SEN||I2C1CONbits.PEN||I2C1CONbits.RCEN||I2C1CONbits.ACKEN||I2C1STATbits.TRSTAT)


/************************************************************************************************
 * Local Variables
 ************************************************************************************************/
/** I2C status */
typedef union
{
  unsigned char val;
  struct
    {
      /** start */
      unsigned START:1;
      /** restart */
      unsigned RESTART:1;
      /** stop */
      unsigned STOP:1;
      /** not acknowledgement */
      unsigned NACK:1;
      unsigned :1;
      unsigned :1;
      unsigned :1;
      unsigned :1;    
    } bits;
} I2C_STATUS;
/** indicate the start and stop condition with the data in progress */
static I2C_STATUS i2c_status;


#if (I2C_NUM > 1)
#include <pthread.h>
pthread_mutex_t i2c_mutex; 
#endif /* I2C_NUM > 1 */


void 
i2c_open (void)
{
  //Open i2c if not already opened
  if (I2C1CONbits.I2CEN == 0)
    {
      //Disable I2C master interrupt (Status of I2C is detected by polling)
      _MI2C1IF = 0;
      _MI2C1IE = 0;
      //Enable I2C slave interrupt
      _SI2C1IF = 0;
      _SI2C1IE = 1;
        
      //Configure Baud rate (400kHz, change in define.h)
      I2C1BRG = I2C_BRG;
        
      //I2C Configuration:
      // Enable clock stretch
      // 7-bit address
      I2C1CONbits.STREN = 1;
      I2C1CONbits.I2CEN = 1;

      //I2C bus at idle state, awaiting transmission
      i2cIdle ();

      //clear status flags        
      i2c_status.val = 0;

#if (I2C_NUM > 1)
      pthread_mutex_init (&i2c_mutex, NULL);
#endif /* I2C_NUM > 1 */
    }   
}


int 
i2c_write (unsigned char *buf)
{
  unsigned int count = 0;
    
  /*
   * Send a start or restart bit if needed
   */
  if (i2c_status.bits.START)
    {
      I2C1CONbits.SEN = 1;
      Nop();                            //A small delay for hardware to respond
      while (I2C1CONbits.SEN);          //Wait till Start sequence is completed
    }
  else if (i2c_status.bits.RESTART)
    {
      I2C1CONbits.RSEN = 1;
      Nop();                            //A small delay for hardware to respond
      while (I2C1CONbits.RSEN);         //Wait till Start sequence is completed
    }
    
  /*
   * Send the byte
   */
  I2C1TRN = *buf;                       //Transmit register
  while (I2C1STATbits.TBF);             //Wait for transmit buffer to empty
    
  /*
   * Check if slave acknowledged
   */
  while (I2C1STATbits.ACKSTAT)
    {
      if (++count > ACK_TIMEOUT)
        {
          //Slave did not acknowledge, byte did not transmit successfully,
          //send stop bit to reset i2c
          I2C1CONbits.PEN = 1;
          Nop();                        //A small delay for hardware to respond
          while (I2C1CONbits.PEN);      //Wait till stop sequence is completed
          i2cIdle ();
          return 0;
        }
    }
  i2cIdle ();

  /*
   * Send a stop bit if needed
   */
  if (i2c_status.bits.STOP)
    {
      I2C1CONbits.PEN = 1;
      Nop();                            //A small delay for hardware to respond
      while (I2C1CONbits.PEN);          //Wait till stop sequence is completed
      i2cIdle ();
    }
  i2c_status.val = 0;                   //Clear status
    
  return 1;
}


int 
i2c_read (unsigned char *buf)
{
  /*
   * Get the byte
   */
  I2C1CONbits.RCEN = 1;                   //Enable Receive
  while (I2C1CONbits.RCEN);
  I2C1STATbits.I2COV = 0;                 //Clear receive overflow
  *buf = (unsigned char) I2C1RCV;         //Access the receive buffer
    
  /*
   * Send Acknowledgement
   */
  I2C1CONbits.ACKDT = (i2c_status.bits.NACK)? 1 : 0;
  I2C1CONbits.ACKEN = 1;                //Send Acknowledgement/Not Acknowledgement
  i2cIdle();                            //I2C bus at idle state, awaiting transmission
    
  /*
   * Send a stop bit if needed
   */
  if (i2c_status.bits.STOP)
    {
      I2C1CONbits.PEN = 1;
      Nop();                            //A small delay for hardware to respond
      while (I2C1CONbits.PEN);          //Wait till stop sequence is completed
      i2cIdle ();
    }

  i2c_status.val = 0;                   //Clear status
  return 1;
}


int 
i2c_ioctl (int request, unsigned char* argp)
{
  switch (request)
    {
      case I2C_SET_STATUS:
        i2c_status.val = *argp;
        break;
      default:
        return -1;      //request code not recognised   
    }
  return 0;
}

/** @} */
/** @} */
