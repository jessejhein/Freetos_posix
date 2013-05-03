/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup i2c_adc I2C ADC
 * @{
 */

/**
 * \file
 * I2C ADC Driver
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


//------------------------------------------------------------------------
#define I2C_ADC_COMMAND                 0x00
#define I2C_ADC_MAX_DATA                4
//------------------------------------------------------------------------
/** Template to store the high and low byte for i2c communication */
struct I2C_ADC_DATA_T
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
};
//------------------------------------------------------------------------


/** Store IO setting */
static __u8 i2c_adc_io_flag;


int
i2c_adc_open (int flags)
{
  i2c_adc_io_flag = (__u8) flags;
  i2c_open ();
  return 0;
}


int
i2c_adc_read (__u16 *buf, int count)
{
  //Perform Read if read operation is enabled
  if ((i2c_adc_io_flag & O_RDWR) || !(i2c_adc_io_flag & O_WRONLY))
    {
      //convert byte count to word count
      count /= 2;

      __u8 error = 0;

      struct I2C_ADC_DATA_T i2c_adc_data[I2C_ADC_MAX_DATA];
#if (I2C_NUM > 1)
      if (pthread_mutex_lock (&i2c_mutex) == 0)
        {
#endif /* I2C_NUM > 1 */
          //Send start bit, slave address (Write Mode)
          i2c_usr_status = I2C_START;
          i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
          i2c_usr_data = (__u8) I2C_ADC_ADDR;
          if (i2c_write (&i2c_usr_data) == 0) error = 1;

          //Send command byte: start conversion
          i2c_usr_data = (__u8) I2C_ADC_COMMAND;
          if (i2c_write (&i2c_usr_data) == 0) error = 1;

          //Send restart bit, slave address (Read Mode)
          i2c_usr_status = I2C_RESTART;
          i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
          i2c_usr_data = (__u8) (I2C_ADC_ADDR | 0x01);
          if (i2c_write (&i2c_usr_data) == 0) error = 1;

          //Read upto 4 data bytes
          int i;
          for (i = 0; (i < count) && (i < I2C_ADC_MAX_DATA); i++)
            {
              //Receive High Byte with Acknowledgement
              if (i2c_read (&i2c_usr_data) == 0) error = 1;
              i2c_adc_data[i].byte.high = (__u8) i2c_usr_data;

              //Receive Low Byte with Not Acknowledgement (with Stop bit)
              i2c_usr_status = ((i < (count - 1)) && (i < (I2C_ADC_MAX_DATA - 1)))? I2C_RESTART : (I2C_NACK | I2C_STOP);
              i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
              if (i2c_read (&i2c_usr_data) == 0) error = 1;
              i2c_adc_data[i].byte.low = (__u8) i2c_usr_data;
            }
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

      int i;
      for (i = 0; (i < count) && (i < I2C_ADC_MAX_DATA); i++)
        {
          buf[i] = i2c_adc_data[i].value;
        }
      return 2 * i;
    }
  //IO not opened for reading
  else
    {
      errno = EROFS;
      return -1;
    }
}

/** @} */
/** @} */
