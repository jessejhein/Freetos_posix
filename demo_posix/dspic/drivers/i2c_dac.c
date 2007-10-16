/************************************************************************************************
 * File:            i2c_dac.c
 * Description:     control i2c dac
 ***********************************************************************************************
 * SUMMARY:
 * 1)   Control access to a I2C 10-bit Quad-channel DAC (DAC6574)
 * 2)   The driver has a POSIX-compliant interface with read(), write(), ioctl().
 * 3)   Default communication speed is 400kHz (Fast Speed).
 * 4)   Reading and Writing from the module is a 10-bit integer (i.e 0-1023)
 * 5)   When i2c has multiple devices, read(), write() cannot be used in ISR (Interrupt routine)
 ***********************************************************************************************/

#include <sys/ioctl.h>
#include <define.h>
#include <fcntl.h>
#include <errno.h>

#if (I2C_DAC_MOD == 0)
//do not include DAC module if disabled

#else 

/************************************************************************************************
 * Local Variables
 ************************************************************************************************/
static unsigned char dac_ctrl_byte = 0;    //Store control byte for transmit/receive
static struct {
    unsigned char high;
    unsigned char low;
} dac_data;                           //Store the high and low byte for i2c communication
static int dac_io_flag;

/************************************************************************************************
 * External Variables
 ************************************************************************************************/
#if ( defined(MPLAB_DSPIC33_PORT) & (I2C_DAC_MOD>0) & (NVM_MOD>0) & (NVM_SRC==NVM_SRC_I2C) ) 
 #include <pthread.h>
 extern pthread_mutex_t i2c_mutex;
#endif

/************************************************************************************************
 * Local Functions Declaration
 ************************************************************************************************/
static void int2dac(unsigned int value);    //Split $value to high byte and low byte
static int dac2int(void);                   //Combine high byte to low byte to integer

/************************************************************************************************
 * Name:                int i2c_dac_open(int flags)
 * 
 * Input:               flags: accessing mode
 * 
 * Output:              0: dac opened
 * 
 * Function:            Initialize I2C DAC
 ************************************************************************************************/
int i2c_dac_open(int flags)
{
    dac_io_flag = flags;
    i2c_open();
    return 0;
}

/************************************************************************************************
 * Name:                int i2c_dac_write(unsigned int *buf)
 * 
 * Input:               *buf: pointer of data to write to dac
 * 
 * Output:              number of bytes written
 *                      0: no data has been written
 *                      2: two bytes has been written
 *                      -1: not opened for writing error (errno = EBADF)
 * 
 * Function:            write 2 bytes to dac
 ************************************************************************************************
 * Mst/Slv    _______ M ___M___ M S ____M___ S ____M___ S ___M____ S M ________ 
 * SDA (Data)        |S|       | |A|        |A|        |A|        |A|S|
 *                   |T|address|W|C|channelA|C| Data H |C| Data L |C|T|
 *                   |A|1001111|0|K|00010000|K|10101010|K|10XXXXXX|K|P|
 ************************************************************************************************/
int i2c_dac_write(unsigned int *buf)
{
    //Perform Write if write operation is enabled
    if(dac_io_flag & O_RDWR || dac_io_flag & O_WRONLY)
    {
        unsigned int status, data;
        unsigned int error = 0;
    
        /*
         * Convert data to dac format
         */ 
        int2dac(buf[0]);
        
       #if ( defined(MPLAB_DSPIC33_PORT) & (I2C_DAC_MOD>0) & (NVM_MOD>0) & (NVM_SRC==NVM_SRC_I2C) ) 
        if(pthread_mutex_lock(&i2c_mutex) == 0)
        {
       #endif    
    
            /*
             * Send start bit, slave address
             */ 
            status = I2C_START;
            i2c_ioctl(I2C_SET_STATUS, &status);
            data = (unsigned char) I2C_DAC_ADDR;
            if(i2c_write(&data) == 0) error = 1;
            
            /*
             * Send control byte: Channel select
             */
            data = (unsigned char) dac_ctrl_byte;
            if(i2c_write(&data) == 0) error = 1;
            
            /*
             * Send High Byte
             */
            data = (unsigned char) dac_data.high;       //Write high byte of buf[i]
            if(i2c_write(&data) == 0) error = 1;
            
            /*
             * Send Low Byte with Stop bit
             */
            status = I2C_STOP;
            i2c_ioctl(I2C_SET_STATUS, &status);
            data = (unsigned char) dac_data.low;        //Write low byte of buf[i]
            if(i2c_write(&data) == 0) error = 1;
    
       #if ( defined(MPLAB_DSPIC33_PORT) & (I2C_DAC_MOD>0) & (NVM_MOD>0) & (NVM_SRC==NVM_SRC_I2C) ) 
            pthread_mutex_unlock(&i2c_mutex);
        }
        else
        {
            error = 1;  //i2c is busy
        }
       #endif    
        
        return (error == 1)? 0 : 2;
    }
    //Error, raise error flag
    else
    {
        errno = EBADF;  //io not opened for writing
        return -1;
    } 
}

