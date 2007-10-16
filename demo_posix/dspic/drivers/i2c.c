/************************************************************************************************
 * File:            i2c.c
 * Description:     open i2c port
 ***********************************************************************************************
 * SUMMARY:
 * 1)   The I2C module uses SCL and SDA, located at pin 36 and 37 respectively.
 * 2)   All I2C devices shares a common communication speed (default: 400kHz) 
 * 3)   The driver has a POSIX-like interface with open(), read(), write(), ioctl()
 ***********************************************************************************************/

#include <define.h>
#include <sys/ioctl.h>
#include <asm/system.h>

#if (I2C_MOD == 0)
//do not include I2C module if disabled

#else 

#define ACK_TIMEOUT 0x0F        //counts to wait for an acknowledgment
#define i2cIdle()   while(I2CCONbits.SEN||I2CCONbits.PEN||I2CCONbits.RCEN||I2CCONbits.ACKEN||I2CSTATbits.TRSTAT)

/************************************************************************************************
 * Local Variables
 ************************************************************************************************/
typedef union{
    unsigned char val;
    struct{
        unsigned START:1;       //start
        unsigned RESTART:1; //restart
        unsigned STOP:1;        //stop
        unsigned NACK:1;        //not acknowledgment
        unsigned :1;
        unsigned :1;
        unsigned :1;
        unsigned :1;    
    }bits;
} I2C_STATUS;
static I2C_STATUS i2c_status;   //indicate the start and stop condition with the data in progress

/************************************************************************************************
 * Semaphore for multiple i2c devices
 * +-- program must acquire i2c_busy before read/write operation (from start bit to stop bit)
 ************************************************************************************************/
#if ( defined(MPLAB_DSPIC33_PORT) & (I2C_DAC_MOD>0) & (NVM_MOD>0) & (NVM_SRC==NVM_SRC_I2C) ) 
#include <pthread.h>
pthread_mutex_t i2c_mutex; 
#endif

/************************************************************************************************
 * Name:                i2c_open()
 * 
 * Input:               none
 * 
 * Output:              none
 * 
 * Function:            Initialize I2C with assigned baudrate
 ************************************************************************************************/
void i2c_open(void)
{
    //Open i2c if not already opened
    if(I2CCONbits.I2CEN == 0)
    {
        /*
         * Disable I2C interrupt (not used)
         * +-- Status of I2C is detected by polling
         * 
         */
        _SI2CIF = 0;        //Clear Slave interrupt
        _MI2CIF = 0;        //Clear Master interrupt
        _SI2CIE = 0;        //Disable Slave interrupt
        _MI2CIE = 0;        //Disable Master interrupt
        
        /*
         * Configure Baud rate (400kHz, change in define.h>
         */
        I2CBRG = I2C_BRG;
        
        /*
         * I2C Configuration:
         * +-- Default:     Continue in Idle mode, Release SCL clock
         *                  Disable IPMI, SMBus, General Call, clock stretch
         *                  7-bit address,
         *                  Acknowledge sequence, Receive sequence,
         *                  Stop & Start & Repeated Start conditions not in progress
         * +-- Enable I2C, disable slew rate control,      
         */
        I2CCONbits.I2CEN = 1;   //Enable I2C module 
        i2cIdle();              //I2C bus at idle state, awaiting transimission
        
        i2c_status.val = 0;     //clear status flags

      #if ( defined(MPLAB_DSPIC33_PORT) & (I2C_DAC_MOD>0) & (NVM_MOD>0) & (NVM_SRC==NVM_SRC_I2C) ) 
        pthread_mutex_init(&i2c_mutex, NULL);
      #endif
    }   
}

/************************************************************************************************
 * Name:                i2c_write(unsigned char *buf)
 * 
 * Input:               *buf:   pointer to data for transmission
 * 
 * Output:              on success return 1
 *                      on failure return 0
 * 
 * Precondition:        i2c_status is set appropriately 
 * 
 * Function:            send a byte via I2c
 ************************************************************************************************
 * Mst/Slv    _______ M ____M___ S M ________ 
 * SDA (Data)        |S|  data  |A|S|
 *                   |T|        |C|T|
 *                   |A|XXXXXXXX|K|P|
 ************************************************************************************************/
