/**
 * \file
 * POSIX standard API for file stream
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
#include <errno.h>
#include <string.h>
#ifdef ETHERNET_MOD
#include <net/dm9000a.h>
#endif /* ETHERNET_MOD */
#include <analog/adc.h>
#include <analog/i2c_adc.h>
#include <analog/i2c_dac.h>
#include <analog/i2c_temp.h>
#include <gpio/i2c_led_driver.h>
#include <gpio/i2c_gpio.h>
#include <gpio/led.h>
#include <key/kb.h>
#include <nvm/flash_eeprom.h>
#include <nvm/fatfs.h>
#include <nvm/i2c_eeprom.h>
#include <pwm/i2c_mod_master_dspic.h>
#include <pwm/i2c_mod_slave_dspic.h>
#include <pwm/pwm.h>
#include <uart/uart.h>

//-----------------------------------------------------------------------------------------
/**
 * \brief string identifier for root path for open()
 * \remarks
 * For convenient, we use a single character to denote the path,
 * '>' for stdin
 * '?' for stdout
 * '@' for stderr
 * 'A' for 1st device
 * 'B' for 2nd device, etc.
 */
#define DEVICE_USR_PATH                 0x3E            //'A'-3


/**
 * Definition of File Descriptor
 * 
 * Only use 1 byte because fopen() in stdio.h can only handle file descriptor of 1 byte
 * 
 *  +-----+---+-----------+
 *  | 0 0 | T | i i i i i |
 *  +-----+---+-----------+
 *        |   |
 *        |   +--------------------- Index (0 - 31)
 *        |
 *        +------------------------- Type  (0 - 1)
 * 
 */
#define DEV_INDEX_MASK                  0x001F
#define DEV_TYPE_MASK                   0x0060
  #define DEV_TYPE_USR_DEFINED          0x0000
  #define DEV_TYPE_VOLUME0              0x0020


/**
 * \brief Default Root Folder
 */
#ifndef FS_ROOT
#define FS_ROOT                         "0:"
#endif /* FS_ROOT */

//-----------------------------------------------------------------------------------------
/**
 * \remarks
 * \li if pathname starts with FS_ROOT, goto file system
 * \li else check valid pathname spans from 'A' - 'Z' (i.e. support 26 devices at max)
 * \li return file handler 3 -28 respectively
 * \li file handler 0, 1, 2 are reserved for stdout, stdin, and stderr respectively     
 */