/************************************************************************************************
 * Name:                int i2c_dac_read(unsigned int *buf)i2c_dac_read()
 * 
 * Input:               *buf: pointer of data to read from dac
 * 
 * Output:              number of bytes read
 *                      0: no data has been read
 *                      2: two bytes has been read
 *                      -1: not opened for reading error (errno = EBADF)
 * 
 * Function:            read 2 bytes from dac
 ************************************************************************************************
 * Mst/Slv    _______ M ___M___ M S ____M___ S M ___M___ M S ___S____ M ___S____ M M _____ 
 * SDA (Data)        |S|       | |A|        |A|R|       | |A|        |A|        |N|S|
 *                   |T|address|W|C|channelA|C|E|address|R|C| Data H |C| Data L |A|T|
 *                   |A|1001111|0|K|00010010|K|S|1001111|1|K|10101010|K|10XXXXXX|K|P|
 ************************************************************************************************/
int i2c_dac_read(unsigned int *buf)
{
    //Perform Write if write operation is enabled
    if(dac_io_flag & O_RDWR || dac_io_flag & O_RDONLY)
    {
        unsigned int status, data;
        unsigned int error = 0;
    
       #if ( defined(MPLAB_DSPIC33_PORT) & (I2C_DAC_MOD>0) & (NVM_MOD>0) & (NVM_SRC==NVM_SRC_I2C) ) 
        if(pthread_mutex_lock(&i2c_mutex) == 0)
        {
       #endif    
    
            /*
             * Send start bit, slave address (Write Mode)
             */ 
            status = I2C_START;
            i2c_ioctl(I2C_SET_STATUS, &status);
            data = (unsigned char) I2C_DAC_ADDR;
            if(i2c_write(&data) == 0) error = 1;
        
            /*
             * Send control byte: Channel select
             */
            data = (unsigned char) dac_ctrl_byte;
            if(i2c_write(&data) == 0) error = 1;
        
            /*
             * Send restart bit, slave address (Read Mode)
             */
            status = I2C_RESTART;
            i2c_ioctl(I2C_SET_STATUS, &status);
            data = (unsigned char) (I2C_DAC_ADDR|0x01);
            if(i2c_write(&data) == 0) error = 1;
        
            /*
             * Receive High Byte with Acknowledgment
             */
            if(i2c_read(&data) == 0) error = 1;
            dac_data.high = (unsigned char) data;
        
            /*
             * Receive Low Byte with Not Acknowledgment and stop bit
             */ 
            status = I2C_NACK | I2C_STOP;
            i2c_ioctl(I2C_SET_STATUS, &status);
            if(i2c_read(&data) == 0) error = 1;
            dac_data.low = (unsigned char) data;
    
       #if ( defined(MPLAB_DSPIC33_PORT) & (I2C_DAC_MOD>0) & (NVM_MOD>0) & (NVM_SRC==NVM_SRC_I2C) ) 
            pthread_mutex_unlock(&i2c_mutex); 
        }
        else
        {
            error = 1;  //i2c is busy
        }      
       #endif
    
        if(error == 1)
            return 0;
    
        /*
         * Convert data to integer format
         */ 
        buf[0] = dac2int();
    
        return 2;   
    }
    //Error, raise error flag
    else
    {
        errno = EBADF;  //io not opened for reading
        return -1;
    } 
}

/************************************************************************************************
 * Name:                i2c_dac_ioctl()
 * 
 * Input:               request:    Request code - defined in ioctl.h
 *                      *argp:      pointer for control config, request code dependent.
 * 
 * Output:              0: success
 *                      -1: error
 * 
 * Function:            change setting for dac
 ************************************************************************************************/
int i2c_dac_ioctl(int request, unsigned char* argp)
{
    switch(request){
        case DAC_SET_CTL:
            dac_ctrl_byte = *argp;
            break;
        default:
            return -1;      //request code not recognised   
    }
    return 0;
}

/********************************************************************
 * Convert 16-bit integer to/from 2-byte format for 10-bit DAC
 ********************************************************************/
/*
 * int2dac()
 */
static void int2dac(unsigned int value)
{
    dac_data.high = (unsigned char) ((value & 0x03FC) >> 2);    //Bit <9:2>
    dac_data.low = (unsigned char) ((value & 0x0003) << 6); //Bit <1:0>
}

/*
 * dac2int()
 */
static int dac2int(void)
{
    int value = (dac_data.high << 8) +  (dac_data.low & 0xFF);
    return ((value >> 6) & 0x3FF);
}

#endif //I2C_DAC_MOD

