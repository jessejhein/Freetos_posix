/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup i2c_eeprom I2C EEPROM
 * @{
 * 
 * Control I2C EEPROM (24LC256) 32KB
 * \li The I2C module uses SCL and SDA, located at pin 36 and 37 respectively.
 * \li All I2C devices shares a common communication speed (default: 400kHz) 
 * \li The driver has a POSIX-like interface with open(), read(), write(), ioctl()
 * \li When i2c has multiple devices, read(), write() cannot be used in ISR (Interrupt routine)
 */

/**
 * \file
 * I2C EEPROM Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

#ifdef NVM_I2C 

#include <define.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

/************************************************************************************************
 * Local Variables
 ************************************************************************************************/
static unsigned int i2c_eeprom_pointer = 0;
static unsigned char i2c_eeprom_busy = 0;
static int i2c_eeprom_io_flag;

/************************************************************************************************
 * External Variables
 ************************************************************************************************/
#if (I2C_NUM > 1)
#include <pthread.h>
extern pthread_mutex_t i2c_mutex;
#endif /* I2C_NUM>1 */


/**
 * \brief open i2c eeprom
 * \param flags accessing mode
 * \retval 0 eeprom open
 * \remarks In NON-BLOCKING mode, application can only write upto 64 bytes at a time
 */
int 
i2c_eeprom_open(int flags)
{
  i2c_eeprom_io_flag = flags;
  i2c_open();
  return 0;
}


/**
 * \brief write $count bytes to eeprom
 * \param buf pointer of data to write to eeprom
 * \param count number of bytes to be written
 * \return number of bytes written
 * \retval 0 In non-blocking mode:
 * \n        i2c is busy (used by other device)
 * \n        communication fail (no acknowledgment received)
 * \n        In blocking mode:
 * \n        i2c is busy (used by other device)
 * \n        communication fail (no acknowledgment received)
 * \retval -1 In non-blocking mode:
 * \n         eeprom is busy (possibly not yet finished last write operation) (errno = EAGAIN)
 * \n         eeprom is not opened for reading (errno = EBADF)
 * \n         In blocking mode:
 * \n         eeprom is not opened for reading (errno = EBADF)
 * \remarks
 * In NON-BLOCKING mode, application can only write upto 64 bytes at a time
 * \verbatim
    Mst/Slv    _______ M ___M___ M S ____M___ S ____M___ S ____M___ S ____M___ S M ________ 
    SDA (Data)        |S|       | |A|        |A|        |A|        |A|        |A|S|
                      |T|address|W|C|High Add|C|Low Add |C| Data 0 |C| Data 1 |C|T|
                      |A|1010000|0|K|XFFFFFFF|K|FFFFFFFF|K|10101010|K|10101010|K|P|
   \endverbatim
 */
int 
i2c_eeprom_write(unsigned char* buf, int count)
{
  //Perform Write if write operation is enabled    
  if(i2c_eeprom_io_flag & O_RDWR || i2c_eeprom_io_flag & O_WRONLY)
    {
      int i;
      unsigned int status, data;
      unsigned int error = 0; //0= no error, 1=device fail, 2=eeprom busy
        
      i2c_eeprom_busy = 1;
      /*
       * Start to write data
       */
      for(i=0; i<count && i2c_eeprom_pointer<I2C_EEPROM_SIZE; i++, i2c_eeprom_pointer++)
        {
          /*
           * Determine if control byte and address bytes are required
           * (i.e. 1st byte OR byte starts at page boundary
           */
          if(i==0 || i2c_eeprom_pointer%I2C_EEPROM_PAGE_SIZE == 0)
            {
#if (I2C_NUM > 1)
              if(pthread_mutex_lock(&i2c_mutex) != 0) return 0; //i2c is busy
#endif /* I2C_NUM>1 */

              //NON-BLOCK mode, return -1, set errno = EAGAIN
              if(i2c_eeprom_io_flag & O_NONBLOCK)
                {
                  /*
                   * Send start bit, slave address (Write Mode)
                   */ 
                  status = I2C_START;
                  i2c_ioctl(I2C_SET_STATUS, &status);
                  data = (unsigned char) I2C_EEPROM_ADDR;
                  if(i2c_write(&data) == 0) error = 1;
                    
                  /*
                   * Send High Address
                   */
                  data = (i2c_eeprom_pointer >> 8) & 0xff;
                  if(i2c_write(&data) == 0) error = 2; 
                }
              //BLOCK mode, wait until eeprom is ready
              else
                {
                  do
                    {
                      /*
                       * Send start bit, slave address
                       */
                      status = I2C_START;
                      i2c_ioctl(I2C_SET_STATUS, &status);
                      data = (unsigned char) I2C_EEPROM_ADDR;
                      if(i2c_write(&data) == 0)
                        {
                          error = 1;
                          break;
                        }
                        
                      /*
                       * Send High Address
                       */
                      data = (i2c_eeprom_pointer >> 8) & 0xff;
                    }
                  while(i2c_write(&data) == 0);
                }
    
              /*
               * Send Low Address
               */
              data = i2c_eeprom_pointer & 0xff;
              if(i2c_write(&data) == 0) error = 1;
            }
            
          if(error > 0)
            {
              break;
            }
    
          /*
           * Determine if a stop bit is required
           * (i.e last byte OR byte at page boundary
           */
          if(i == count-1 || i2c_eeprom_pointer%I2C_EEPROM_PAGE_SIZE == I2C_EEPROM_PAGE_SIZE-1)
            {
              status = I2C_STOP;
              i2c_ioctl(I2C_SET_STATUS, &status);
            }
          if(i2c_write(buf+i) == 0)
            {
              error = 1;
              break;  //byte write failure exit loop
            }
#if (I2C_NUM > 1)
          if(i == count-1 || i2c_eeprom_pointer%I2C_EEPROM_PAGE_SIZE == I2C_EEPROM_PAGE_SIZE-1)
            {
              pthread_mutex_unlock(&i2c_mutex);
            }
#endif /* I2C_NUM>1 */
        }
    
#if (I2C_NUM > 1)
      if(error == 1)
        {
          pthread_mutex_unlock(&i2c_mutex);       
        }
      else if(error == 2)
        {
          //eeprom is busy, return -1 in non-blocking mode
          pthread_mutex_unlock(&i2c_mutex);
          errno = EAGAIN;
          return -1;
        }
#endif /* I2C_NUM>1 */
        
      i2c_eeprom_busy = 0;        
      return i;
    }
  //Error, raise error flag
  else
    {
      errno = EBADF;  //io not opened for writing
      return -1;
    }     
}


/**
 * \brief read count bytes from eeprom
 * \param buf pointer of data to read from eeprom
 * \param count number of bytes to be written
 * \return number of bytes read
 * \retval 0 In non-blocking mode:
 * \n        i2c is busy (used by other device)
 * \n        communication fail (no acknowledgment received)
 * \n        In blocking mode:
 * \n        i2c is busy (used by other device)
 * \n        communication fail (no acknowledgment received)
 * \retval -1 In non-blocking mode:
 * \n         eeprom is busy (possibly not yet finished last write operation) (errno = EAGAIN)
 * \n         eeprom is not opened for reading (errno = EBADF)
 * \n         In blocking mode:
 * \n         eeprom is not opened for reading (errno = EBADF)
 * \remarks
 * In NON-BLOCKING mode, application can only write upto 64 bytes at a time
 * \verbatim
    Mst/Slv    ___ M ___M___ M S ____M___ S ____M___ S M ___M___ M S ____S___ M ____S___ M M ____ 
    SDA (Data)    |S|       | |A|        |A|        |A|R|       | |A|        |A|        |N|S| 
                  |T|address|W|C|High Add|C|Low Add |C|E|address|R|C| Data 0 |C| Data 1 |A|T|
                  |A|1010000|0|K|XFFFFFFF|K|FFFFFFFF|K|S|1010000|1|K|10101010|K|10101010|K|P|
   \endverbatim
 */
int 
i2c_eeprom_read(unsigned char* buf, int count)
{
  //Perform Read if read operation is enabled
  if(i2c_eeprom_io_flag & O_RDWR || i2c_eeprom_io_flag & O_RDONLY)
    {
      int i;
      unsigned int status, data;
      unsigned int error = 0; //0= no error, 1=device fail, 2=eeprom busy

      i2c_eeprom_busy = 1;   
      /*
       * Start to read data
       */
      for(i=0; i<count && i2c_eeprom_pointer<I2C_EEPROM_SIZE; i++, i2c_eeprom_pointer++)
        {
          /*
           * Determine if control byte and address bytes are required
           * (i.e. 1st byte)
           */
          if(i==0)
            {
#if (I2C_NUM > 1)
              if(pthread_mutex_lock(&i2c_mutex) != 0) return 0; //i2c is busy
#endif /* I2C_NUM>1 */
                
              //NON-BLOCK mode, return -1, set errno = EAGAIN
              if(i2c_eeprom_io_flag & O_NONBLOCK)
                {
                  /*
                   * Send start bit, slave address (Write Mode)
                   */ 
                  status = I2C_START;
                  i2c_ioctl(I2C_SET_STATUS, &status);
                  data = (unsigned char) I2C_EEPROM_ADDR;
                  if(i2c_write(&data) == 0) error = 1;
                    
                  /*
                   * Send High Address
                   */
                  data = (i2c_eeprom_pointer >> 8) & 0xff;
                  if(i2c_write(&data) == 0) error = 2;                    
                }
              //BLOCK mode, wait until eeprom is ready
              else
                {
                  do
                    {
                      /*
                       * Send start bit, slave address
                       */
                      status = I2C_START;
                      i2c_ioctl(I2C_SET_STATUS, &status);
                      data = (unsigned char) I2C_EEPROM_ADDR;
                      if(i2c_write(&data) == 0)
                        {
                          error = 1;
                          break;
                        }
                        
                      /*
                       * Send High Address
                       */
                      data = (i2c_eeprom_pointer >> 8) & 0xff;
                    }
                  while(i2c_write(&data) == 0);                    
                }

              /*
               * Send Low Address
               */
              data = i2c_eeprom_pointer & 0xff;
              if(i2c_write(&data) == 0) error = 1;            
    
              /*
               * Send start bit, slave address (Read Mode)
               */ 
              status = I2C_RESTART;
              i2c_ioctl(I2C_SET_STATUS, &status);
              data = (unsigned char) (I2C_EEPROM_ADDR | 0x01);
              if(i2c_write(&data) == 0) error = 1;
            }
            
          if(error > 0)
            {
              break;
            }
    
          /*
           * Determine if (NACK+stop) is needed
           * (i.e last byte required by user OR end-of-file)
           */
          if(i == count-1 || i2c_eeprom_pointer == I2C_EEPROM_SIZE-1)
            {
              status = I2C_NACK | I2C_STOP;
            }
          i2c_ioctl(I2C_SET_STATUS, &status);
          if(i2c_read(buf+i) == 0)
            {
              error = 1;
              break;      //read failure, exit loop
            }
#if (I2C_NUM > 1)
          if(i == count-1 || i2c_eeprom_pointer == I2C_EEPROM_SIZE-1)
            {
              pthread_mutex_unlock(&i2c_mutex);       
            }
#endif /* I2C_NUM>1 */
        }
    
#if (I2C_NUM > 1)
      if(error == 1)
        {
          pthread_mutex_unlock(&i2c_mutex);
        }
      else if(error == 2)
        {
          //eeprom is busy, return -1 in non-blocking mode
          pthread_mutex_unlock(&i2c_mutex);
          errno = EAGAIN;
          return -1;            
        }
#endif /* I2C_NUM>1 */

      i2c_eeprom_busy = 0;
                
      return i;
    }
  //Error, raise error flag
  else
    {
      errno = EBADF;  //io not opened for reading
      return -1;
    }   
}


/**
 * \brief change the current pointer from base address
 * \param offset offset from location determined by whence
 * \param whence
 * \n SEEK_SET: BASE (0) + offset
 * \n SEEK_CUR: unimplement
 * \n SEEK_END: unimplement
 * \return current offset from base address (can be greater than I2C_EEPROM_SIZE)
 * \retval -1 another read/write is in progress
 */
int 
i2c_eeprom_lseek(int offset, int whence)
{
  if(i2c_eeprom_busy > 0)
    {
      errno = EAGAIN;
      return -1;
    }

  i2c_eeprom_pointer = offset;
  return i2c_eeprom_pointer;
}

#endif /* NVM_I2C */

/** @} */
/** @} */
