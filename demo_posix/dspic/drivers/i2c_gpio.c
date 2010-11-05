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

#include <define.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <i2c.h>

/*
 * Register address
 */
//-----------------------------------------
//Default bank = 0
#define I2C_GPIO_DEFAULT_IOCON          0x0A
//Default bank = 1
#define I2C_GPIO_IODIR                  0x00
#define I2C_GPIO_IPOL                   0x01
#define I2C_GPIO_GPINTEN                0x02
#define I2C_GPIO_DEFVAL                 0x03
#define I2C_GPIO_INTCON                 0x04
#define I2C_GPIO_IOCON                  0x05
#define I2C_GPIO_GPPU                   0x06
#define I2C_GPIO_INTF                   0x07
#define I2C_GPIO_INTCAP                 0x08
#define I2C_GPIO_GPIO                   0x09
#define I2C_GPIO_OLAT                   0x0A
//-----------------------------------------


/** Store IO setting */
static int gpio_io_flag;
/** Store address selected */
static unsigned char gpio_address;
static unsigned char gpio_port;
static unsigned char gpio_tris;
static unsigned char gpio_lat;
static unsigned char gpio_od;


static int
i2c_gpio_lv_write (unsigned char* buf)
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
      data = (unsigned char) I2C_GPIO_ADDR;
      if (i2c_write (&data) == 0) error = 1;

      //Send control byte: Command byte
      data = (unsigned char) gpio_address;
      if (i2c_write (&data) == 0) error = 1;

      //Send data with Stop bit
      status = I2C_STOP;
      i2c_ioctl (I2C_SET_STATUS, &status);
      data = (unsigned char) *buf;
      if (i2c_write (&data) == 0) error = 1;

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


int
i2c_gpio_open (int flags)
{
  gpio_io_flag = flags;
  i2c_open ();

  //Bank = 1, no pointer increment
  gpio_address = I2C_GPIO_DEFAULT_IOCON;
  unsigned char data = 0xA0;
  while (i2c_gpio_lv_write (&data) != 1);

  //port b to output
  gpio_address = 0x10 | I2C_GPIO_IODIR;
  data = 0x00;
  while (i2c_gpio_lv_write (&data) != 1);

  //set output to 1
  gpio_address = 0x10 | I2C_GPIO_OLAT;
  data = 0xAA;
  while (i2c_gpio_lv_write (&data) != 1);

  return 0;
}




int
i2c_gpio_write (unsigned char *buf)
{/*
  //Perform Write if write operation is enabled
  if (gpio_io_flag & O_RDWR || gpio_io_flag & O_WRONLY)
    {
      int input = ((gpio_tris >> gpio_ch) & 0x0001);
      //input
      if (input)
        {
          printf ("input\n");
          return 0;
        }
      //output
      else
        {
          //set LAT
          if (*buf == 0) gpio_lat &= (~(0x0001 << gpio_ch));
          else gpio_lat |= (0x0001 << gpio_ch);

          int open_drain = ((gpio_od >> gpio_ch) & 0x0001);
          //open drain
          if (open_drain)
            {
            printf ("open\n");
              if (*buf == 0)
                {
                  //change to output
                  gpio_address = I2C_GPIO_IODIR;
                  //while (i2c_gpio_lv_write (&??) != 2);

                  //set zero
                  gpio_address = I2C_GPIO_OLAT;
                  while (i2c_gpio_lv_write (&gpio_lat) != 2);
                }
              else
                {
                  //change to input
                  gpio_address = I2C_GPIO_IODIR;
                  //while (i2c_gpio_lv_write (&??) != 2);
                }
            }
          //force output
          else
            {
              printf ("write to LAT 0x%x\n", gpio_lat.value);
              //write to LAT
              gpio_address = I2C_GPIO_OLAT;
              while (i2c_gpio_lv_write (&gpio_lat) != 2);

              printf ("done\n");
            }
          return 1;
        }
    }
  //Error, raise error flag
  else
    {
      errno = EBADF;  //IO not opened for writing
      return -1;
    }*/
  printf ("here\n");
  return 1;
}


int
i2c_gpio_read (unsigned char *buf)
{
  return 0;
}


int
i2c_gpio_ioctl (int request, unsigned char* argp)
{/*
  switch (request)
    {
      case I2C_GPIO_SET_CH:
        {
          gpio_ch = *argp;
          break;
        }
      case I2C_GPIO_SET_MODE:
        {
          if (*argp == I2C_GPIO_OPEN_DRAIN)
            {
              gpio_open_drain.value |= (0x0001 << gpio_ch);
            }
          else if (*argp == I2C_GPIO_OUTPUT)
            {
              gpio_input.value &= (~(0x0001 << gpio_ch));

              printf ("output = 0x%x\n", gpio_input.value);

              gpio_address = I2C_GPIO_IODIR;
              while (i2c_gpio_lv_write (&gpio_input) != 2);

              printf ("done\n");

            }
          else
            {
              gpio_input.value |= (0x0001 << gpio_ch);
              //gpio_address = I2C_GPIO_IODIR;
              //while (i2c_gpio_lv_write (&gpio_input) != 2);
            }
          break;
        }
      default:
        return -1;      //request code not recognised
    }*/
  return 0;
}

/** @} */
/** @} */

