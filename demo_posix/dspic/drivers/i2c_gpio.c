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
#define I2C_GPIO_DEFAULT_IOCON          0x0B
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
static unsigned char gpio_port;

static int od_flag;
/** keeps the user set OD */
static unsigned char od[2];
/** keeps the user set TRIS */
static unsigned char tris[2];
/** keeps the user set LAT */
static unsigned char lat[2];


/**
 * \brief perform a write to device
 * \remarks gpio_address and gpio_port must be set appropriately beforehand
 */
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
      data = (unsigned char) gpio_address | gpio_port;
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


/**
 * \brief perform a read from device
 * \remarks gpio_address and gpio_port must be set appropriately beforehand
 */
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
      data = (unsigned char) gpio_address | gpio_port;
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

  //initialise all pins to input
  tris[0] = 0xff; od[0] = 0x00;
  tris[1] = 0xff; od[1] = 0x00;
  gpio_address = I2C_GPIO_PORTA | I2C_GPIO_IODIR;
  while (i2c_gpio_lv_write (&tris[0]) != 1);
  gpio_address = I2C_GPIO_PORTB | I2C_GPIO_IODIR;
  while (i2c_gpio_lv_write (&tris[1]) != 1);

  //reset LAT
  lat[0] = 0x00;
  lat[1] = 0x00;
  gpio_address = I2C_GPIO_PORTA | I2C_GPIO_OLAT;
  while (i2c_gpio_lv_write (&lat[0]) != 1);
  gpio_address = I2C_GPIO_PORTB | I2C_GPIO_OLAT;
  while (i2c_gpio_lv_write (&lat[1]) != 1);

  return 0;
}


/**
 * \brief update the pin status according to the open_drain/normal setting
 * \remarks this function will not update software lat[] status
 */
static void
update_lat_pin (void)
{
  unsigned char *tris_ptr;
  unsigned char *od_ptr;
  unsigned char *lat_ptr;
  if (gpio_port == I2C_GPIO_PORTB)
    {
      tris_ptr = &tris[1];
      od_ptr = &od[1];
      lat_ptr = &lat[1];
    }
  else
    {
      tris_ptr = &tris[0];
      od_ptr = &od[0];
      lat_ptr = &lat[0];
    }

  int i;
  unsigned char tris_tmp = *tris_ptr;
  unsigned char lat_tmp = *lat_ptr;
  for (i = 0; i < 8; i++)
    {
      //pin is input
      if ((*tris_ptr >> i) & 0x01)
        {
          //Set pin to zero
          lat_tmp &= ~(0x01 << i);
        }
      //pin is OD
      else if ((*od_ptr >> i) & 0x01)
        {
          //Set pin to zero
          lat_tmp &= ~(0x01 << i);

          //logic HIGH
          if ((*lat_ptr >> i) & 0x01)
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
          if ((*lat_ptr >> i) & 0x01) lat_tmp |= (0x01 << i);
          else lat_tmp &= ~(0x01 << i);
        }
    }

  gpio_address = I2C_GPIO_OLAT;
  while (i2c_gpio_lv_write (&lat_tmp) != 1);

  gpio_address = I2C_GPIO_IODIR;
  while (i2c_gpio_lv_write (&tris_tmp) != 1);
}



