/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup i2c_mod_master I2C MODULATION DSPIC (MASTER) 
 * @{
 */

/**
 * \file
 *  I2C MODULATION DSPIC (MASTER) Driver
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
static unsigned char mod_dspic_ctrl_byte = 0;
/** Store the data bytes for i2c communication */
typedef union
{
  float val;
  unsigned char byte[4];
} i2c_mod_dspic_data_t;
/** indicate the start and stop condition with the data in progress */
static i2c_mod_dspic_data_t i2c_data;
/** Store IO setting */
static int mod_dspic_flag;


int 
i2c_mod_master_open (int flags)
{
  mod_dspic_flag = flags;
  i2c_open ();
  return 0;
}


int 
i2c_mod_master_write (float *buf)
{
  //Perform Write if write operation is enabled
  if (mod_dspic_flag & O_RDWR || mod_dspic_flag & O_WRONLY)
    {
      unsigned int error = 0;
      unsigned char status = 0;
      unsigned char data = 0;
      
      i2c_data.val = *buf;
    
#if (I2C_NUM > 1)
      if (pthread_mutex_lock (&i2c_mutex) == 0)
        {
#endif /* I2C_NUM > 1 */
    
          //Send start bit, slave address
          status = I2C_START;
          i2c_ioctl (I2C_SET_STATUS, &status);
          data = (unsigned char) I2C_MOD_DSPIC_ADDR;
          if (i2c_write (&data) == 0) error = 1;
            
          //Send control byte: Channel select
          data = (unsigned char) mod_dspic_ctrl_byte;
          if(i2c_write(&data) == 0) error = 1;
            
          //Send Data 0 Byte
          data = (unsigned char) i2c_data.byte[0];
          if (i2c_write (&data) == 0) error = 1;

          //Send Data 1 Byte
          data = (unsigned char) i2c_data.byte[1];
          if (i2c_write (&data) == 0) error = 1;

          //Send Data 2 Byte
          data = (unsigned char) i2c_data.byte[2];
          if (i2c_write (&data) == 0) error = 1;
            
          //Send Data 3 with Stop bit
          status = I2C_STOP;
          i2c_ioctl(I2C_SET_STATUS, &status);
          data = (unsigned char) i2c_data.byte[3];
          if (i2c_write (&data) == 0) error = 1;
    
#if (I2C_NUM > 1)
          pthread_mutex_unlock (&i2c_mutex);
        }
      else
        {
          error = 1;  //i2c is busy
        }
#endif /* I2C_NUM > 1 */
        
      i2c_data.val = 0;
      return (error == 1)? 0 : 4;
    }
  //Error, raise error flag
  else
    {
      errno = EBADF;  //io not opened for writing
      return -1;
    } 
}


int 
i2c_mod_master_read (float *buf)
{
  //Perform Write if write operation is enabled
  if (mod_dspic_flag & O_RDWR || mod_dspic_flag & O_RDONLY)
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
          i2c_ioctl(I2C_SET_STATUS, &status);
          data = (unsigned char) I2C_MOD_DSPIC_ADDR;
          if (i2c_write (&data) == 0) error = 1;
        
          //Send control byte: Channel select
          data = (unsigned char) mod_dspic_ctrl_byte;
          if (i2c_write (&data) == 0) error = 1;
        
          //Send restart bit, slave address (Read Mode)
          status = I2C_RESTART;
          i2c_ioctl (I2C_SET_STATUS, &status);
          data = (unsigned char) (I2C_MOD_DSPIC_ADDR|0x01);
          if (i2c_write (&data) == 0) error = 1;
        
          //Receive Data 0 Byte with Acknowledgement
          if (i2c_read (&data) == 0) error = 1;
          i2c_data.byte[0] = (unsigned char) data;

          //Receive Data 1 Byte with Acknowledgement
          if (i2c_read (&data) == 0) error = 1;
          i2c_data.byte[1] = (unsigned char) data;

          //Receive Data 2 Byte with Acknowledgement
          if (i2c_read (&data) == 0) error = 1;
          i2c_data.byte[2] = (unsigned char) data;
        
          //Receive Data 3 Byte with Not Acknowledgement and stop bit
          status = I2C_NACK | I2C_STOP;
          i2c_ioctl (I2C_SET_STATUS, &status);
          if (i2c_read(&data) == 0) error = 1;
          i2c_data.byte[3] = (unsigned char) data;
    
#if (I2C_NUM > 1)
          pthread_mutex_unlock (&i2c_mutex);
        }
      else
        {
          error = 1;  //i2c is busy
        }      
#endif /* I2C_NUM > 1 */
    
      if(error == 1)
        {
          return 0;
        }

      *buf = i2c_data.val;
      i2c_data.val = 0;
      return 4;   
    }
  //Error, raise error flag
  else
    {
      errno = EBADF;  //io not opened for reading
      return -1;
    } 
}


int 
i2c_mod_master_ioctl (int request, unsigned char* argp)
{
  switch(request)
    {
      case MOD_DSPIC_SET_CTL:
        mod_dspic_ctrl_byte = *argp;
        break;
      default:
        return -1;      //request code not recognised   
    }
  return 0;
}

/** @} */
/** @} */

