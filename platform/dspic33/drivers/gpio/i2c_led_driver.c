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


/** Store IO setting */
static int led_io_flag;
/** Store led status */
static unsigned char led_status;
/** Store address selected */
static unsigned char led_address;
/** Store channel selected */
static unsigned char led_ch;


int
i2c_led_driver_open (int flags)
{
  static char timeout = 0;

  led_io_flag = flags;
  i2c_open ();
  //adjust gain to 500uA
  led_address = I2C_LED_DRIVER_LED_GAIN_CTRL;
  unsigned char value = 0x0f;
  while (i2c_led_driver_write (&value) != 1)
    {
      //cannot communicate to driver, consider fail
      if (timeout > 50) return -1;
      timeout++;
    }
  //dim all channels to zero
  int i;
  led_address = I2C_LED_DRIVER_LED1_DIM_CTRL;
  for (i = 0; i < 8; i++)
    {
      value = 0;
      while (i2c_led_driver_write (&value) != 1);
      led_address++;
    }
  return 0;
}


int
i2c_led_driver_write (unsigned char *buf)
{
  //Perform Write if write operation is enabled
  if (led_io_flag & O_RDWR || led_io_flag & O_WRONLY)
    {
      unsigned int error = 0;
      unsigned char status = 0;
      unsigned char data = 0;

#if (I2C_NUM > 1)
      if (pthread_mutex_lock(&i2c_mutex) == 0)
        {
#endif /* I2C_NUM > 1 */

          //Send start bit, slave address
          status = I2C_START;
          i2c_ioctl (I2C_SET_STATUS, &status);
          data = (unsigned char) I2C_LED_DRIVER_ADDR;
          if (i2c_write (&data) == 0) error = 1;

          //Send control byte: Command byte
          data = (unsigned char) led_address;
          if (i2c_write (&data) == 0) error = 1;

          //Send data byte with Stop bit
          status = I2C_STOP;
          i2c_ioctl (I2C_SET_STATUS, &status);
          if (led_address == I2C_LED_DRIVER_LED_ON_CTRL)
            {
              if (*buf == 0)
                {
                  data = led_status & (~(0x01 << led_ch));
                }
              else
                {
                  data = led_status | (0x01 << led_ch);
                }
            }
          else
            {
              data = (unsigned char) *buf;
            }
          if (i2c_write (&data) == 0)
            {
              error = 1;
            }
          else
            {
              if (led_address == I2C_LED_DRIVER_LED_ON_CTRL) led_status = data;
            }

#if (I2C_NUM > 1)
          pthread_mutex_unlock (&i2c_mutex);
        }
      else
        {
          error = 1;  //i2c is busy
        }
#endif /* I2C_NUM > 1 */

      return (error == 1)? 0 : 1;
    }
  //Error, raise error flag
  else
    {
      errno = EBADF;  //IO not opened for writing
      return -1;
    }
}


int
i2c_led_driver_ioctl (int request, unsigned char* argp)
{
  if (*argp > 7) return -1;

  switch (request)
    {
      case I2C_LED_DRIVER_CH_DIM:
        {
          led_address = I2C_LED_DRIVER_LED1_DIM_CTRL + *argp;
          break;
        }
      case I2C_LED_DRIVER_CH_ON:
        {
          led_address = I2C_LED_DRIVER_LED_ON_CTRL;
          led_ch = *argp;
          break;
        }
      default:
        return -1;      //request code not recognised
    }
  return 0;
}

/** @} */
/** @} */