int
i2c_gpio_write (unsigned char *buf)
{
  //Perform Write if write operation is enabled
  if (gpio_io_flag & O_RDWR || gpio_io_flag & O_WRONLY)
    {
      //set OD
      if (od_flag)
        {
          od_flag = 0;
          if (gpio_port == I2C_GPIO_PORTB) od[1] = *buf;
          else od[0] = *buf;
        }
      //set TRIS
      else if (gpio_address == I2C_GPIO_IODIR)
        {
          while (i2c_gpio_lv_write (buf) != 1);
          if (gpio_port == I2C_GPIO_PORTB) tris[1] = *buf;
          else tris[0] = *buf;
        }
      //set LAT/PORT
      else
        {
          //update software LAT status
          if (gpio_port == I2C_GPIO_PORTB)
            {
              lat[1] = *buf;
            }
          else
            {
              lat[0] = *buf;
            }
          update_lat_pin ();
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
      //read OD
      if (od_flag)
        {
          if (gpio_port == I2C_GPIO_PORTB) *buf = od[1];
          else *buf = od[0];
          od_flag = 0;
        }
      //read TRIS
      else if (gpio_address == I2C_GPIO_IODIR)
        {
          if (gpio_port == I2C_GPIO_PORTB) *buf = tris[1];
          else *buf = tris[0];
        }
      //read from LAT
      else if (gpio_address == I2C_GPIO_OLAT)
        {
          if (gpio_port == I2C_GPIO_PORTB) *buf = lat[1];
          else *buf = lat[0];
        }
      //read from PORT
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
          gpio_port = I2C_GPIO_PORTA;
          gpio_address =  I2C_GPIO_IODIR;
          break;
        }
      case GPIO_SET_TRISB:
        {
          gpio_port = I2C_GPIO_PORTB;
          gpio_address =  I2C_GPIO_IODIR;
          break;
        }
      case GPIO_SET_ODA:
        {
          od_flag = 1;
          gpio_port = I2C_GPIO_PORTA;
          gpio_address =  I2C_GPIO_IODIR;
          break;
        }
      case GPIO_SET_ODB:
        {
          od_flag = 1;
          gpio_port = I2C_GPIO_PORTB;
          gpio_address =  I2C_GPIO_IODIR;
          break;
        }
      case GPIO_SET_LATA:
        {
          gpio_port = I2C_GPIO_PORTA;
          gpio_address =  I2C_GPIO_OLAT;
          break;
        }
      case GPIO_SET_LATB:
        {
          gpio_port = I2C_GPIO_PORTB;
          gpio_address =  I2C_GPIO_OLAT;
          break;
        }
      case GPIO_SET_PORTA:
        {
          gpio_port = I2C_GPIO_PORTA;
          gpio_address =  I2C_GPIO_OLAT;
          break;
        }
      case GPIO_SET_PORTB:
        {
          gpio_port = I2C_GPIO_PORTB;
          gpio_address =  I2C_GPIO_OLAT;
          break;
        }
      case GPIO_GET_TRISA:
        {
          gpio_port = I2C_GPIO_PORTA;
          gpio_address =  I2C_GPIO_IODIR;
          break;
        }
      case GPIO_GET_TRISB:
        {
          gpio_port = I2C_GPIO_PORTB;
          gpio_address =  I2C_GPIO_IODIR;
          break;
        }
      case GPIO_GET_ODA:
        {
          od_flag = 1;
          gpio_port = I2C_GPIO_PORTA;
          gpio_address =  I2C_GPIO_IODIR;
          break;
        }
      case GPIO_GET_ODB:
        {
          od_flag = 1;
          gpio_port = I2C_GPIO_PORTB;
          gpio_address =  I2C_GPIO_IODIR;
          break;
        }
      case GPIO_GET_LATA:
        {
          gpio_port = I2C_GPIO_PORTA;
          gpio_address =  I2C_GPIO_OLAT;
          break;
        }
      case GPIO_GET_LATB:
        {
          gpio_port = I2C_GPIO_PORTB;
          gpio_address =  I2C_GPIO_OLAT;
          break;
        }
      case GPIO_GET_PORTA:
        {
          gpio_port = I2C_GPIO_PORTA;
          gpio_address =  I2C_GPIO_GPIO;
          break;
        }
      case GPIO_GET_PORTB:
        {
          gpio_port = I2C_GPIO_PORTB;
          gpio_address =  I2C_GPIO_GPIO;
          break;
        }
      default:
        return -1;      //request code not recognised
    }
  return 0;
}

/** @} */
/** @} */

