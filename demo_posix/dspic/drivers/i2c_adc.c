/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup i2c_adc I2C ADC
 * @{
 * 
 * Control I2C ADC ADS7823 (12-bit I2C ADC)
 * \li The I2C module uses SCL and SDA, located at pin 36 and 37 respectively.
 * \li All I2C devices shares a common communication speed (default: 400kHz) 
 * \li The driver has a POSIX-like interface with open(), read(), write(), ioctl()
 * \li When i2c has multiple devices, read(), write() cannot be used in ISR (Interrupt routine)
 * \li At most 4 ADC data (8 bytes) can be read per operation.
 */

/**
 * \file
 * I2C ADC Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

#ifdef I2C_ADC_MOD

#include <define.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

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

#if (I2C_NUM > 1)
#include <pthread.h>
/** for mutual exclusion of other device using I2C bus */
extern pthread_mutex_t i2c_mutex;
#endif /* I2C_NUM>1 */


/**
 * \brief Initialise I2C ADC
 * \param flags accessing mode
 * \retval 0 ADC opened
 */
int
i2c_adc_open (int flags)
{
  adc_io_flag = flags;
  i2c_open ();
  return 0;
}


/**
 * \brief read 2 bytes from ADC
 * \param buf pointer of data to read from ADC
 * \param count number of bytes to read
 * \return number of bytes read
 * \retval 0 no data has been read
 * \retval -1 not opened for reading error (errno = EBADF)
 *
 * \remarks
 * \li example
 * \verbatim
                                        |<--ADC Conversion-->|
    Mst/Slv    _______ M ___M___ M S ____M___ S M ___M___ M S ___S____ M ___S____ M ___S____ M ___S____ M ___S____ M ___S____ M ___S____ M ___S____ M M _____
    SDA (Data)        |S|       | |A|        |A|R|       | |A|        |A|        |N|        |A|        |N|        |A|        |N|        |A|        |N|S|
                      |T|address|W|C|command |C|E|address|R|C|Data0 H |C|Data0 L |A|Data1 H |C|Data1 L |A|Data2 H |C|Data2 L |A|Data3 H |C|Data3 L |A|T|
                      |A|1001000|0|K|000xxxxx|K|S|1001000|1|K|0000xxxx|K|xxxxxxxx|K|0000xxxx|K|xxxxxxxx|K|0000xxxx|K|xxxxxxxx|K|0000xxxx|K|xxxxxxxx|K|P|
   \endverbatim
 */
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


/**
 * \brief change setting for DAC
 * \param request Request code defined in ioctl.h
 * \param argp pointer for control configuration, request code dependent.
 * \retval 0 success
 * \retval -1 error
 */
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
#endif /* I2C_ADC_MOD */

/** @} */
/** @} */
