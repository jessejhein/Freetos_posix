/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup i2c_eeprom I2C EEPROM
 * @{
 */

/**
 * \file
 * I2C EEPROM Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

#include <define.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <i2c/i2c.h>

/** store pointer to EEPROM */
static unsigned int i2c_eeprom_pointer = 0;
/** indicate EEPROM module is busy */
static unsigned char i2c_eeprom_busy = 0;
/** store io setting */
static int i2c_eeprom_io_flag;


int 
i2c_eeprom_open (int flags)
{
  i2c_eeprom_io_flag = flags;
  i2c_open ();
  return 0;
}


int 
i2c_eeprom_write (unsigned char* buf, int count)
{
  //Perform Write if write operation is enabled    
  if (i2c_eeprom_io_flag & O_RDWR || i2c_eeprom_io_flag & O_WRONLY)
    {
      i2c_eeprom_busy = 1;

      static int byte_written = 0;
      int i;
      unsigned int error = 0;
      unsigned char status = 0;
      unsigned char data = 0;
        
      /*
       * Start to write data
       */
      for (i = byte_written; (i < count) && (i2c_eeprom_pointer < I2C_EEPROM_SIZE); i++, i2c_eeprom_pointer++, byte_written++)
        {
          /*
           * Determine if control byte and address bytes are required
           * (i.e. 1st byte OR byte starts at page boundary
           */
          if ((i == 0) || (i2c_eeprom_pointer%I2C_EEPROM_PAGE_SIZE == 0))
            {
#if (I2C_NUM > 1)
              if (pthread_mutex_lock (&i2c_mutex) != 0) return 0; //i2c is busy
#endif /* I2C_NUM > 1 */

              //NON-BLOCK mode, return -1, set errno = EAGAIN
              if (i2c_eeprom_io_flag & O_NONBLOCK)
                {
                  /*
                   * Send start bit, slave address (Write Mode)
                   */ 
                  status = I2C_START;
                  i2c_ioctl (I2C_SET_STATUS, &status);
                  data = (unsigned char) I2C_EEPROM_ADDR;
                  if (i2c_write (&data) == 0) error = 1;
                    
                  /*
                   * Send High Address
                   */
                  data = (i2c_eeprom_pointer >> 8) & 0xff;
                  if (i2c_write (&data) == 0) error = 1;
                }
              //BLOCK mode, wait until EEPROM is ready
              else
                {
                  do
                    {
                      /*
                       * Send start bit, slave address
                       */
                      status = I2C_START;
                      i2c_ioctl (I2C_SET_STATUS, &status);
                      data = (unsigned char) I2C_EEPROM_ADDR;
                      if (i2c_write (&data) == 0)
                        {
                          error = 1;
                          break;
                        }
                        
                      /*
                       * Send High Address
                       */
                      data = (i2c_eeprom_pointer >> 8) & 0xff;
                    }
                  while (i2c_write (&data) == 0);
                }
    
              /*
               * Send Low Address
               */
              data = i2c_eeprom_pointer & 0xff;
              if (i2c_write (&data) == 0) error = 1;
            }
            
          if (error > 0) break;
    
          /*
           * Determine if a stop bit is required
           * (i.e last byte OR byte at page boundary
           */
          if ((i == count-1) || (i2c_eeprom_pointer%I2C_EEPROM_PAGE_SIZE == I2C_EEPROM_PAGE_SIZE-1))
            {
              status = I2C_STOP;
              i2c_ioctl (I2C_SET_STATUS, &status);
            }
          if (i2c_write (buf+i) == 0)
            {
              error = 1;
              break;  //byte write failure exit loop
            }
#if (I2C_NUM > 1)
          if ((i == count-1) || (i2c_eeprom_pointer%I2C_EEPROM_PAGE_SIZE == I2C_EEPROM_PAGE_SIZE-1))
            {
              pthread_mutex_unlock(&i2c_mutex);
            }
#endif /* I2C_NUM > 1 */
        }
    
#if (I2C_NUM > 1)
      if (error > 0)
        {
          //EEPROM is busy, return -1 in non-blocking mode
          pthread_mutex_unlock (&i2c_mutex);
          errno = EAGAIN;
          return -1;
        }
#endif /* I2C_NUM > 1 */
        
      i2c_eeprom_busy = 0;
      i = byte_written;
      byte_written = 0;
      return i;
    }
  //Error, raise error flag
  else
    {
      errno = EBADF;  //io not opened for writing
      return -1;
    }     
}


int 
i2c_eeprom_read (unsigned char* buf, int count)
{
  //Perform Read if read operation is enabled
  if ((i2c_eeprom_io_flag & O_RDWR) || (i2c_eeprom_io_flag & O_RDONLY))
    {
      int i;
      unsigned int error = 0;
      unsigned char status = 0;
      unsigned char data = 0;

      i2c_eeprom_busy = 1;   
      /*
       * Start to read data
       */
      for (i = 0; (i < count) && (i2c_eeprom_pointer < I2C_EEPROM_SIZE); i++, i2c_eeprom_pointer++)
        {
          /*
           * Determine if control byte and address bytes are required
           * (i.e. 1st byte)
           */
          if (i == 0)
            {
#if (I2C_NUM > 1)
              if (pthread_mutex_lock (&i2c_mutex) != 0) return 0; //i2c is busy
#endif /* I2C_NUM > 1 */
                
              //NON-BLOCK mode, return -1, set errno = EAGAIN
              if (i2c_eeprom_io_flag & O_NONBLOCK)
                {
                  /*
                   * Send start bit, slave address (Write Mode)
                   */ 
                  status = I2C_START;
                  i2c_ioctl (I2C_SET_STATUS, &status);
                  data = (unsigned char) I2C_EEPROM_ADDR;
                  if (i2c_write (&data) == 0) error = 1;
                    
                  /*
                   * Send High Address
                   */
                  data = (i2c_eeprom_pointer >> 8) & 0xff;
                  if (i2c_write (&data) == 0) error = 1;
                }
              //BLOCK mode, wait until EEPROM is ready
              else
                {
                  do
                    {
                      /*
                       * Send start bit, slave address
                       */
                      status = I2C_START;
                      i2c_ioctl (I2C_SET_STATUS, &status);
                      data = (unsigned char) I2C_EEPROM_ADDR;
                      if (i2c_write (&data) == 0)
                        {
                          error = 1;
                          break;
                        }
                        
                      /*
                       * Send High Address
                       */
                      data = (i2c_eeprom_pointer >> 8) & 0xff;
                    }
                  while (i2c_write (&data) == 0);
                }

              /*
               * Send Low Address
               */
              data = i2c_eeprom_pointer & 0xff;
              if (i2c_write (&data) == 0) error = 1;
    
              /*
               * Send start bit, slave address (Read Mode)
               */ 
              status = I2C_RESTART;
              i2c_ioctl (I2C_SET_STATUS, &status);
              data = (unsigned char) (I2C_EEPROM_ADDR | 0x01);
              if (i2c_write (&data) == 0) error = 1;
            }
            
          if(error > 0) break;
    
          /*
           * Determine if (NACK+stop) is needed
           * (i.e last byte required by user OR end-of-file)
           */
          if ((i == count-1) || (i2c_eeprom_pointer == I2C_EEPROM_SIZE-1))
            {
              status = I2C_NACK | I2C_STOP;
            }
          i2c_ioctl (I2C_SET_STATUS, &status);
          if (i2c_read (buf+i) == 0)
            {
              error = 1;
              break;      //read failure, exit loop
            }
#if (I2C_NUM > 1)
          if ((i == count-1) || (i2c_eeprom_pointer == I2C_EEPROM_SIZE-1))
            {
              pthread_mutex_unlock (&i2c_mutex);
            }
#endif /* I2C_NUM > 1 */
        }
    
#if (I2C_NUM > 1)
      if (error > 0)
        {
          pthread_mutex_unlock (&i2c_mutex);
          errno = EAGAIN;
          return -1;            
        }
#endif /* I2C_NUM > 1 */

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


int 
i2c_eeprom_lseek (int offset, int whence)
{
  if (i2c_eeprom_busy > 0)
    {
      errno = EAGAIN;
      return -1;
    }

  i2c_eeprom_pointer = offset;
  return i2c_eeprom_pointer;
}

/** @} */
/** @} */
