/**
 * \addtogroup drivers Drivers
 * @{
 *
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup i2c_gpio I2C GPIO
 * @{
 */

/**
 * \file
 * I2C GPIO Driver
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
#ifdef FILE_SYSTEM
#include <syslog.h>
#endif /* FILE_SYSTEM */

/*
 * Register address
 */
//------------------------------------------------------------------------
//Default bank = 0
#define I2C_GPIO_DEFAULT_IOCON          0x0B
  #define I2C_GPIO_IOCON_BANK           0x80
  #define I2C_GPIO_IOCON_SEQOP          0x20
//Default bank = 1 (Using)
#define I2C_GPIO_PORTA                  0x00
#define I2C_GPIO_PORTB                  0x10
//------------------------------------------------------------------------
#define I2C_GPIO_IODIR                  0x00            //I/O Direction
#define I2C_GPIO_IPOL                   0x01            //Input Polarity
#define I2C_GPIO_GPINTEN                0x02            //Interrupt On Change
#define I2C_GPIO_DEFVAL                 0x03            //Default Compare Value
#define I2C_GPIO_INTCON                 0x04            //Interrupt Compare against 0: Previous Status/1:Default Value
#define I2C_GPIO_IOCON                  0x05            //Configuration
#define I2C_GPIO_GPPU                   0x06            //Pull-up Resistor
#define I2C_GPIO_INTF                   0x07            //Interrupt Flag
#define I2C_GPIO_INTCAP                 0x08            //Interrupt Capture
#define I2C_GPIO_GPIO                   0x09            //Port
#define I2C_GPIO_OLAT                   0x0A            //Output Latch
//------------------------------------------------------------------------
#define I2C_GPIO_MAX_CHANNEL_PER_PORT   8
//------------------------------------------------------------------------
/** data type to store virtual IO register for a port */
struct I2C_GPIO_PORT_REG_T
{
  /** IO direction register */
  __u8 TRIS;
  /** Output latch register */
  __u8 LAT;
  /** open drain register */
  __u8 OD;
};
//------------------------------------------------------------------------


/** Store IO setting */
static __u8 i2c_gpio_io_flag;
/** Store register address selected */
static __u8 i2c_gpio_address;
/** Store the port selected */
static __u8 i2c_gpio_port;
/** Store whether the next operation is associated with OD register */
static __u8 i2c_od_flag;
/** Store local port A register value */
static struct I2C_GPIO_PORT_REG_T i2c_gpio_portA;
/** Store local port B register value */
static struct I2C_GPIO_PORT_REG_T i2c_gpio_portB;


/**
 * \brief perform a write to device
 * \remarks i2c_gpio_address and i2c_gpio_port must be set appropriately beforehand
 */