int _LIBC 
open (const char *pathname, int flags)
{
#ifdef FILE_SYSTEM
  //file system (for compatibility of fopen)
  if (strncmp (pathname, FS_ROOT, strlen (FS_ROOT)) == 0)
    {
      int index = fatfs_open (0, pathname, flags);
      if (index != -1)
        {
          return (DEV_TYPE_VOLUME0 | index);
        }
    }
  //user defined devices
  else
#endif /* FILE_SYSTEM */
    {
      int id = *pathname - DEVICE_USR_PATH;

      //STDOUT, STDIN, STDERR
      if (id < 3)
        {
          return id;
        }

#ifdef UART_MOD
      if (id < (BASE_COM + NO_OF_UART))
        {
          return (uart_open (id - BASE_COM, flags) == 0)? id : -1;
        }
#endif /* UART_MOD */

#ifdef I2C_DAC_MOD
      if (id == BASE_I2C_DAC)
        {
          return (i2c_dac_open (flags) == 0)? id : -1;
        }
#endif /* I2C_DAC_MOD */

#ifdef I2C_ADC_MOD
      if (id == BASE_I2C_ADC)
        {
          return (i2c_adc_open (flags) == 0)? id : -1;
        }
#endif /* I2C_ADC_MOD */

#ifdef I2C_TEMP_MOD
      if (id == BASE_I2C_TEMP)
        {
          return (i2c_temp_open (flags) == 0)? id : -1;
        }
#endif /* I2C_TEMP_MOD */

#ifdef NVM_MOD
      if (id == BASE_NVM)
        {
#ifdef NVM_FLASH
          return (flash_eeprom_open (flags) == 0)? id : -1;
#endif /* NVM_FLASH */
        }
#ifdef NVM_I2C
      if (id == (BASE_NVM + 1))
        {
          return (i2c_eeprom_open (flags) == 0)? id : -1;
        }
#endif /* NVM_I2C */
#endif /* NVM_MOD */

#ifdef I2C_MOD_MASTER_DSPIC
      if (id == BASE_I2C_MOD_DSPIC)
        {
          return (i2c_mod_master_open (flags) == 0)? id : -1;
        }
#endif /* I2C_MOD_MASTER_DSPIC */

#ifdef I2C_MOD_SLAVE_DSPIC
      if (id == BASE_I2C_MOD_DSPIC)
        {
          return (i2c_mod_slave_open (flags) == 0)? id : -1;
        }
#endif /* I2C_MOD_SLAVE_DSPIC */

#ifdef I2C_LED_DRIVER_MOD
      if (id == BASE_I2C_LED_DRIVER)
        {
          return (i2c_led_driver_open (flags) == 0)? id : -1;
        }
#endif /* I2C_LED_DRIVER_MOD */

#ifdef I2C_GPIO_MOD
      if (id == BASE_I2C_GPIO)
        {
          return (i2c_gpio_open (flags) == 0)? id : -1;
        }
#endif /* I2C_GPIO_MOD */

#ifdef ADC_MOD
      if (id == BASE_ADC)
        {
          return (adc_open (flags) == 0)? id : -1;
        }
#endif /* ADC_MOD */

#ifdef PWM_MOD
      if (id == BASE_PWM)
        {
          return (pwm_open (flags) == 0)? id : -1;
        }
#endif /* PWM_MOD */

#ifdef KB_MOD
      if (id == BASE_KB)
        {
          return (kb_open (flags) == 0)? id : -1;
        }
#endif /* KB_MOD */

#ifdef ETHERNET_MOD
      if (id == BASE_ETHERNET)
        {
          return (dmfe_open (flags) == 0)? id : -1;
        }
#endif /* ETHERNET_MOD */

#ifdef LED_MOD
      if (id == BASE_LED)
        {
          return (led_open (flags) == 0)? id : -1;
        }
#endif /* LED_MOD */

#ifdef LCD_MOD
      if (id == BASE_LCD)
        {
          return id;
        }
#endif /* LCD_MOD */
    }
  errno = ENXIO;
  return -1;
}



//-----------------------------------------------------------------------------------------
int _LIBC 
close (int fd)
{
  switch (fd & DEV_TYPE_MASK)
    {
      /*
       * User defined devices
       */
      case DEV_TYPE_USR_DEFINED:
        {
          fd &= DEV_INDEX_MASK;
#ifdef UART_MOD
          if (fd < (BASE_COM + NO_OF_UART))
            {
              return uart_close (fd - BASE_COM);
            }
#endif /* UART_MOD */

#ifdef ETHERNET_MOD
          if (fd == BASE_ETHERNET)
            {
              return dmfe_close ();
            }
#endif /* ETHERNET_MOD */
          break;
        }
#ifdef FILE_SYSTEM
      /*
       * DISK 0
       */
      case DEV_TYPE_VOLUME0:
        {
          fd &= DEV_INDEX_MASK;
          return fatfs_close (0, fd);
        }
#endif /* FILE_SYSTEM */
    }
  return -1;
}



