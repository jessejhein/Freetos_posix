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

#include <define.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <i2c.h>

//Register
#define TEMP_DATA_REGISTER              0x00
#define TEMP_CONFIG_REGISTER            0x01
#define TEMP_HYSTERESIS_REGISTER        0x02
#define TEMP_LIMIT_REGISTER             0x03


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
} temp_data;
/** Store IO setting */
static int temp_io_flag;


int
i2c_temp_open (int flags)
{
  temp_io_flag = flags;
  i2c_open ();
  return 0;
}


int
i2c_temp_read (unsigned int *buf, int count)
{
  //Perform read if read operation is enabled
  if (temp_io_flag & O_RDWR || temp_io_flag & O_WRONLY)
    {
      errno = EROFS;
      return -1;
    }
  else
    {
      unsigned int error = 0;
      unsigned char status = 0;
      unsigned char data = 0;
#if (I2C_NUM > 1)
      if (pthread_mutex_lock (&i2c_mutex) == 0)
        {
#endif /* I2C_NUM > 1 */
          //configure resolution
          static char init = 0;
          if (init == 0)
            {
              //Send start bit, slave address (Write Mode)
              status = I2C_START;
              i2c_ioctl (I2C_SET_STATUS, &status);
              data = (unsigned char) I2C_TEMP_ADDR;
              if (i2c_write (&data) == 0) error = 1;

              //Send register byte
              data = (unsigned char) TEMP_CONFIG_REGISTER;
              if (i2c_write (&data) == 0) error = 1;

              //Send value byte
              status = I2C_STOP;
              i2c_ioctl (I2C_SET_STATUS, &status);
              data = (unsigned char) TEMP_RESOLUTION;
              if (i2c_write (&data) == 0) error = 1;

              //complete initialisation
              init = 1;

              //Send start bit, slave address (Write Mode)
              status = I2C_START;
              i2c_ioctl (I2C_SET_STATUS, &status);
              data = (unsigned char) I2C_TEMP_ADDR;
              if (i2c_write (&data) == 0) error = 1;

              //Send register byte
              data = (unsigned char) TEMP_DATA_REGISTER;
              if (i2c_write (&data) == 0) error = 1;
            }

          //Send start bit, slave address (Read Mode)
          status = I2C_START;
          i2c_ioctl (I2C_SET_STATUS, &status);
          data = (unsigned char) (I2C_TEMP_ADDR | 0x01);
          if (i2c_write (&data) == 0) error = 1;

          //Receive High Byte with Acknowledgement
          if (i2c_read (&data) == 0) error = 1;
          temp_data.byte.high = (unsigned char) data;

          //Receive Low Byte with Not Acknowledgement and stop bit
          status = I2C_NACK | I2C_STOP;
          i2c_ioctl (I2C_SET_STATUS, &status);
          if (i2c_read (&data) == 0) error = 1;
          temp_data.byte.low = (unsigned char) data;

          //use I2C_START for successive read
          if (init == 1) init++;

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

      //export raw data
      *buf = temp_data.value;
      return 2;
    }
}


int
i2c_temp_ioctl (int request, unsigned char* argp)
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
