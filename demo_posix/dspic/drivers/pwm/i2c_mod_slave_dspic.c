/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup i2c_mod_slave I2C MODULATION DSPIC (SLAVE) 
 * @{
 */

/**
 * \file
 *  I2C MODULATION DSPIC (SLAVE) Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

#include <define.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <asm/types.h>
#include <i2c.h>

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

/** Store io setting */
static int mod_dspic_flag;

/** modulation frequency */
static float mod_frequency[4];


int 
i2c_mod_slave_open (int flags)
{
  mod_dspic_flag = flags;

  // Check for unique match address
  I2CADD = (I2C_MOD_DSPIC_ADDR >> 1);
  I2CMSK = 0x00;

  i2c_open ();
  return 0;
}


/**
 * \brief i2c slave interrupt routine
 * 
 * \remarks
 * \li write to slave example
 * \verbatim
    Mst/Slv    _______ M ___M___ M S ____M___ S ____M___ S ___M____ S ___M____ S ___M ___ S M ____
    SDA (Data)        |S|       | |A|        |A|        |A|        |A|        |A|        |A|S|
                      |T|address|W|C|channelA|C| Data 0 |C| Data 1 |C| Data 2 |C| Data 3 |C|T|
                      |A|1011000|0|K|00010000|K|10101010|K|10101010|K|10101010|K|10101010|K|P|
   \endverbatim
 * \li read from slave example
 * \verbatim
    Mst/Slv    _______ M ___M___ M S ____M___ S M ___M___ M S ___S____ M ___S____ M ___S____ M ___S____ M M _____ 
    SDA (Data)        |S|       | |A|        |A|R|       | |A|        |A|        |A|        |A|        |N|S|
                      |T|address|W|C|channelA|C|E|address|R|C| Data 0 |C| Data 1 |C| Data 2 |C| Data 3 |A|T|
                      |A|1011000|0|K|00010010|K|S|1011000|1|K|10101010|K|10101010|K|10101010|K|10101010|K|P|
   \endverbatim
 */ 
void
i2c_slave_interrupt(void)
{
#define SLAVE_WRITE           0
#define SLAVE_READ            1
#define SLAVE_RESET           2

  /* keep track of slave write or slave read operation */
  static unsigned char i2c_state = SLAVE_WRITE;
  /* keep track of how many data has been rx (excluding address byte) */
  static unsigned char i2c_rx_cnt;
  /* keep track of how many data has been tx */
  static unsigned char i2c_tx_cnt;
  /* keep track assessing which channel */
  static unsigned char mod_channel;

  switch(i2c_state)
    {
      //master write to slave (write address, channel, data + read first byte)
      case SLAVE_WRITE:
        {
          //rx byte is address
          if (I2CSTATbits.D_A == 0)
            {
              //clear rx buffer to avoid overrun
              unsigned char dummy;
              dummy = I2CRCV;
        
              //master read from slave
              if (I2CSTATbits.R_W == 1)
                {
                  if (mod_channel == MOD_DSPIC_CHA) i2c_data.val = mod_frequency[0];
                  else if (mod_channel == MOD_DSPIC_CHB) i2c_data.val = mod_frequency[1];
                  else if (mod_channel == MOD_DSPIC_CHC) i2c_data.val = mod_frequency[2];
                  else if (mod_channel == MOD_DSPIC_CHD) i2c_data.val = mod_frequency[3];
                  else i2c_data.val = 0;
                  
                  //tx data 0
                  I2CTRN = (unsigned char) i2c_data.byte[0];
                  I2CSTATbits.TBF = 1;
                  i2c_tx_cnt = 1;
                  i2c_state = SLAVE_READ;
                }
              //else nothing to do (master write to slave at next interrupt)
            }
          //rx byte is data
          else 
            {
              //1st byte denotes which channel
              if (i2c_rx_cnt == 0) mod_channel = (unsigned char) I2CRCV;
              //2nd byte onwards denotes data 0, 1, 2, 3
              else i2c_data.byte[i2c_rx_cnt -1] = (unsigned char) I2CRCV;
              i2c_rx_cnt++;
              //i2c_data is ready
              if (i2c_rx_cnt >= 5)
                {
                  if (mod_channel == MOD_DSPIC_CHA) mod_frequency[0] = i2c_data.val;
                  else if (mod_channel == MOD_DSPIC_CHB) mod_frequency[1] = i2c_data.val;
                  else if (mod_channel == MOD_DSPIC_CHC) mod_frequency[2] = i2c_data.val;
                  else if (mod_channel == MOD_DSPIC_CHD) mod_frequency[3] = i2c_data.val;
                  //reset variables
                  i2c_data.val = 0.0f;
                  i2c_rx_cnt = 0;
                  i2c_tx_cnt = 0;
                  mod_channel = 0;
                  i2c_state = SLAVE_WRITE;
                }
            }
          break;
        }
      //master read from slave (data only)
      case SLAVE_READ:
        {
          //tx byte 1, 2, 3
          I2CTRN = (unsigned char) i2c_data.byte[i2c_tx_cnt];
          I2CSTATbits.TBF = 1;
          i2c_tx_cnt++;
          if(i2c_tx_cnt >= 4) i2c_state = SLAVE_RESET;
          break;
        }
      default:
        {
          //reset variables
          i2c_data.val = 0.0f;
          i2c_rx_cnt = 0;
          i2c_tx_cnt = 0;
          mod_channel = 0;
          i2c_state = SLAVE_WRITE;
        }
    }
  I2CCONbits.SCLREL = 1;       //set to release master clock
  _SI2CIF = 0;
}


int 
i2c_mod_slave_write (float *buf)
{
  //Perform Write if write operation is enabled
  if (mod_dspic_flag & O_RDWR || mod_dspic_flag & O_WRONLY)
    {
      switch (mod_dspic_ctrl_byte)
        {
          case MOD_DSPIC_CHA:
            {
              mod_frequency[0] = *buf;
              break;
            }
          case MOD_DSPIC_CHB:
            {
              mod_frequency[1] = *buf;
              break;
            }
          case MOD_DSPIC_CHC:
            {
              mod_frequency[2] = *buf;
              break;
            }
          case MOD_DSPIC_CHD:
            {
              mod_frequency[3] = *buf;
              break;
            }
          default:
            {
              return -1;
            }
        }
      return 4;
    }
  //Error, raise error flag
  else
    {
      errno = EBADF;  //io not opened for writing
      return -1;
    } 
}


int 
i2c_mod_slave_read (float *buf)
{
  //Perform Write if write operation is enabled
  if (mod_dspic_flag & O_RDWR || mod_dspic_flag & O_RDONLY)
    {
      switch (mod_dspic_ctrl_byte)
        {
          case MOD_DSPIC_CHA:
            {
              *buf = mod_frequency[0];
              break;
            }
          case MOD_DSPIC_CHB:
            {
              *buf = mod_frequency[1];
              break;
            }
          case MOD_DSPIC_CHC:
            {
              *buf = mod_frequency[2];
              break;
            }
          case MOD_DSPIC_CHD:
            {
              *buf = mod_frequency[3];
              break;
            }
          default:
            {
              return -1;
            }
        }
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
i2c_mod_slave_ioctl (int request, unsigned char* argp)
{
  switch (request)
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

