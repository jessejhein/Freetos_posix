/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup i2c_dac I2C DAC
 * @{
 * 
 * Control I2C DAC DAC6574/DAC7574
 * \li The I2C module uses SCL and SDA, located at pin 36 and 37 respectively.
 * \li All I2C devices shares a common communication speed (default: 400kHz) 
 * \li The driver has a POSIX-like interface with open(), read(), write(), ioctl()
 * \li When i2c has multiple devices, read(), write() cannot be used in ISR (Interrupt routine)
 */

/**
 * \file
 * I2C DAC Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

#ifdef I2C_DAC_MOD

#include <define.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

/** Store control byte for transmit/receive */
static unsigned char dac_ctrl_byte = 0;
/** Store the high and low byte for i2c communication */
static struct 
{
  unsigned char high;
  unsigned char low;
} dac_data;
/** Store io setting */
static int dac_io_flag;

#if (I2C_NUM > 1)
#include <pthread.h>
/** for mutual exclusion of other device using I2C bus */
extern pthread_mutex_t i2c_mutex;
#endif /* I2C_NUM>1 */

/** Split $value to high byte and low byte */
static void int2dac(unsigned int value);
/** Combine high byte to low byte to integer */
static int dac2int(void);


/**
 * \brief Initialize I2C DAC
 * \param flags accessing mode
 * \retval 0 dac opened
 */
int 
i2c_dac_open(int flags)
{
  dac_io_flag = flags;
  i2c_open();
  return 0;
}


/**
 * \brief write 2 bytes to dac
 * \param buf pointer of data to write to dac
 * \return number of bytes written
 * \retval 0 no data has been written
 * \retval 2 two bytes has been written
 * \retval -1 not opened for writing error (errno = EBADF)
 * 
 * \remarks
 * \li example DAC6574
 * \verbatim
    Mst/Slv    _______ M ___M___ M S ____M___ S ____M___ S ___M____ S M ________ 
    SDA (Data)        |S|       | |A|        |A|        |A|        |A|S|
                       |T|address|W|C|channelA|C| Data H |C| Data L |C|T|
                       |A|1001111|0|K|00010000|K|10101010|K|10XXXXXX|K|P|
   \endverbatim
 * \li example DAC7574
 * \verbatim
    Mst/Slv    _______ M ___M___ M S ____M___ S ____M___ S ___M____ S M ________ 
    SDA (Data)        |S|       | |A|        |A|        |A|        |A|S|
                      |T|address|W|C|channelA|C| Data H |C| Data L |C|T|
                      |A|1001111|0|K|00010000|K|10101010|K|1010XXXX|K|P|
   \endverbatim
 */
int 
i2c_dac_write(unsigned int *buf)
{
  //Perform Write if write operation is enabled
  if(dac_io_flag & O_RDWR || dac_io_flag & O_WRONLY)
    {
      unsigned int status, data;
      unsigned int error = 0;
    
      //Convert data to dac format
      int2dac(buf[0]);
        
#if (I2C_NUM > 1)
      if(pthread_mutex_lock(&i2c_mutex) == 0)
        {
#endif /* I2C_NUM>1 */
    
          //Send start bit, slave address
          status = I2C_START;
          i2c_ioctl(I2C_SET_STATUS, &status);
          data = (unsigned char) I2C_DAC_ADDR;
          if(i2c_write(&data) == 0) error = 1;
            
          //Send control byte: Channel select
          data = (unsigned char) dac_ctrl_byte;
          if(i2c_write(&data) == 0) error = 1;
            
          //Send High Byte
          data = (unsigned char) dac_data.high;
          if(i2c_write(&data) == 0) error = 1;
            
          //Send Low Byte with Stop bit
          status = I2C_STOP;
          i2c_ioctl(I2C_SET_STATUS, &status);
          data = (unsigned char) dac_data.low;
          if(i2c_write(&data) == 0) error = 1;
    
#if (I2C_NUM > 1)
          pthread_mutex_unlock(&i2c_mutex);
        }
      else
        {
            error = 1;  //i2c is busy
        }
#endif /* I2C_NUM>1 */
        
      return (error == 1)? 0 : 2;
    }
  //Error, raise error flag
  else
    {
      errno = EBADF;  //io not opened for writing
      return -1;
    } 
}


/**
 * \brief read 2 bytes from dac
 * \param buf pointer of data to read from dac
 * \return number of bytes read
 * \retval 0 no data has been read
 * \retval 2 two bytes has been read
 * \retval -1 not opened for reading error (errno = EBADF)
 * 
 * \remarks
 * \li example DAC6574
 * \verbatim
    Mst/Slv    _______ M ___M___ M S ____M___ S M ___M___ M S ___S____ M ___S____ M M _____ 
    SDA (Data)        |S|       | |A|        |A|R|       | |A|        |A|        |N|S|
                      |T|address|W|C|channelA|C|E|address|R|C| Data H |C| Data L |A|T|
                      |A|1001111|0|K|00010010|K|S|1001111|1|K|10101010|K|10XXXXXX|K|P|
   \endverbatim
 * \li example DAC7574
 * \verbatim
    Mst/Slv    _______ M ___M___ M S ____M___ S M ___M___ M S ___S____ M ___S____ M M _____ 
    SDA (Data)        |S|       | |A|        |A|R|       | |A|        |A|        |N|S|
                      |T|address|W|C|channelA|C|E|address|R|C| Data H |C| Data L |A|T|
                      |A|1001111|0|K|00010010|K|S|1001111|1|K|10101010|K|1010XXXX|K|P|
   \endverbatim
 */
int 
i2c_dac_read(unsigned int *buf)
{
  //Perform Write if write operation is enabled
  if(dac_io_flag & O_RDWR || dac_io_flag & O_RDONLY)
    {
      unsigned int status, data;
      unsigned int error = 0;
    
#if (I2C_NUM > 1)
      if(pthread_mutex_lock(&i2c_mutex) == 0)
        {
#endif /* I2C_NUM>1 */
    
          //Send start bit, slave address (Write Mode)
          status = I2C_START;
          i2c_ioctl(I2C_SET_STATUS, &status);
          data = (unsigned char) I2C_DAC_ADDR;
          if(i2c_write(&data) == 0) error = 1;
        
          //Send control byte: Channel select
          data = (unsigned char) dac_ctrl_byte;
          if(i2c_write(&data) == 0) error = 1;
        
          //Send restart bit, slave address (Read Mode)
          status = I2C_RESTART;
          i2c_ioctl(I2C_SET_STATUS, &status);
          data = (unsigned char) (I2C_DAC_ADDR|0x01);
          if(i2c_write(&data) == 0) error = 1;
        
          //Receive High Byte with Acknowledgment
          if(i2c_read(&data) == 0) error = 1;
          dac_data.high = (unsigned char) data;
        
          //Receive Low Byte with Not Acknowledgment and stop bit
          status = I2C_NACK | I2C_STOP;
          i2c_ioctl(I2C_SET_STATUS, &status);
          if(i2c_read(&data) == 0) error = 1;
          dac_data.low = (unsigned char) data;
    
#if (I2C_NUM > 1)
          pthread_mutex_unlock(&i2c_mutex); 
        }
      else
        {
            error = 1;  //i2c is busy
        }      
#endif /* I2C_NUM>1 */
    
      if(error == 1)
        return 0;
    
      //Convert data to integer format
      buf[0] = dac2int();
    
      return 2;   
    }
  //Error, raise error flag
  else
    {
      errno = EBADF;  //io not opened for reading
      return -1;
    } 
}


/**
 * \brief change setting for dac
 * \param request Request code defined in ioctl.h
 * \param argp pointer for control config, request code dependent.
 * \retval 0 success
 * \retval -1 error
 */
int 
i2c_dac_ioctl(int request, unsigned char* argp)
{
  switch(request)
    {
      case DAC_SET_CTL:
        dac_ctrl_byte = *argp;
        break;
      default:
        return -1;      //request code not recognised   
    }
  return 0;
}


#if (DAC_RESOLUTION == 10) 
/********************************************************************
 * Convert 16-bit integer to/from 2-byte format for 10-bit DAC
 ********************************************************************/
static void 
int2dac(unsigned int value)
{
  dac_data.high = (unsigned char) ((value & 0x03FC) >> 2);  //Bit <9:2>
  dac_data.low = (unsigned char) ((value & 0x0003) << 6);   //Bit <1:0>
}

static int 
dac2int(void)
{
  int value = (dac_data.high << 8) + (dac_data.low & 0xFF);
  return ((value >> 6) & 0x3FF);
}
#elif  (DAC_RESOLUTION == 12)
/********************************************************************
 * Convert 16-bit integer to/from 2-byte format for 12-bit DAC
 ********************************************************************/
static void 
int2dac(unsigned int value)
{
  dac_data.high = (unsigned char) ((value & 0x0FF0) >> 4);  //Bit <11:4>
  dac_data.low = (unsigned char) ((value & 0x000F) << 4);   //Bit <3:0>
}

static int 
dac2int(void)
{
  int value = (dac_data.high << 8) + (dac_data.low & 0xFF);
  return ((value >> 4) & 0x0FFF);
}
#else /* DAC_RESOLUTION ERROR */
#error "ERROR in DAC_RESOLUTION setting"
#endif /* DAC_RESOLUTION ERROR */

#endif /* I2C_DAC_MOD */

/** @} */
/** @} */

