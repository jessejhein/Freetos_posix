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
static unsigned char dac_ctrl_byte = 0;
/** Store the high and low byte for i2c communication */
static struct 
{
  unsigned char high;
  unsigned char low;
} dac_data;
/** Store IO setting */
static int dac_io_flag;

/** Split $value to high byte and low byte */
static void int2dac(unsigned int value);
/** Combine high byte to low byte to integer */
static int dac2int(void);


int 
i2c_dac_open (int flags)
{
  dac_io_flag = flags;
  i2c_open ();
  return 0;
}


int 
i2c_dac_write (unsigned int *buf)
{
  //Perform Write if write operation is enabled
  if (dac_io_flag & O_RDWR || dac_io_flag & O_WRONLY)
    {
      unsigned int error = 0;
      unsigned char status = 0;
      unsigned char data = 0;
    
      //Convert data to DAC format
      int2dac (buf[0]);
        
#if (I2C_NUM > 1)
      if (pthread_mutex_lock(&i2c_mutex) == 0)
        {
#endif /* I2C_NUM > 1 */
    
          //Send start bit, slave address
          status = I2C_START;
          i2c_ioctl (I2C_SET_STATUS, &status);
          data = (unsigned char) I2C_DAC_ADDR;
          if (i2c_write (&data) == 0) error = 1;
            
          //Send control byte: Channel select
          data = (unsigned char) dac_ctrl_byte;
          if (i2c_write (&data) == 0) error = 1;
            
          //Send High Byte
          data = (unsigned char) dac_data.high;
          if (i2c_write (&data) == 0) error = 1;
            
          //Send Low Byte with Stop bit
          status = I2C_STOP;
          i2c_ioctl (I2C_SET_STATUS, &status);
          data = (unsigned char) dac_data.low;
          if (i2c_write (&data) == 0) error = 1;
    
#if (I2C_NUM > 1)
          pthread_mutex_unlock (&i2c_mutex);
        }
      else
        {
          error = 1;  //i2c is busy
        }
#endif /* I2C_NUM > 1 */
        
      return (error == 1)? 0 : 2;
    }
  //Error, raise error flag
  else
    {
      errno = EBADF;  //io not opened for writing
      return -1;
    } 
}


int 
i2c_dac_read (unsigned int *buf)
{
  //Perform Write if write operation is enabled
  if (dac_io_flag & O_RDWR || dac_io_flag & O_RDONLY)
    {
      unsigned int error = 0;
      unsigned char status = 0;
      unsigned char data = 0;

#if (I2C_NUM > 1)
      if (pthread_mutex_lock (&i2c_mutex) == 0)
        {
#endif /* I2C_NUM > 1 */
    
          //Send start bit, slave address (Write Mode)
          status = I2C_START;
          i2c_ioctl (I2C_SET_STATUS, &status);
          data = (unsigned char) I2C_DAC_ADDR;
          if (i2c_write (&data) == 0) error = 1;
        
          //Send control byte: Channel select
          data = (unsigned char) dac_ctrl_byte;
          if (i2c_write (&data) == 0) error = 1;
        
          //Send restart bit, slave address (Read Mode)
          status = I2C_RESTART;
          i2c_ioctl (I2C_SET_STATUS, &status);
          data = (unsigned char) (I2C_DAC_ADDR|0x01);
          if (i2c_write (&data) == 0) error = 1;
        
          //Receive High Byte with Acknowledgement
          if (i2c_read (&data) == 0) error = 1;
          dac_data.high = (unsigned char) data;
        
          //Receive Low Byte with Not Acknowledgement and stop bit
          status = I2C_NACK | I2C_STOP;
          i2c_ioctl (I2C_SET_STATUS, &status);
          if (i2c_read (&data) == 0) error = 1;
          dac_data.low = (unsigned char) data;
    
#if (I2C_NUM > 1)
          pthread_mutex_unlock (&i2c_mutex);
        }
      else
        {
          error = 1;  //i2c is busy
        }      
#endif /* I2C_NUM > 1 */
    
      if (error == 1)
        return 0;
    
      //Convert data to integer format
      buf[0] = dac2int ();
    
      return 2;   
    }
  //Error, raise error flag
  else
    {
      errno = EBADF;  //io not opened for reading
      return -1;
    } 
}


int 
i2c_dac_ioctl (int request, unsigned char* argp)
{
  switch(request)
    {
      case DAC_SET_CTL:
        dac_ctrl_byte = *argp;
        break;
      default:
        return -1;      //request code not recognised   
    }
  return 0;
}


#if (DAC_RESOLUTION == 10) 
/********************************************************************
 * Convert 16-bit integer to/from 2-byte format for 10-bit DAC
 ********************************************************************/
static void 
int2dac (unsigned int value)
{
  dac_data.high = (unsigned char) ((value & 0x03FC) >> 2);  //Bit <9:2>
  dac_data.low = (unsigned char) ((value & 0x0003) << 6);   //Bit <1:0>
}

static int 
dac2int (void)
{
  int value = (dac_data.high << 8) + (dac_data.low & 0xFF);
  return ((value >> 6) & 0x3FF);
}
#elif  (DAC_RESOLUTION == 12)
/********************************************************************
 * Convert 16-bit integer to/from 2-byte format for 12-bit DAC
 ********************************************************************/
static void 
int2dac (unsigned int value)
{
  dac_data.high = (unsigned char) ((value & 0x0FF0) >> 4);  //Bit <11:4>
  dac_data.low = (unsigned char) ((value & 0x000F) << 4);   //Bit <3:0>
}

static int 
dac2int (void)
{
  int value = (dac_data.high << 8) + (dac_data.low & 0xFF);
  return ((value >> 4) & 0x0FFF);
}
#else /* DAC_RESOLUTION ERROR */
#error "ERROR in DAC_RESOLUTION setting"
#endif /* DAC_RESOLUTION ERROR */

/** @} */
/** @} */

