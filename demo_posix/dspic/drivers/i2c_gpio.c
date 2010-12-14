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
  #define I2C_GPIO_IOCON_BANK           0x80
  #define I2C_GPIO_IOCON_SEQOP          0x20
//Default bank = 1
#define I2C_GPIO_PORTA                  0x00
#define I2C_GPIO_PORTB                  0x10
//-----------------------------------------
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

static int od_flag;


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


static int
i2c_gpio_lv_read (unsigned char* buf)
{
  unsigned int error = 0;
  unsigned char status = 0;
  unsigned char data = 0;

#if (I2C_NUM > 1)
  if (pthread_mutex_lock(&i2c_mutex) == 0)
    {
#endif /* I2C_NUM > 1 */

      //Send start bit, slave address (Write Mode)
      status = I2C_START;
      i2c_ioctl (I2C_SET_STATUS, &status);
      data = (unsigned char) I2C_GPIO_ADDR;
      if (i2c_write (&data) == 0) error = 1;

      //Send control byte: Command byte
      data = (unsigned char) gpio_address;
      if (i2c_write (&data) == 0) error = 1;

      //Send restart bit, slave address (Read Mode)
      status = I2C_RESTART;
      i2c_ioctl (I2C_SET_STATUS, &status);
      data = (unsigned char) (I2C_GPIO_ADDR | 0x01);
      if (i2c_write (&data) == 0) error = 1;

      //Receive data with Not Acknowledgement and stop bit
      status = I2C_NACK | I2C_STOP;
      i2c_ioctl (I2C_SET_STATUS, &status);
      if (i2c_read (&data) == 0) error = 1;
      *buf = (unsigned char) data;

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
  unsigned char data = (I2C_GPIO_IOCON_BANK | I2C_GPIO_IOCON_SEQOP);
  while (i2c_gpio_lv_write (&data) != 1);

  return 0;
}


int
i2c_gpio_write (unsigned char *buf)
{
  //Perform Write if write operation is enabled
  if (gpio_io_flag & O_RDWR || gpio_io_flag & O_WRONLY)
    {
      //changing direction pin
      if ((gpio_address & (~I2C_GPIO_PORTB)) == I2C_GPIO_IODIR)
        {
          //set to input or output
          if (od_flag == 0)
            {
              while (i2c_gpio_lv_write (buf) != 1);
            }
          else
            {
              printf ("write od\n");
            }
        }
      //write to PORT or LAT
      else
        {
          while (i2c_gpio_lv_write (buf) != 1);
        }
    }
  //Error, raise error flag
  else
    {
      errno = EBADF;  //IO not opened for writing
      return -1;
    }
  return 1;
}


int
i2c_gpio_read (unsigned char *buf)
{

  //Perform Write if write operation is enabled
  if (gpio_io_flag & O_RDWR || gpio_io_flag & O_RDONLY)
    {
      //changing direction pin
      if ((gpio_address & (~I2C_GPIO_PORTB)) == I2C_GPIO_IODIR)
        {
          //set to input or output
          if (od_flag == 0)
            {
              while (i2c_gpio_lv_write (buf) != 1);
            }
          else
            {
              printf ("read od\n");
            }
        }
      //read from PORT or LAT
      else
        {
          while (i2c_gpio_lv_read (buf) != 1);
        }
    }
  //Error, raise error flag
  else
  {
    errno = EBADF;  //io not opened for reading
    return -1;
  }
  return 1;
}


int
i2c_gpio_ioctl (int request, unsigned char* argp)
{
  switch (request)
    {
      case GPIO_SET_TRISA:
        {
          gpio_address = I2C_GPIO_PORTA | I2C_GPIO_IODIR;
          break;
        }
      case GPIO_SET_TRISB:
        {
          gpio_address = I2C_GPIO_PORTB | I2C_GPIO_IODIR;
          break;
        }
      case GPIO_SET_ODA:
        {
          od_flag = 1;
          gpio_address = I2C_GPIO_PORTA | I2C_GPIO_IODIR;
          break;
        }
      case GPIO_SET_ODB:
        {
          od_flag = 1;
          gpio_address = I2C_GPIO_PORTB | I2C_GPIO_IODIR;
          break;
        }
      case GPIO_SET_LATA:
        {
          gpio_address = I2C_GPIO_PORTA | I2C_GPIO_OLAT;
          break;
        }
      case GPIO_SET_LATB:
        {
          gpio_address = I2C_GPIO_PORTB | I2C_GPIO_OLAT;
          break;
        }
      case GPIO_SET_PORTA:
        {
          gpio_address = I2C_GPIO_PORTA | I2C_GPIO_GPIO;
          break;
        }
      case GPIO_SET_PORTB:
        {
          gpio_address = I2C_GPIO_PORTB | I2C_GPIO_GPIO;
          break;
        }
      case GPIO_GET_TRISA:
        {
          gpio_address = I2C_GPIO_PORTA | I2C_GPIO_IODIR;
          break;
        }
      case GPIO_GET_TRISB:
        {
          gpio_address = I2C_GPIO_PORTB | I2C_GPIO_IODIR;
          break;
        }
      case GPIO_GET_ODA:
        {
          od_flag = 1;
          gpio_address = I2C_GPIO_PORTA | I2C_GPIO_IODIR;
          break;
        }
      case GPIO_GET_ODB:
        {
          od_flag = 1;
          gpio_address = I2C_GPIO_PORTB | I2C_GPIO_IODIR;
          break;
        }
      case GPIO_GET_LATA:
        {
          gpio_address = I2C_GPIO_PORTA | I2C_GPIO_OLAT;
          break;
        }
      case GPIO_GET_LATB:
        {
          gpio_address = I2C_GPIO_PORTB | I2C_GPIO_OLAT;
          break;
        }
      case GPIO_GET_PORTA:
        {
          gpio_address = I2C_GPIO_PORTA | I2C_GPIO_GPIO;
          break;
        }
      case GPIO_GET_PORTB:
        {
          gpio_address = I2C_GPIO_PORTB | I2C_GPIO_GPIO;
          break;
        }
      default:
        return -1;      //request code not recognised
    }
  return 0;
}

/** @} */
/** @} */

