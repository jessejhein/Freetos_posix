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
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <i2c/i2c.h>

/** store pointer to EEPROM */
static __u16 i2c_eeprom_pointer = 0;
/** indicate EEPROM module is busy */
static __u8 i2c_eeprom_busy = 0;
/** store IO setting */
static __u8 i2c_eeprom_io_flag;


int 
i2c_eeprom_open (int flags)
{
  i2c_eeprom_io_flag = (__u8) flags;
  i2c_open ();
  return 0;
}


int 
i2c_eeprom_write (__u8* buf, int count)
{
  //Perform Write if write operation is enabled    
  if ((i2c_eeprom_io_flag & O_RDWR) || (i2c_eeprom_io_flag & O_WRONLY))
    {
      i2c_eeprom_busy = 1;

      //keeps the number of bytes written (for NON-BLOCK mode)
      static __u16 byte_written = 0;
      __u16 i;
      __u8 error = 0;

      /*
       * Start to write data
       */
      for (i = byte_written; (i < count) && (i2c_eeprom_pointer < I2C_EEPROM_SIZE); i++, i2c_eeprom_pointer++, byte_written++)
        {
          /*
           * Determine if control byte and address bytes are required
           * (i.e. 1st byte OR byte starts at page boundary
           */
          if ((i == 0) || ((i2c_eeprom_pointer % I2C_EEPROM_PAGE_SIZE) == 0))
            {
#if (I2C_NUM > 1)
              //i2c bus is busy
              if (pthread_mutex_lock (&i2c_mutex) != 0) return 0;
#endif /* I2C_NUM > 1 */

              //acknowledgement polling
              do
                {
                  /*
                   * Send start bit, slave address (Write Mode)
                   */
                  i2c_usr_status = I2C_START;
                  i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
                  i2c_usr_data = (__u8) I2C_EEPROM_ADDR;
                }
              while (i2c_write (&i2c_usr_data) == 0);

              /*
               * Send High Address
               */
              i2c_usr_data = (i2c_eeprom_pointer >> 8) & 0xff;
              if (i2c_write (&i2c_usr_data) == 0) error = 1;

              /*
               * Send Low Address
               */
              i2c_usr_data = i2c_eeprom_pointer & 0xff;
              if (i2c_write (&i2c_usr_data) == 0) error = 1;
            }

          if (error > 0) break;

          /*
           * Determine if a stop bit is required
           * (i.e last byte OR byte at page boundary
           */
          if ((i == (count - 1)) || ((i2c_eeprom_pointer % I2C_EEPROM_PAGE_SIZE) == (I2C_EEPROM_PAGE_SIZE - 1)))
            {
              i2c_usr_status = I2C_STOP;
              i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
            }
          //byte write failure exit loop
          if (i2c_write (buf + i) == 0)
            {
              error = 1;
              break;
            }
#if (I2C_NUM > 1)
          if ((i == (count - 1)) || ((i2c_eeprom_pointer % I2C_EEPROM_PAGE_SIZE) == (I2C_EEPROM_PAGE_SIZE - 1)))
            {
              pthread_mutex_unlock (&i2c_mutex);
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
  //IO not opened for writing
  else
    {
      errno = EBADF;
      return -1;
    }     
}


int 
i2c_eeprom_read (__u8* buf, int count)
{
  //Perform Read if read operation is enabled
  if ((i2c_eeprom_io_flag & O_RDWR) || !(i2c_eeprom_io_flag & O_WRONLY))
    {
      __u16 i;
      __u8 error = 0;

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
              //i2c  bus is busy
              if (pthread_mutex_lock (&i2c_mutex) != 0) return 0;
#endif /* I2C_NUM > 1 */

              //acknowledgement polling
              do
                {
                  /*
                   * Send start bit, slave address (Write Mode)
                   */
                  i2c_usr_status = I2C_START;
                  i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
                  i2c_usr_data = (__u8) I2C_EEPROM_ADDR;
                }
              while (i2c_write (&i2c_usr_data) == 0);

              /*
               * Send High Address
               */
              i2c_usr_data = (i2c_eeprom_pointer >> 8) & 0xff;
              if (i2c_write (&i2c_usr_data) == 0) error = 1;

              /*
               * Send Low Address
               */
              i2c_usr_data = i2c_eeprom_pointer & 0xff;
              if (i2c_write (&i2c_usr_data) == 0) error = 1;

              /*
               * Send start bit, slave address (Read Mode)
               */ 
              i2c_usr_status = I2C_RESTART;
              i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
              i2c_usr_data = (__u8) (I2C_EEPROM_ADDR | 0x01);
              if (i2c_write (&i2c_usr_data) == 0) error = 1;
            }

          if (error > 0) break;

          /*
           * Determine if (NACK+stop) is needed
           * (i.e last byte required by user OR end-of-file)
           */
          if ((i == (count - 1)) || (i2c_eeprom_pointer == (I2C_EEPROM_SIZE - 1)))
            {
              i2c_usr_status = I2C_NACK | I2C_STOP;
            }
          i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
          if (i2c_read (buf + i) == 0)
            {
              error = 1;
              break;      //read failure, exit loop
            }
#if (I2C_NUM > 1)
          if ((i == (count - 1)) || (i2c_eeprom_pointer == (I2C_EEPROM_SIZE - 1)))
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
  //IO not opened for reading
  else
    {
      errno = EBADF;
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