//-----------------------------------------------------------------------------------------
int _LIBC 
write (int fd, void* buf, int count) 
{
  switch (fd & DEV_TYPE_MASK)
    {
      /*
       * User defined devices
       */
      case DEV_TYPE_USR_DEFINED:
        {
          fd &= DEV_INDEX_MASK;
          if (fd < 0)
            {
              return -1;
            }

          //DEBUG PORT
          if (fd < 3)
            {
              return uart_write (DEBUG_UART, buf, count);
            }

#ifdef UART_MOD
          if (fd < (BASE_COM + NO_OF_UART))
            {
              return uart_write (fd - BASE_COM, buf, count);
            }
#endif /* UART_MOD */

#ifdef I2C_DAC_MOD
          if (fd == BASE_I2C_DAC)
            {
              return i2c_dac_write (buf);
            }
#endif /* I2C_DAC_MOD */

#ifdef NVM_MOD
#ifdef NVM_FLASH
          if (fd == BASE_NVM)
            {
              return flash_eeprom_write (buf, count);
            }
#endif /* NVM_FLASH */
#ifdef NVM_I2C
          if (fd == (BASE_NVM + 1))
            {
              return i2c_eeprom_write (buf, count);
            }
#endif /* NVM_I2C */
#endif /* NVM_MOD */

#ifdef I2C_MOD_MASTER_DSPIC
          if (fd == BASE_I2C_MOD_DSPIC)
            {
              return i2c_mod_master_write (buf);
            }
#endif /* I2C_MOD_MASTER_DSPIC */

#ifdef I2C_MOD_SLAVE_DSPIC
          if (fd == BASE_I2C_MOD_DSPIC)
            {
              return i2c_mod_slave_write (buf);
            }
#endif /* I2C_MOD_SLAVE_DSPIC */

#ifdef I2C_LED_DRIVER_MOD
          if (fd == BASE_I2C_LED_DRIVER)
            {
              return i2c_led_driver_write (buf);
            }
#endif /* I2C_LED_DRIVER_MOD */

#ifdef I2C_GPIO_MOD
          if (fd == BASE_I2C_GPIO)
            {
              return i2c_gpio_write (buf);
            }
#endif /* I2C_GPIO_MOD */

#ifdef PWM_MOD
          if (fd == BASE_PWM)
            {
              return pwm_write (buf);
            }
#endif /* PWM_MOD */

#ifdef ETHERNET_MOD
          if (fd == BASE_ETHERNET)
            {
              return dmfe_write ();
            }
#endif /* ETHERNET_MOD */

#ifdef LED_MOD
          if (fd == BASE_LED)
            {
              return led_write (buf);
            }
#endif /* LED_MOD */

          //for all other fd > 3
          return 0;
        }
#ifdef FILE_SYSTEM
      /*
       * DISK 0
       */
      case DEV_TYPE_VOLUME0:
        {
          fd &= DEV_INDEX_MASK;
          return fatfs_write (0, fd, buf, count);
        }
#endif /* FILE_SYSTEM */
    }
  return -1;
}



//-----------------------------------------------------------------------------------------
int _LIBC 
read (int fd, void* buf, int count)
{
  switch (fd & DEV_TYPE_MASK)
    {
      /*
       * User defined devices
       */
      case DEV_TYPE_USR_DEFINED:
        {
          fd &= DEV_INDEX_MASK;

          if (fd < 3)
            {
              return -1;
            }

#ifdef UART_MOD
          if (fd < (BASE_COM + NO_OF_UART))
            {
              return uart_read (fd - BASE_COM, buf);
            }
#endif /* UART_MOD */

#ifdef I2C_DAC_MOD
          if (fd == BASE_I2C_DAC)
            {
              return i2c_dac_read (buf);
            }
#endif /* I2C_DAC_MOD */

#ifdef I2C_ADC_MOD
          if (fd == BASE_I2C_ADC)
            {
              return i2c_adc_read (buf, count);
            }
#endif /* I2C_ADC_MOD */

#ifdef I2C_TEMP_MOD
          if (fd == BASE_I2C_TEMP)
            {
              return i2c_temp_read (buf);
            }
#endif /* I2C_TEMP_MOD */

#ifdef NVM_MOD
#ifdef NVM_FLASH
          if (fd == BASE_NVM)
            {
              return flash_eeprom_read (buf, count);
            }
#endif /* NVM_FLASH */
#ifdef NVM_I2C
          if (fd == (BASE_NVM + 1))
            {
              return i2c_eeprom_read (buf, count);
            }
#endif /* NVM_I2C */
#endif /* NVM_MOD */

#ifdef I2C_MOD_MASTER_DSPIC
          if (fd == BASE_I2C_MOD_DSPIC)
            {
              return i2c_mod_master_read (buf);
            }
#endif /* I2C_MOD_MASTER_DSPIC */

#ifdef I2C_MOD_SLAVE_DSPIC
          if (fd == BASE_I2C_MOD_DSPIC)
            {
              return i2c_mod_slave_read (buf);
            }
#endif /* I2C_MOD_SLAVE_DSPIC */

#ifdef I2C_GPIO_MOD
          if (fd == BASE_I2C_GPIO)
            {
              return i2c_gpio_read (buf);
            }
#endif /* I2C_GPIO_MOD */

#ifdef ADC_MOD
          if (fd == BASE_ADC)
            {
              return adc_read (buf, count);
            }
#endif /* ADC_MOD */

#ifdef KB_MOD
          if (fd == BASE_KB)
            {
              return kb_read (buf);
            }
#endif /* KB_MOD */

#ifdef ETHERNET_MOD
          if (fd == BASE_ETHERNET)
            {
              return dmfe_read ();
            }
#endif /* ETHERNET_MOD */

          //for all other fd > 3
          return 0;
        }
#ifdef FILE_SYSTEM
      /*
       * DISK 0
       */
      case DEV_TYPE_VOLUME0:
        {
          fd &= DEV_INDEX_MASK;
          return fatfs_read (0, fd, buf, count);
        }
#endif /* FILE_SYSTEM */
    }
  return -1;
}



