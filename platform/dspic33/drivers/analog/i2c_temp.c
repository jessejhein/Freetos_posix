/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup i2c_temp I2C Temperature Sensor
 * @{
 */

/**
 * \file
 * I2C Temperature Sensor Driver
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
//Register
#define TEMP_DATA_REGISTER              0x00
#define TEMP_CONFIG_REGISTER            0x01
#define TEMP_HYSTERESIS_REGISTER        0x02
#define TEMP_LIMIT_REGISTER             0x03
//------------------------------------------------------------------------
//Resolution
#if (I2C_TEMP_RESOLUTION == 9)
#define TEMP_RESOLUTION                 0x00
#elif (I2C_TEMP_RESOLUTION == 10)
#define TEMP_RESOLUTION                 0x20
#elif (I2C_TEMP_RESOLUTION == 11)
#define TEMP_RESOLUTION                 0x40
#else /* I2C_TEMP_RESOLUTION == 12 */
#define TEMP_RESOLUTION                 0x60
#endif /* I2C_TEMP_RESOLUTION == 12 */
//------------------------------------------------------------------------
/** Template to store high and low byte for i2c communication */
struct I2C_TEMP_DATA_T
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


/** Store I2C Temperature IC address */
static __u8 i2c_temp_addr;

/** Store IO setting */
static __u8 i2c_temp_io_flag[NO_OF_I2C_TEMP];


/**
 * \brief set the I2C Temperature IC address
 * \param id channel ID
 */
static void
i2c_temp_set_address (int id)
{
  switch (id)
    {
#if (NO_OF_I2C_TEMP > 1)
      case 1:
        i2c_temp_addr = (__u8) I2C_TEMP_ADDR1;
        break;
#endif /* NO_OF_I2C_TEMP > 1 */
#if (NO_OF_I2C_TEMP > 2)
      case 2:
        i2c_temp_addr = (__u8) I2C_TEMP_ADDR2;
        break;
#endif /* NO_OF_I2C_TEMP > 1 */
#if (NO_OF_I2C_TEMP > 3)
      case 2:
        i2c_temp_addr = (__u8) I2C_TEMP_ADDR3;
        break;
#endif /* NO_OF_I2C_TEMP > 1 */
      default:
        i2c_temp_addr = (__u8) I2C_TEMP_ADDR0;
        break;
    }
}


int
i2c_temp_open (int id, int flags)
{
  i2c_temp_io_flag[id] = (__u8) flags;
  i2c_open ();

  i2c_temp_set_address (id);

  /*
   * configure resolution
   */
  __u8 error = 0;
  //Send start bit, slave address (Write Mode)
  i2c_usr_status = I2C_START;
  i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
  i2c_usr_data = (__u8) i2c_temp_addr;
  if (i2c_write (&i2c_usr_data) == 0) error = 1;

  //Send register byte
  i2c_usr_data = (__u8) TEMP_CONFIG_REGISTER;
  if (i2c_write (&i2c_usr_data) == 0) error = 1;

  //Send value byte
  i2c_usr_status = I2C_STOP;
  i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
  i2c_usr_data = (__u8) TEMP_RESOLUTION;
  if (i2c_write (&i2c_usr_data) == 0) error = 1;

  //Send start bit, slave address (Write Mode)
  i2c_usr_status = I2C_START;
  i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
  i2c_usr_data = (__u8) i2c_temp_addr;
  if (i2c_write (&i2c_usr_data) == 0) error = 1;

  //Send register byte
  i2c_usr_data = (__u8) TEMP_DATA_REGISTER;
  if (i2c_write (&i2c_usr_data) == 0) error = 1;

  //read first temperature value
  __u16 value;
  if (i2c_temp_read (id, &value) == 0) error = 1;

  if (error == 1) return -1;
  return 0;
}


int
i2c_temp_read (int id, __u16* buf)
{
  //Perform Read if read operation is enabled
  if ((i2c_temp_io_flag[id] & O_RDWR) || !(i2c_temp_io_flag[id] & O_WRONLY))
    {
      __u8 error = 0;

      struct I2C_TEMP_DATA_T i2c_temp_data;
#if (I2C_NUM > 1)
      if (pthread_mutex_lock (&i2c_mutex) == 0)
        {
#endif /* I2C_NUM > 1 */

          i2c_temp_set_address (id);

          //Send start bit, slave address (Read Mode)
          i2c_usr_status = I2C_START;
          i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
          i2c_usr_data = (__u8) (i2c_temp_addr | 0x01);
          if (i2c_write (&i2c_usr_data) == 0) error = 1;

          //Receive High Byte with Acknowledgement
          if (i2c_read (&i2c_usr_data) == 0) error = 1;
          i2c_temp_data.byte.high = (__u8) i2c_usr_data;

          //Receive Low Byte with Not Acknowledgement and stop bit
          i2c_usr_status = I2C_NACK | I2C_STOP;
          i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
          if (i2c_read (&i2c_usr_data) == 0) error = 1;
          i2c_temp_data.byte.low = (__u8) i2c_usr_data;
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

      //export raw data
      *buf = i2c_temp_data.value;
      return 2;
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
