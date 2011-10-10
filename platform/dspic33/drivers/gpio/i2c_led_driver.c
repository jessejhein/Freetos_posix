/**
 * \addtogroup drivers Drivers
 * @{
 *
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup i2c_led_driver I2C LED Driver
 * @{
 */

/**
 * \file
 * I2C LED Driver
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

/*
 * Register address
 */
/** LED ON/OFF Control */
#define I2C_LED_DRIVER_LED_ON_CTRL              0x00
/** LED1 Dimming Control */
#define I2C_LED_DRIVER_LED1_DIM_CTRL            0x01
/** LED2 Dimming Control */
#define I2C_LED_DRIVER_LED2_DIM_CTRL            0x02
/** LED3 Dimming Control */
#define I2C_LED_DRIVER_LED3_DIM_CTRL            0x03
/** LED4 Dimming Control */
#define I2C_LED_DRIVER_LED4_DIM_CTRL            0x04
/** LED5 Dimming Control */
#define I2C_LED_DRIVER_LED5_DIM_CTRL            0x05
/** LED6 Dimming Control */
#define I2C_LED_DRIVER_LED6_DIM_CTRL            0x06
/** LED7 Dimming Control */
#define I2C_LED_DRIVER_LED7_DIM_CTRL            0x07
/** LED8 Dimming Control */
#define I2C_LED_DRIVER_LED8_DIM_CTRL            0x08
/** LED Gain Control */
#define I2C_LED_DRIVER_LED_GAIN_CTRL            0x09
//--------------------------------------------------
#define I2C_LED_DRIVER_MAX_CHANNEL              8


/** Store IO setting */
static __u8 i2c_led_io_flag;
/** Store led status of 8 channels */
static __u8 i2c_led_status;
/** Store register address selected */
static __u8 i2c_led_address;
/** Store channel selected */
static __u8 i2c_led_ch;


int
i2c_led_driver_open (int flags)
{
  i2c_led_io_flag = (__u8) flags;
  i2c_open ();

  //adjust gain to 500uA
  i2c_led_address = I2C_LED_DRIVER_LED_GAIN_CTRL;
  __u8 value = 0x0f;

  //prepare timeout counter
  i2c_timeout_cnt = 0;
  while (i2c_led_driver_write (&value) != 1)
    {
      //cannot communicate to driver, consider fail
      if (i2c_timeout_cnt > I2C_TIMEOUT) return -1;
      i2c_timeout_cnt++;
    }

  //dim all channels to zero
  int i;
  i2c_led_address = I2C_LED_DRIVER_LED1_DIM_CTRL;
  for (i = 0; i < I2C_LED_DRIVER_MAX_CHANNEL; i++)
    {
      value = 0;
      while (i2c_led_driver_write (&value) != 1);
      i2c_led_address++;
    }
  return 0;
}


int
i2c_led_driver_write (__u8* buf)
{
  //Perform Write if write operation is enabled
  if ((i2c_led_io_flag & O_RDWR) || (i2c_led_io_flag & O_WRONLY))
    {
      __u8 error = 0;
#if (I2C_NUM > 1)
      if (pthread_mutex_lock (&i2c_mutex) == 0)
        {
#endif /* I2C_NUM > 1 */
          //Send start bit, slave address
          i2c_usr_status = I2C_START;
          i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
          i2c_usr_data = (__u8) I2C_LED_DRIVER_ADDR;
          if (i2c_write (&i2c_usr_data) == 0) error = 1;

          //Send control byte: Command byte
          i2c_usr_data = (__u8) i2c_led_address;
          if (i2c_write (&i2c_usr_data) == 0) error = 1;

          //Send data byte with Stop bit
          i2c_usr_status = I2C_STOP;
          i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
          if (i2c_led_address == I2C_LED_DRIVER_LED_ON_CTRL)
            {
              if (*buf == 0)
                {
                  i2c_usr_data = i2c_led_status & (~(0x01 << i2c_led_ch));
                }
              else
                {
                  i2c_usr_data = i2c_led_status | (0x01 << i2c_led_ch);
                }
            }
          else
            {
              i2c_usr_data = (__u8) *buf;
            }
          if (i2c_write (&i2c_usr_data) == 0)
            {
              error = 1;
            }
          else
            {
              if (i2c_led_address == I2C_LED_DRIVER_LED_ON_CTRL) i2c_led_status = i2c_usr_data;
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
      return (error == 1)? 0 : 1;
    }
  //IO not opened for writing
  else
    {
      errno = EBADF;
      return -1;
    }
}


int
i2c_led_driver_ioctl (int request, unsigned char* argp)
{
  if (*argp >= I2C_LED_DRIVER_MAX_CHANNEL) return -1;

  switch (request)
    {
      //select the channel to perform dimming
      case I2C_LED_DRIVER_CH_DIM:
        {
          i2c_led_address = I2C_LED_DRIVER_LED1_DIM_CTRL + *argp;
          break;
        }
      //select the channel to turn on
      case I2C_LED_DRIVER_CH_ON:
        {
          i2c_led_address = I2C_LED_DRIVER_LED_ON_CTRL;
          i2c_led_ch = *argp;
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

