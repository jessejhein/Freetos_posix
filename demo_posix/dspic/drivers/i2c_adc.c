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

#include <define.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <i2c.h>

#define I2C_ADC_COMMAND                 0x00
#define I2C_ADC_MAX_DATA                4


/** Store the high and low byte for i2c communication */
static struct
{
  union
    {
      unsigned int value;
      struct
        {
          unsigned char low;
          unsigned char high;
        } byte;
    };
} adc_data[I2C_ADC_MAX_DATA];
/** Store io setting */
static int adc_io_flag;


int
i2c_adc_open (int flags)
{
  adc_io_flag = flags;
  i2c_open ();
  return 0;
}


int
i2c_adc_read (unsigned int *buf, int count)
{
  //Perform read if read operation is enabled
  if (adc_io_flag & O_RDWR || adc_io_flag & O_WRONLY)
    {
      errno = EROFS;
      return -1;
    }
  else
    {
      unsigned int error = 0;
      unsigned char status = 0;
      unsigned char data = 0;

      //convert byte count to word count
      count /= 2;

#if (I2C_NUM > 1)
      if (pthread_mutex_lock (&i2c_mutex) == 0)
        {
#endif /* I2C_NUM > 1 */
          //Send start bit, slave address (Write Mode)
          status = I2C_START;
          i2c_ioctl (I2C_SET_STATUS, &status);
          data = (unsigned char) I2C_ADC_ADDR;
          if (i2c_write (&data) == 0) error = 1;

          //Send command byte: start conversion
          data = (unsigned char) I2C_ADC_COMMAND;
          if (i2c_write (&data) == 0) error = 1;

          //Send restart bit, slave address (Read Mode)
          status = I2C_RESTART;
          i2c_ioctl (I2C_SET_STATUS, &status);
          data = (unsigned char) (I2C_ADC_ADDR | 0x01);
          if (i2c_write (&data) == 0) error = 1;

          //Read upto 4 data bytes
          int i;
          for (i = 0; (i < count) && (i < I2C_ADC_MAX_DATA); i++)
            {
              //Receive High Byte with Acknowledgement
              if (i2c_read (&data) == 0) error = 1;
              adc_data[i].byte.high = (unsigned char) data;

              //Receive Low Byte with Not Acknowledgement (with Stop bit)
              status = ((i < (count-1)) && (i < (I2C_ADC_MAX_DATA-1)))? I2C_RESTART : (I2C_NACK | I2C_STOP);
              i2c_ioctl (I2C_SET_STATUS, &status);
              if (i2c_read (&data) == 0) error = 1;
              adc_data[i].byte.low = (unsigned char) data;
            }

#if (I2C_NUM > 1)
          pthread_mutex_unlock(&i2c_mutex);
        }
      else
        {
          error = 1;  //i2c is busy
        }
#endif /* I2C_NUM > 1 */

      if (error == 1)
        {
          return 0;
        }

      int i;
      for (i = 0; (i < count) && (i < I2C_ADC_MAX_DATA); i++)
        {
          buf[i] = adc_data[i].value;
        }
      return 2 * i;
    }
}


int
i2c_adc_ioctl (int request, unsigned char* argp)
{
  switch (request)
    {
      default:
        return -1;      //request code not recognised
    }
  return 0;
}

/** @} */
/** @} */
