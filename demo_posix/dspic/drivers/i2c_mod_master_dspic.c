/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup i2c_mod_master_slave I2C MODULATION DSPIC (MASTER) 
 * @{
 * 
 * Control I2C MODULATION DSPIC (MASTER)
 * \li The I2C module uses SCL and SDA, located at pin 36 and 37 respectively.
 * \li All I2C devices shares a common communication speed (default: 400kHz) 
 * \li The driver has a POSIX-like interface with open(), read(), write(), ioctl()
 * \li When i2c has multiple devices, read(), write() cannot be used in ISR (Interrupt routine)
 */

/**
 * \file
 *  I2C MODULATION DSPIC (MASTER) Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

#ifdef I2C_MOD_MASTER_DSPIC

#include <define.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

/** Store control byte for transmit/receive */
static unsigned char mod_dspic_ctrl_byte = 0;
/** Store the data bytes for i2c communication */
typedef union
{
  float val;
  unsigned char byte[4];
} i2c_mod_dspic_data_t;
/** indicate the start and stop condition with the data in progress */
static i2c_mod_dspic_data_t i2c_data;
/** Store io setting */
static int mod_dspic_flag;

#if (I2C_NUM > 1)
#include <pthread.h>
/** for mutual exclusion of other device using I2C bus */
extern pthread_mutex_t i2c_mutex;
#endif /* I2C_NUM>1 */


/**
 * \brief Initialize I2C MODULATION DSPIC
 * \param flags accessing mode
 * \retval 0 dac opened
 */
int 
i2c_mod_master_open(int flags)
{
  mod_dspic_flag = flags;
  i2c_open();
  return 0;
}


/**
 * \brief write 4 bytes to mod_dspic_slave
 * \param buf pointer of data to write
 * \return number of bytes written
 * \retval 0 no data has been written
 * \retval 4 two bytes has been written
 * \retval -1 not opened for writing error (errno = EBADF)
 * 
 * \remarks
 * \li example
 * Mst/Slv    _______ M ___M___ M S ____M___ S ____M___ S ___M____ S ___M____ S ___M ___ S M ____
 * SDA (Data)        |S|       | |A|        |A|        |A|        |A|        |A|        |A|S|
 *                   |T|address|W|C|channelA|C| Data 0 |C| Data 1 |C| Data 2 |C| Data 3 |C|T|
 *                   |A|1011000|0|K|00010000|K|10101010|K|10101010|K|10101010|K|10101010|K|P|
 */
int 
i2c_mod_master_write(float *buf)
{
  //Perform Write if write operation is enabled
  if(mod_dspic_flag & O_RDWR || mod_dspic_flag & O_WRONLY)
    {
      unsigned int status, data;
      unsigned int error = 0;
      
      i2c_data.val = *buf;
    
#if (I2C_NUM > 1)
      if(pthread_mutex_lock(&i2c_mutex) == 0)
        {
#endif /* I2C_NUM>1 */
    
          //Send start bit, slave address
          status = I2C_START;
          i2c_ioctl(I2C_SET_STATUS, &status);
          data = (unsigned char) I2C_MOD_DSPIC_ADDR;
          if(i2c_write(&data) == 0) error = 1;
            
          //Send control byte: Channel select
          data = (unsigned char) mod_dspic_ctrl_byte;
          if(i2c_write(&data) == 0) error = 1;
            
          //Send Data 0 Byte
          data = (unsigned char) i2c_data.byte[0];
          if(i2c_write(&data) == 0) error = 1;

          //Send Data 1 Byte
          data = (unsigned char) i2c_data.byte[1];
          if(i2c_write(&data) == 0) error = 1;

          //Send Data 2 Byte
          data = (unsigned char) i2c_data.byte[2];
          if(i2c_write(&data) == 0) error = 1;
            
          //Send Data 3 with Stop bit
          status = I2C_STOP;
          i2c_ioctl(I2C_SET_STATUS, &status);
          data = (unsigned char) i2c_data.byte[3];
          if(i2c_write(&data) == 0) error = 1;
    
#if (I2C_NUM > 1)
          pthread_mutex_unlock(&i2c_mutex);
        }
      else
        {
          error = 1;  //i2c is busy
        }
#endif /* I2C_NUM>1 */
        
      i2c_data.val = 0;
      return (error == 1)? 0 : 4;
    }
  //Error, raise error flag
  else
    {
      errno = EBADF;  //io not opened for writing
      return -1;
    } 
}


/**
 * \brief read 4 bytes from mod dspic
 * \param pointer of data to read
 * \return number of bytes read
 * \retval 0 no data has been read
 * \retval 4 two bytes has been read
 * \retval -1 not opened for reading error (errno = EBADF)
 * 
 * \remarks
 * \li example
 * Mst/Slv    _______ M ___M___ M S ____M___ S M ___M___ M S ___S____ M ___S____ M ___S____ M ___S____ M M _____ 
 * SDA (Data)        |S|       | |A|        |A|R|       | |A|        |A|        |A|        |A|        |N|S|
 *                   |T|address|W|C|channelA|C|E|address|R|C| Data 0 |C| Data 1 |C| Data 2 |C| Data 3 |A|T|
 *                   |A|1011000|0|K|00010010|K|S|1011000|1|K|10101010|K|10101010|K|10101010|K|10101010|K|P|
 */
int 
i2c_mod_master_read(float *buf)
{
  //Perform Write if write operation is enabled
  if(mod_dspic_flag & O_RDWR || mod_dspic_flag & O_RDONLY)
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
          data = (unsigned char) I2C_MOD_DSPIC_ADDR;
          if(i2c_write(&data) == 0) error = 1;
        
          //Send control byte: Channel select
          data = (unsigned char) mod_dspic_ctrl_byte;
          if(i2c_write(&data) == 0) error = 1;
        
          //Send restart bit, slave address (Read Mode)
          status = I2C_RESTART;
          i2c_ioctl(I2C_SET_STATUS, &status);
          data = (unsigned char) (I2C_MOD_DSPIC_ADDR|0x01);
          if(i2c_write(&data) == 0) error = 1;
        
          //Receive Data 0 Byte with Acknowledgment
          if(i2c_read(&data) == 0) error = 1;
          i2c_data.byte[0] = (unsigned char) data;

          //Receive Data 1 Byte with Acknowledgment
          if(i2c_read(&data) == 0) error = 1;
          i2c_data.byte[1] = (unsigned char) data;

          //Receive Data 2 Byte with Acknowledgment
          if(i2c_read(&data) == 0) error = 1;
          i2c_data.byte[2] = (unsigned char) data;
        
          //Receive Data 3 Byte with Not Acknowledgment and stop bit
          status = I2C_NACK | I2C_STOP;
          i2c_ioctl(I2C_SET_STATUS, &status);
          if(i2c_read(&data) == 0) error = 1;
          i2c_data.byte[3] = (unsigned char) data;
    
#if (I2C_NUM > 1)
          pthread_mutex_unlock(&i2c_mutex); 
        }
      else
        {
          error = 1;  //i2c is busy
        }      
#endif /* I2C_NUM>1 */
    
      if(error == 1)
        {
          return 0;
        }

      *buf = i2c_data.val;
      i2c_data.val = 0;
      return 4;   
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
i2c_mod_master_ioctl(int request, unsigned char* argp)
{
  switch(request)
    {
      case MOD_DSPIC_SET_CTL:
        mod_dspic_ctrl_byte = *argp;
        break;
      default:
        return -1;      //request code not recognised   
    }
  return 0;
}

#endif /* I2C_MOD_MASTER_DSPIC */

