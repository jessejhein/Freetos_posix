/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup i2c_dac I2C DAC
 * @{
 */

/**
 * \file
 * I2C DAC Driver
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


/** Store control byte for transmit/receive */
static __u8 i2c_dac_ctrl_byte = 0;
/** Store IO setting */
static __u8 i2c_dac_io_flag;
/** Store the high and low byte for i2c communication */
static struct 
{
  union
    {
      __u16 value;
      struct
        {
          __u8 low;
          __u8 high;
        } byte;
    };
} i2c_dac_data;


#if (DAC_RESOLUTION == 10)
/**
 * \brief Convert 16-bit integer to 2-byte format for 10-bit DAC
 * \param _val value to convert
 * \verbatim
   value                           DAC format
                                   <--High---><--Low--->
                                   +----+----+----+----+
   0000 00AA BBBB CCCC   ------>   |AABB|BBCC|CC00|0000|
                                   +----+----+----+----+
   \endverbatim
 **/
#define int2dac(_val)                   (i2c_dac_data.value = ((__u16)_val << 6) & 0xFFC0)


/**
 * \brief Convert 16-bit integer from 2-byte format for 10-bit DAC
 * \return 10-bit resolution DAC value
 * \verbatim
   value                           DAC format
                                   <--High---><--Low--->
                                   +----+----+----+----+
   0000 00AA BBBB CCCC   <------   |AABB|BBCC|CC00|0000|
                                   +----+----+----+----+
   \endverbatim
 **/
#define dac2int()                       (__u16)((i2c_dac_data.value >> 6) & 0x3FF)
#elif (DAC_RESOLUTION == 12)
/**
 * \brief Convert 16-bit integer to 2-byte format for 12-bit DAC
 * \param _val value to convert
 * \verbatim
   value                           DAC format
                                   <--High---><--Low--->
                                   +----+----+----+----+
   0000 AAAA BBBB CCCC   ------>   |AAAA|BBBB|CCCC|0000|
                                   +----+----+----+----+
   \endverbatim
 **/
#define int2dac(_val)                   (i2c_dac_data.value = ((__u16)_val << 4) & 0xFFF0)


/**
 * \brief Convert 16-bit integer to 2-byte format for 12-bit DAC
 * \return 12-bit resolution DAC value
 * \verbatim
   value                           DAC format
                                   <--High---><--Low--->
                                   +----+----+----+----+
   0000 AAAA BBBB CCCC   <------   |AAAA|BBBB|CCCC|0000|
                                   +----+----+----+----+
   \endverbatim
 **/
#define dac2int()                       (__u16)((i2c_dac_data.value >> 4) & 0x0FFF)
#else /* DAC_RESOLUTION ERROR */
#error "ERROR in DAC_RESOLUTION setting"
#endif /* DAC_RESOLUTION ERROR */


int 
i2c_dac_open (int flags)
{
  i2c_dac_io_flag = (__u8) flags;
  i2c_open ();
  return 0;
}


int 
i2c_dac_write (__u16* buf)
{
  //Perform Write if write operation is enabled
  if ((i2c_dac_io_flag & O_RDWR) || (i2c_dac_io_flag & O_WRONLY))
    {
      __u8 error = 0;
#if (I2C_NUM > 1)
      if (pthread_mutex_lock (&i2c_mutex) == 0)
        {
#endif /* I2C_NUM > 1 */
          //Convert data to DAC format
          int2dac (buf[0]);

          //Send start bit, slave address
          i2c_usr_status = I2C_START;
          i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
          i2c_usr_data = (__u8) I2C_DAC_ADDR;
          if (i2c_write (&i2c_usr_data) == 0) error = 1;

          //Send control byte: Channel select
          i2c_usr_data = (__u8) i2c_dac_ctrl_byte;
          if (i2c_write (&i2c_usr_data) == 0) error = 1;

          //Send High Byte
          i2c_usr_data = (__u8) i2c_dac_data.byte.high;
          if (i2c_write (&i2c_usr_data) == 0) error = 1;

          //Send Low Byte with Stop bit
          i2c_usr_status = I2C_STOP;
          i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
          i2c_usr_data = (__u8) i2c_dac_data.byte.low;
          if (i2c_write (&i2c_usr_data) == 0) error = 1;
#if (I2C_NUM > 1)
          pthread_mutex_unlock (&i2c_mutex);
        }
      //i2c bus is busy
      else
        {
          error = 1;
        }
#endif /* I2C_NUM > 1 */
      return (error == 1)? 0 : 2;
    }
  //IO not opened for writing
  else
    {
      errno = EBADF;
      return -1;
    } 
}


int 
i2c_dac_read (__u16* buf)
{
  //Perform Read if read operation is enabled
  if ((i2c_dac_io_flag & O_RDWR) || !(i2c_dac_io_flag & O_WRONLY))
    {
      __u8 error = 0;
#if (I2C_NUM > 1)
      if (pthread_mutex_lock (&i2c_mutex) == 0)
        {
#endif /* I2C_NUM > 1 */
          //Send start bit, slave address (Write Mode)
          i2c_usr_status = I2C_START;
          i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
          i2c_usr_data = (__u8) I2C_DAC_ADDR;
          if (i2c_write (&i2c_usr_data) == 0) error = 1;

          //Send control byte: Channel select
          i2c_usr_data = (__u8) i2c_dac_ctrl_byte;
          if (i2c_write (&i2c_usr_data) == 0) error = 1;

          //Send restart bit, slave address (Read Mode)
          i2c_usr_status = I2C_RESTART;
          i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
          i2c_usr_data = (__u8) (I2C_DAC_ADDR | 0x01);
          if (i2c_write (&i2c_usr_data) == 0) error = 1;

          //Receive High Byte with Acknowledgement
          if (i2c_read (&i2c_usr_data) == 0) error = 1;
          i2c_dac_data.byte.high = (__u8) i2c_usr_data;

          //Receive Low Byte with Not Acknowledgement and stop bit
          i2c_usr_status = I2C_NACK | I2C_STOP;
          i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
          if (i2c_read (&i2c_usr_data) == 0) error = 1;
          i2c_dac_data.byte.low = (__u8) i2c_usr_data;
#if (I2C_NUM > 1)
          pthread_mutex_unlock (&i2c_mutex);
        }
      //i2c bus is busy
      else
        {
          error = 1;
        }      
#endif /* I2C_NUM > 1 */
      if (error == 1) return 0;

      //Convert data to integer format
      buf[0] = dac2int ();
      return 2;   
    }
  //IO not opened for reading
  else
    {
      errno = EBADF;
      return -1;
    } 
}


int 
i2c_dac_ioctl (int request, unsigned char* argp)
{
  switch (request)
    {
      //select DAC channel
      case DAC_SET_CTL:
        {
          i2c_dac_ctrl_byte = *argp;
          break;
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

