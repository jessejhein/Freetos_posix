/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup i2c I2C
 * @{
 * 
 * Control I2C
 * \li The I2C module uses SCL and SDA, located at pin 36 and 37 respectively.
 * \li All I2C devices shares a common communication speed (default: 400kHz) 
 * \li The driver has a POSIX-like interface with open(), read(), write(), ioctl()
 */

/**
 * \file
 * I2C Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

#ifdef I2C_MOD

#include <define.h>
#include <sys/ioctl.h>
#include <asm/system.h>

/** Number of counts to wait for an acknowledgment */
#define ACK_TIMEOUT       0x0F
#define i2cIdle()         while(I2CCONbits.SEN||I2CCONbits.PEN||I2CCONbits.RCEN||I2CCONbits.ACKEN||I2CSTATbits.TRSTAT)

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
      /** not acknowledgment */
      unsigned NACK:1;
      unsigned :1;
      unsigned :1;
      unsigned :1;
      unsigned :1;    
    } bits;
} I2C_STATUS;
/** indicate the start and stop condition with the data in progress */
static I2C_STATUS i2c_status;

/************************************************************************************************
 * Semaphore for multiple i2c devices
 * +-- program must acquire i2c_busy before read/write operation (from start bit to stop bit)
 ************************************************************************************************/
#if (I2C_NUM > 1)
#include <pthread.h>
pthread_mutex_t i2c_mutex; 
#endif /* I2C_NUM>1 */


//---------------------------------------------------------------------------------------
/**
 * \brief Initialize I2C with assigned baudrate
 */
void 
i2c_open(void)
{
  //Open i2c if not already opened
  if(I2CCONbits.I2CEN == 0)
    {
      /*
       * Disable I2C interrupt (not used)
       * +-- Status of I2C is detected by polling
       * 
       */
      _SI2CIF = 0;        //Clear Slave interrupt
      _MI2CIF = 0;        //Clear Master interrupt
      _SI2CIE = 0;        //Disable Slave interrupt
      _MI2CIE = 0;        //Disable Master interrupt
        
      /*
       * Configure Baud rate (400kHz, change in define.h>
       */
      I2CBRG = I2C_BRG;
        
      /*
       * I2C Configuration:
       * +-- Default:     Continue in Idle mode, Release SCL clock
       *                  Disable IPMI, SMBus, General Call, clock stretch
       *                  7-bit address,
       *                  Acknowledge sequence, Receive sequence,
       *                  Stop & Start & Repeated Start conditions not in progress
       * +-- Enable I2C, disable slew rate control,      
       */
      I2CCONbits.I2CEN = 1;   //Enable I2C module 
      i2cIdle();              //I2C bus at idle state, awaiting transimission
        
      i2c_status.val = 0;     //clear status flags

#if (I2C_DAC_MOD & NVM_I2C) 
      pthread_mutex_init(&i2c_mutex, NULL);
#endif
    }   
}


//---------------------------------------------------------------------------------------
/**
 * \brief send a byte via I2c
 * \param buf pointer to data for transmission
 * \retval 0 failure
 * \retval 1 success
 * \remarks Precondition: 
 * \li i2c_status is set appropriately (START or RESTART)
 * \internal
 * Mst/Slv    _______ M ____M___ S M ________ 
 * SDA (Data)        |S|  data  |A|S|
 *                   |T|        |C|T|
 *                   |A|XXXXXXXX|K|P|
 */
int 
i2c_write(unsigned char *buf)
{
  unsigned int count = 0;
    
  /*
   * Send a start or restart bit if needed
   */
  if(i2c_status.bits.START)
    {
      I2CCONbits.SEN = 1;                 
      Nop();                          //A small delay for hardware to respond
      while(I2CCONbits.SEN);          //Wait till Start sequence is completed
    }
  else if(i2c_status.bits.RESTART)
    {
      I2CCONbits.RSEN = 1;                
      Nop();                          //A small delay for hardware to respond
      while(I2CCONbits.RSEN);         //Wait till Start sequence is completed
    }
    
  /*
   * Send the byte
   */
  I2CTRN = *buf;                  //Transmit register
  while(I2CSTATbits.TBF);         //Wait for transmit buffer to empty
    
  /*
   * Check if slave acknowledged
   */
  while(I2CSTATbits.ACKSTAT)
    {
      if(++count > ACK_TIMEOUT)
        {
          //Slave did not acknowledge, byte did not transmit sucessfully, 
          //send stop bit to reset i2c
          I2CCONbits.PEN = 1;
          Nop();                          //A small delay for hardware to respond
          while(I2CCONbits.PEN);          //Wait till stop sequence is completed
          i2cIdle();
          return 0;
        }
    }
  i2cIdle();

  /*
   * Send a stop bit if needed
   */
  if(i2c_status.bits.STOP)
    {
      I2CCONbits.PEN = 1;
      Nop();                          //A small delay for hardware to respond
      while(I2CCONbits.PEN);          //Wait till stop sequence is completed
      i2cIdle();
    }
  i2c_status.val = 0;             //Clear status
    
  return 1;
}


//---------------------------------------------------------------------------------------
/**
 * \brief read a byte via I2c
 * \param buf pointer to data for reception
 * \retval 0 failure
 * \retval 1 success
 * \remarks  Precondition: 
 * \li i2c_status is set appropriately (STOP if requred)
 * \li target device has been set appropriately
 * \internal
 * Mst/Slv     ____ ___S____ M M _____ 
 * SDA (Data)      |  data  |A|S|
 *                 |        |C|T|
 *                 |XXXXXXXX|K|P|
 */
int 
i2c_read(unsigned char *buf)
{
  /*
   * Get the byte
   */
  I2CCONbits.RCEN = 1;                    //Enable Receive
  while(I2CCONbits.RCEN);
  I2CSTATbits.I2COV = 0;                  //Clear receive overflow
  *buf = (unsigned char) I2CRCV;          //Access the receive buffer
    
  /*
   * Send Acknowledgment
   */
  I2CCONbits.ACKDT = (i2c_status.bits.NACK)? 1 : 0;
  I2CCONbits.ACKEN = 1;       //Send Acknowledgement/Not Acknowledgement
  i2cIdle();                  //I2C bus at idle state, awaiting transimission
    
  /*
   * Send a stop bit if needed
   */
  if(i2c_status.bits.STOP)
    {
      I2CCONbits.PEN = 1;
      Nop();                          //A small delay for hardware to respond
      while(I2CCONbits.PEN);          //Wait till stop sequence is completed
      i2cIdle();
    }

  i2c_status.val = 0;             //Clear status
  return 1;
}


//---------------------------------------------------------------------------------------
/**
 * \brief set the local parameters of i2c
 * \param request Request code - defined in ioctl.h
 * \param argp pointer for control config, I2C_SET_STATUS
 * \retval -1 failure
 * \retval 0 success
 */ 
int 
i2c_ioctl(int request, unsigned char* argp)
{
  switch(request)
    {
      case I2C_SET_STATUS:
        i2c_status.val = *argp;
        break;
      default:
        return -1;      //request code not recognised   
    }
  return 0;
}

#endif /* I2C_MOD */

/** @} */
/** @} */