static int
i2c_gpio_lv_write (__u8* buf)
{
  __u8 error = 0;
#if (I2C_NUM > 1)
  if (pthread_mutex_lock (&i2c_mutex) == 0)
    {
#endif /* I2C_NUM > 1 */
      //Send start bit, slave address
      i2c_usr_status = I2C_START;
      i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
      i2c_usr_data = (__u8) I2C_GPIO_ADDR;
      if (i2c_write (&i2c_usr_data) == 0) error = 1;

      //Send control byte: Command byte
      i2c_usr_data = (__u8) i2c_gpio_address | i2c_gpio_port;
      if (i2c_write (&i2c_usr_data) == 0) error = 1;

      //Send i2c_usr_data with Stop bit
      i2c_usr_status = I2C_STOP;
      i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
      i2c_usr_data = (__u8) *buf;
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
  return (error == 1)? 0 : 1;
}


/**
 * \brief perform a read from device
 * \remarks i2c_gpio_address and i2c_gpio_port must be set appropriately beforehand
 */
static int
i2c_gpio_lv_read (__u8* buf)
{
  __u8 error = 0;
#if (I2C_NUM > 1)
  if (pthread_mutex_lock (&i2c_mutex) == 0)
    {
#endif /* I2C_NUM > 1 */
      //Send start bit, slave address (Write Mode)
      i2c_usr_status = I2C_START;
      i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
      i2c_usr_data = (__u8) I2C_GPIO_ADDR;
      if (i2c_write (&i2c_usr_data) == 0) error = 1;

      //Send control byte: Command byte
      i2c_usr_data = (__u8) i2c_gpio_address | i2c_gpio_port;
      if (i2c_write (&i2c_usr_data) == 0) error = 1;

      //Send restart bit, slave address (Read Mode)
      i2c_usr_status = I2C_RESTART;
      i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
      i2c_usr_data = (__u8) (I2C_GPIO_ADDR | 0x01);
      if (i2c_write (&i2c_usr_data) == 0) error = 1;

      //Receive data with Not Acknowledgement and stop bit
      i2c_usr_status = I2C_NACK | I2C_STOP;
      i2c_ioctl (I2C_SET_STATUS, &i2c_usr_status);
      if (i2c_read (&i2c_usr_data) == 0) error = 1;
      *buf = (__u8) i2c_usr_data;
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


int
i2c_gpio_open (int flags)
{
  i2c_gpio_io_flag = (__u8) flags;
  i2c_open ();

  //Bank = 1, no pointer increment
  i2c_gpio_address = I2C_GPIO_DEFAULT_IOCON;
  __u8 data = (I2C_GPIO_IOCON_BANK | I2C_GPIO_IOCON_SEQOP);

  //prepare timeout counter
  i2c_timeout_cnt = 0;
  while (i2c_gpio_lv_write (&data) != 1)
    {
      //cannot communicate to driver, consider fail
      if (i2c_timeout_cnt > I2C_TIMEOUT)
        {
#ifdef FILE_SYSTEM
          while (syslog_append ("INIT: MCP23017 [ERR] NO DEV"));
#endif /* FILE_SYSTEM */
          return -1;
        }
      i2c_timeout_cnt++;
    }

  //reset pull-up resistor status
  data = 0x00;
  i2c_gpio_address = I2C_GPIO_PORTA | I2C_GPIO_GPPU;
  while (i2c_gpio_lv_write (&data) != 1);
  i2c_gpio_address = I2C_GPIO_PORTB | I2C_GPIO_GPPU;
  while (i2c_gpio_lv_write (&data) != 1);

  //initialise all pins to input
  i2c_gpio_portA.TRIS = 0xff; i2c_gpio_portA.OD = 0x00;
  i2c_gpio_portB.TRIS = 0xff; i2c_gpio_portB.OD = 0x00;
  i2c_gpio_address = I2C_GPIO_PORTA | I2C_GPIO_IODIR;
  while (i2c_gpio_lv_write (&i2c_gpio_portA.TRIS) != 1);
  i2c_gpio_address = I2C_GPIO_PORTB | I2C_GPIO_IODIR;
  while (i2c_gpio_lv_write (&i2c_gpio_portB.TRIS) != 1);

  //reset LAT
  i2c_gpio_portA.LAT = 0x00;
  i2c_gpio_portB.LAT = 0x00;
  i2c_gpio_address = I2C_GPIO_PORTA | I2C_GPIO_OLAT;
  while (i2c_gpio_lv_write (&i2c_gpio_portA.LAT) != 1);
  i2c_gpio_address = I2C_GPIO_PORTB | I2C_GPIO_OLAT;
  while (i2c_gpio_lv_write (&i2c_gpio_portB.LAT) != 1);

  return 0;
}


/**
 * \brief update the pin status according to the open_drain/normal setting
 * \remarks this function will not update software lat[] status
 */
static void
update_lat_pin (void)
{
  struct I2C_GPIO_PORT_REG_T* port;
  if (i2c_gpio_port == I2C_GPIO_PORTB)
    {
      port = &i2c_gpio_portB;
    }
  else
    {
      port = &i2c_gpio_portA;
    }

  int i;
  __u8 tris_tmp = port->TRIS;
  __u8 lat_tmp = port->LAT;
  for (i = 0; i < I2C_GPIO_MAX_CHANNEL_PER_PORT; i++)
    {
      //pin is input
      if ((port->TRIS >> i) & 0x01)
        {
          //Set pin to zero
          lat_tmp &= ~(0x01 << i);
        }
      //pin is OD
      else if ((port->OD >> i) & 0x01)
        {
          //Set pin to zero
          lat_tmp &= ~(0x01 << i);

          //logic HIGH
          if ((port->LAT >> i) & 0x01)
            {
              //set pin to input for high impedance
              tris_tmp |= (0x01 << i);
            }
          //logic LOW
          else
            {
              //Set pin to output
              tris_tmp &= ~(0x01 << i);
            }
        }
      //pin is output
      else
        {
          //write status to LAT register directly
          if ((port->LAT >> i) & 0x01) lat_tmp |= (0x01 << i);
          else lat_tmp &= ~(0x01 << i);
        }
    }

  i2c_gpio_address = I2C_GPIO_OLAT;
  while (i2c_gpio_lv_write (&lat_tmp) != 1);

  i2c_gpio_address = I2C_GPIO_IODIR;
  while (i2c_gpio_lv_write (&tris_tmp) != 1);
}



int
i2c_gpio_write (__u8* buf)
{
  //Perform Write if write operation is enabled
  if ((i2c_gpio_io_flag & O_RDWR) || (i2c_gpio_io_flag & O_WRONLY))
    {
      //set OD
      if (i2c_od_flag)
        {
          i2c_od_flag = 0;
          if (i2c_gpio_port == I2C_GPIO_PORTB) i2c_gpio_portB.OD = *buf;
          else i2c_gpio_portA.OD = *buf;
        }
      //set TRIS
      else if (i2c_gpio_address == I2C_GPIO_IODIR)
        {
          while (i2c_gpio_lv_write (buf) != 1);
          if (i2c_gpio_port == I2C_GPIO_PORTB) i2c_gpio_portB.TRIS = *buf;
          else i2c_gpio_portA.TRIS = *buf;
        }
      //set pull up resistor status
      else if (i2c_gpio_address == I2C_GPIO_GPPU)
        {
          while (i2c_gpio_lv_write (buf) != 1);
        }
      //set LAT/PORT
      else
        {
          //update software LAT status
          if (i2c_gpio_port == I2C_GPIO_PORTB)
            {
              i2c_gpio_portB.LAT = *buf;
            }
          else
            {
              i2c_gpio_portA.LAT = *buf;
            }
          update_lat_pin ();
        }
    }
  //IO not opened for writing
  else
    {
      errno = EBADF;
      return -1;
    }
  return 1;
}


int
i2c_gpio_read (__u8* buf)
{
  //Perform Read if read operation is enabled
  if ((i2c_gpio_io_flag & O_RDWR) || !(i2c_gpio_io_flag & O_WRONLY))
    {
      //read OD
      if (i2c_od_flag)
        {
          if (i2c_gpio_port == I2C_GPIO_PORTB) *buf = i2c_gpio_portB.OD;
          else *buf = i2c_gpio_portA.OD;
          i2c_od_flag = 0;
        }
      //read TRIS
      else if (i2c_gpio_address == I2C_GPIO_IODIR)
        {
          if (i2c_gpio_port == I2C_GPIO_PORTB) *buf = i2c_gpio_portB.TRIS;
          else *buf = i2c_gpio_portA.TRIS;
        }
      //read from LAT
      else if (i2c_gpio_address == I2C_GPIO_OLAT)
        {
          if (i2c_gpio_port == I2C_GPIO_PORTB) *buf = i2c_gpio_portB.LAT;
          else *buf = i2c_gpio_portA.LAT;
        }
      //read pull up resistor status
      else if (i2c_gpio_address == I2C_GPIO_GPPU)
        {
          while (i2c_gpio_lv_read (buf) != 1);
        }
      //read from PORT
      else
        {
          while (i2c_gpio_lv_read (buf) != 1);
        }
    }
  //IO not opened for reading
  else
    {
      errno = EBADF;
      return -1;
    }
  return 1;
}


int
i2c_gpio_ioctl (int request, unsigned char* argp)
{
  switch (request)
    {
      //Read/Write IO Direction register for Port A
      case GPIO_SET_TRISA:
      case GPIO_GET_TRISA:
        {
          i2c_gpio_port = I2C_GPIO_PORTA;
          i2c_gpio_address = I2C_GPIO_IODIR;
          break;
        }
      //Read/Write IO Direction register for Port B
      case GPIO_SET_TRISB:
      case GPIO_GET_TRISB:
        {
          i2c_gpio_port = I2C_GPIO_PORTB;
          i2c_gpio_address = I2C_GPIO_IODIR;
          break;
        }
      //Read/Write Open Drain register for Port A
      case GPIO_SET_ODA:
      case GPIO_GET_ODA:
        {
          i2c_od_flag = 1;
          i2c_gpio_port = I2C_GPIO_PORTA;
          i2c_gpio_address = I2C_GPIO_IODIR;
          break;
        }
      //Read/Write Open Drain register for Port B
      case GPIO_SET_ODB:
      case GPIO_GET_ODB:
        {
          i2c_od_flag = 1;
          i2c_gpio_port = I2C_GPIO_PORTB;
          i2c_gpio_address = I2C_GPIO_IODIR;
          break;
        }
      //Read/Write Output Latch register for Port A
      case GPIO_SET_LATA:
      case GPIO_GET_LATA:
        {
          i2c_gpio_port = I2C_GPIO_PORTA;
          i2c_gpio_address = I2C_GPIO_OLAT;
          break;
        }
      //Read/Write Output Latch register for Port B
      case GPIO_SET_LATB:
      case GPIO_GET_LATB:
        {
          i2c_gpio_port = I2C_GPIO_PORTB;
          i2c_gpio_address = I2C_GPIO_OLAT;
          break;
        }
      //Write Port register for Port A
      case GPIO_SET_PORTA:
        {
          i2c_gpio_port = I2C_GPIO_PORTA;
          i2c_gpio_address = I2C_GPIO_OLAT;             //Avoid write to port directly
          break;
        }
      //Write Port register for Port B
      case GPIO_SET_PORTB:
        {
          i2c_gpio_port = I2C_GPIO_PORTB;
          i2c_gpio_address = I2C_GPIO_OLAT;             //Avoid write to port directly
          break;
        }
      //Read Port register for Port A
      case GPIO_GET_PORTA:
        {
          i2c_gpio_port = I2C_GPIO_PORTA;
          i2c_gpio_address = I2C_GPIO_GPIO;
          break;
        }
      //Read Port register for Port B
      case GPIO_GET_PORTB:
        {
          i2c_gpio_port = I2C_GPIO_PORTB;
          i2c_gpio_address = I2C_GPIO_GPIO;
          break;
        }
      //Read/Write Pull-up register for Port A
      case GPIO_SET_GPPUA:
      case GPIO_GET_GPPUA:
        {
          i2c_gpio_port = I2C_GPIO_PORTA;
          i2c_gpio_address = I2C_GPIO_GPPU;
          break;
        }
      //Read/Write Pull-up register for Port B
      case GPIO_SET_GPPUB:
      case GPIO_GET_GPPUB:
        {
          i2c_gpio_port = I2C_GPIO_PORTB;
          i2c_gpio_address = I2C_GPIO_GPPU;
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