//-----------------------------------------------------------------------------------------
int _LIBC 
ioctl (int fd, int request, void* argp) 
{
  switch (fd & DEV_TYPE_MASK)
    {
      /*
       * User defined devices
       */
      case DEV_TYPE_USR_DEFINED:
        {
          fd &= DEV_INDEX_MASK;

          if (fd < 3)
            {
              return -1;
            }

#ifdef UART_MOD
          if (fd < (BASE_COM + NO_OF_UART))
            {
              return uart_ioctl (fd - BASE_COM, request, argp);
            }
#endif /* UART_MOD */

#ifdef I2C_DAC_MOD
          if (fd == BASE_I2C_DAC)
            {
              return i2c_dac_ioctl (request, argp);
            }
#endif /* I2C_DAC_MOD */

#ifdef I2C_ADC_MOD
          if (fd == BASE_I2C_ADC)
            {
              return i2c_adc_ioctl (request, argp);
            }
#endif /* I2C_ADC_MOD */

#ifdef I2C_MOD_MASTER_DSPIC
          if (fd == BASE_I2C_MOD_DSPIC)
            {
              return i2c_mod_master_ioctl (request, argp);
            }
#endif /* I2C_MOD_MASTER_DSPIC */

#ifdef I2C_MOD_SLAVE_DSPIC
          if (fd == BASE_I2C_MOD_DSPIC)
            {
              return i2c_mod_slave_ioctl (request, argp);
            }
#endif /* I2C_MOD_SLAVE_DSPIC */

#ifdef I2C_LED_DRIVER_MOD
          if (fd == BASE_I2C_LED_DRIVER)
            {
              return i2c_led_driver_ioctl (request, argp);
            }
#endif /* I2C_LED_DRIVER_MOD */

#ifdef I2C_GPIO_MOD
          if (fd == BASE_I2C_GPIO)
            {
              return i2c_gpio_ioctl (request, argp);
            }
#endif /* I2C_GPIO_MOD */

#ifdef ADC_MOD
          if (fd == BASE_ADC)
            {
              return adc_ioctl(request, argp);
            }
#endif /* ADC_MOD */

#ifdef PWM_MOD
          if (fd == BASE_PWM)
            {
              return pwm_ioctl (request, argp);
            }
#endif /* PWM_MOD */

#ifdef LED_MOD
          if (fd == BASE_LED)
            {
              return led_ioctl (request, argp);
            }
#endif /* LED_MOD */

#ifdef LCD_MOD
          if (fd == BASE_LCD)
            {
              return lcd_ioctl (request, argp);
            }
#endif /* LCD_MOD */

          //for all other fd > 3
          return 0;
        }
    }
  return -1;
}



//-----------------------------------------------------------------------------------------
int _LIBC 
lseek (int fd, int offset, int whence) 
{
  switch (fd & DEV_TYPE_MASK)
    {
      /*
       * User defined devices
       */
      case DEV_TYPE_USR_DEFINED:
        {
          fd &= DEV_INDEX_MASK;

          if (fd < 3)
            {
              return -1;
            }

#ifdef NVM_MOD
#ifdef NVM_FLASH
          if (fd == BASE_NVM)
            {
              return flash_eeprom_lseek (offset, whence);
            }
#endif /* NVM_FLASH */
#ifdef NVM_I2C
          if (fd == (BASE_NVM + 1))
            {
              return i2c_eeprom_lseek (offset, whence);
            }
#endif /* NVM_I2C */
#endif /* NVM_MOD */

          //for all other fd > 3
          return 0;
        }
#ifdef FILE_SYSTEM
      /*
       * DISK 0
       */
      case DEV_TYPE_VOLUME0:
        {
          fd &= DEV_INDEX_MASK;
          return fatfs_seek (0, fd, offset, whence);
        }
#endif /* FILE_SYSTEM */
    }
  return -1;
}