int i2c_write(unsigned char *buf)
{
    unsigned int count = 0;
    
    /*
     * Send a start or restart bit if needed
     */
    if(i2c_status.bits.START)
    {
        I2CCONbits.SEN = 1;                 
        Nop();                          //A small delay for hardware to respond
        while(I2CCONbits.SEN);          //Wait till Start sequence is completed
    }
    else if(i2c_status.bits.RESTART)
    {
        I2CCONbits.RSEN = 1;                
        Nop();                          //A small delay for hardware to respond
        while(I2CCONbits.RSEN);         //Wait till Start sequence is completed
    }
    
    /*
     * Send the byte
     */
    I2CTRN = *buf;                  //Transmit register
    while(I2CSTATbits.TBF);         //Wait for transmit buffer to empty
    
    /*
     * Check if slave acknowledged
     */
    while(I2CSTATbits.ACKSTAT){
        if(++count > ACK_TIMEOUT){
            //Slave did not acknowledge, byte did not transmit sucessfully, 
            //send stop bit to reset i2c
            I2CCONbits.PEN = 1;
            Nop();                          //A small delay for hardware to respond
            while(I2CCONbits.PEN);          //Wait till stop sequence is completed
            i2cIdle();
            return 0;
        }
    }
    i2cIdle();

    /*
     * Send a stop bit if needed
     */
    if(i2c_status.bits.STOP)
    {
        I2CCONbits.PEN = 1;
        Nop();                          //A small delay for hardware to respond
        while(I2CCONbits.PEN);          //Wait till stop sequence is completed
        i2cIdle();
    }
    i2c_status.val = 0;             //Clear status
    
    return 1;
}

/************************************************************************************************
 * Name:                i2c_read(unsigned char *buf)
 * 
 * Input:               *buf:   pointer to data for reception
 * 
 * Output:              on success return 1
 *                      on failure return 0
 * 
 * Precondition:        i2c_status is set appropriately 
 *                      target device has been set appropriately
 * 
 * Function:            read a byte via I2c
 ************************************************************************************************
 * Mst/Slv     ____ ___S____ M M _____ 
 * SDA (Data)      |  data  |A|S|
 *                 |        |C|T|
 *                 |XXXXXXXX|K|P|
 ************************************************************************************************/
int i2c_read(unsigned char *buf)
{
    /*
     * Get the byte
     */
    I2CCONbits.RCEN = 1;                    //Enable Receive
    while(I2CCONbits.RCEN);
    I2CSTATbits.I2COV = 0;                  //Clear receive overflow
    *buf = (unsigned char) I2CRCV;          //Access the receive buffer
    
    /*
     * Send Acknowledgment
     */
    I2CCONbits.ACKDT = (i2c_status.bits.NACK)? 1 : 0;
    I2CCONbits.ACKEN = 1;       //Send Acknowledgement/Not Acknowledgement
    i2cIdle();                  //I2C bus at idle state, awaiting transimission
    
    /*
     * Send a stop bit if needed
     */
    if(i2c_status.bits.STOP)
    {
        I2CCONbits.PEN = 1;
        Nop();                          //A small delay for hardware to respond
        while(I2CCONbits.PEN);          //Wait till stop sequence is completed
        i2cIdle();
    }

    i2c_status.val = 0;             //Clear status
    return 1;
}


/************************************************************************************************
 * Name:                int i2c_ioctl(int request, unsigned char* argp)
 * 
 * Input:               request:    Request code - defined in ioctl.h
 *                      *argp:      pointer for control config, request code dependent.
 * 
 * Output:              on success return 0
 *                      on failure return -1
 * 
 * Precondition:        none
 * 
 * Function:            set the local parameters of i2c
 ************************************************************************************************/
int i2c_ioctl(int request, unsigned char* argp)
{
    switch(request){
        case I2C_SET_STATUS:
            i2c_status.val = *argp;
            break;
        default:
            return -1;      //request code not recognised   
    }
    return 0;
}

#endif //I2C_MOD
