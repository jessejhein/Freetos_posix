/************************************************************************************************
 * File:            adc.c
 * Description:     control ADC input ports
 ***********************************************************************************************
 * SUMMARY:
 * 1)   The 12-bit ADC module consists of 12 channels located at pin 16 (AN0), 15 (AN1), 14 (AN2), 
 *      13 (AN3), 12 (AN4), 11 (AN5), 17 (AN6), 18 (AN7), 21 (AN8), 22 (AN9), 23 (AN10), 24 (AN11),
 *      27 (AN12), 28 (AN13), 29 (AN14), 30 (AN15). These pins are shared with port B, PGC and PGD.
 * 2)   The driver has a POSIX-compliant interface with open(), read(), and ioctl().
 * 3)   Operation of ADC in dsPic30F and dsPic33F is different. For scanning of multiple channels,
 *      dsPic30F uses ADC interrupt, while dsPic33F uses DMA interrupt.
 * 4)   Users should take note that:
 *      a) On open(), AN15 is opened (fos dspic33), AN11 is opened (fos dspic30)
 * 		b) The highest sampling frequency for dsPic30F and dsPic33F are 200kbps and 500kbps
 *         respectively 
 *      c) The result obtained using read() is in unsigned integer format.
 *      d) Users should first add the channel using ioctl() before reading from the channel.
 *      e) There could be a delay between execution of ioctl() and read() to obtain the data,
 *         the delay is indicated by returning -1 from read(). User may poll the return value
 *         to ensure a valid reading. 
 *      f) The interrupt routine for dsPic30F and dsPic33F requires about 7us and 500ns for execution.
 *         User should make sure that the adc sampling time is at least greater than this value, 
 *         and it has adequate processing time for other processes.
 ***********************************************************************************************/

#ifdef ADC_MOD

#include <define.h>
#include <sys/ioctl.h>
#include <asm/system.h>
#include <errno.h>
#include <fcntl.h>
/************************************************************************************************
 *                                     POSIX
 *                              (open, read, ioctl)
 *                                       |
 *                                       |
 *                            +----------+----------+
 *                            |                     |
 *                            |                     |
 *                        uP specific        uP non-specific
 *                     (open, interrupt)     (read, ioctl)
 ************************************************************************************************/



/************************************************************************************************
 * Local Variables
 ************************************************************************************************/
static int adc_io_flag;
static unsigned char adc_ch_status[ADC_MAX_CH];       //1=on, 0=off
static unsigned int adc_ch_select = 0;                //Pointer to channel to be read from
static unsigned int adc_data_ready = 0;               //Indicate if RAM data is ready for output
static volatile unsigned int* adc_buf_ptr;           //Pointer to ADC register buffer, 
                                                        //volatile because data is changed by hardware
static unsigned int adc_queue[ADC_QSIZE][ADC_MAX_CH]; //Store most updated data, 
                                                        //index of array referenced by channel ID 
static unsigned char adc_queue_ptr = ADC_QSIZE-1;     //pointer to most updated samples



#ifdef MPLAB_DSPIC30_PORT
/************************************************************************************************
 * OPERATION OF ADC IN DSPIC30
 * ===========================
 * 
 *    CH          0   2   3        0   2   3        0   2   3        0   2   3
 *                |   |   |        |   |   |        |   |   |        |   |   |
 *                |   |   |   ...  |   |   |   ...  |   |   |   ...  |   |   | 
 *               \|/ \|/ \|/      \|/ \|/ \|/      \|/ \|/ \|/      \|/ \|/ \|/ 
 *          --------------------------------------------------------------------------> Time
 * 
 *                 ADCBUF           ADCBUF           ADCBUF           ADCBUF
 *              +----------+     +----------+     +----------+     +----------+
 *              | ch0(n-3) |     | ch0(n-2) |     | ch0(n-1) |     |  ch0(n)  |
 *              | ch2(n-3) |     | ch2(n-2) |     | ch2(n-1) |     |  ch2(n)  |
 *              | ch3(n-3) |     | ch3(n-2) |     | ch3(n-1) |     |  ch3(n)  |
 *              |   ...    |     |   ...    |     |   ...    |     |   ...    |
 *              +----------+     +----------+     +----------+     +----------+
 *                    |                |                |                |
 * INTERRUPT          |                |                |                |
 *                   \|/              \|/              \|/              \|/
 *              adc_queue[0]     adc_queue[1]     adc_queue[2]     adc_queue[0]
 *              +----------+     +----------+     +----------+     +----------+
 *              | ch0(n-3) |     | ch0(n-2) |     | ch0(n-1) |     |  ch0(n)  |
 *              |   ---    |     |   ---    |     |   ---    |     |   ---    |
 *              | ch2(n-3) |     | ch2(n-2) |     | ch2(n-1) |     |  ch2(n)  |
 *              | ch3(n-3) |     | ch3(n-2) |     | ch3(n-1) |     |  ch3(n)  |
 *              |   ...    |     |   ...    |     |   ...    |     |   ...    |
 *              +----------+     +----------+     +----------+     +----------+
 * 
 ************************************************************************************************/


/************************************************************************************************
 * Name:                int adc_open(int flags)
 * 
 * Input:               flags: accessing mode
 * 
 * Output:              -1 if write mode is selected (errno = EROFS)
 *                      0 if ok
 * 
 * Function:            Initialize ADC for dsPic30
 ************************************************************************************************/
int adc_open(int flags)
{
    if(flags & O_RDWR || flags & O_WRONLY){
        errno = EROFS;
        return -1;
    }
    else
    {
        adc_io_flag = flags;
        //===========================================================================
        // Configure interrupt
        _ADIF = 0;                          //clear ADC interrupt flag
        _ADIE = 1;                          //enable adc interrupt
        //===========================================================================
        // Configure analog i/o  
        _TRISB11 = 1;    
        ADPCFG = 0xF7FF;                    //Enable AN11
        //===========================================================================
        // Configure scan input channels    
        ADCSSL = 0x0800;    //0 => Skip, 1 => Scan
        adc_ch_status[11] = 1;
        //===========================================================================
        // Configure CH0 Sample and Hold for 200kbps
        //  +-- Use MUX A only
        //  +-- Set CH0 S/H -ve to VRef-
        ADCHSbits.CH0NA = 0;
        //===========================================================================
        // ADCCON3:
        //  +--Auto Sample Time = 1TAD
        //  +--A/D Conversion Clock Source = Use system clk
        //  +--A/D Conversion Clock Select ADCS<5:0>= 2(TAD/TCY)-1
        //      200kbps(Sampling frequency)
        ADCON3bits.SAMC = ADC_ACQ_TIME;     //1TAD for sampling time
        ADCON3bits.ADRC = 0;                //Use system clk
        ADCON3bits.ADCS = ADC_ADCS;         //each conversion requires 14TAD
        //===========================================================================
        // ADCCON2:
        //  +--Default: Use MUX A, No splitting of Buffer
        //  +--Voltage Reference Configuration Vref+ and Vref-
        //  +--Scan Input Selections
        //  +--5 samples between interrupt
        ADCON2bits.VCFG = 0;                //AVSS, AVDD
        ADCON2bits.CSCNA = 1;               //Scan input
        ADCON2bits.SMPI = 0;                //take 1 sample per interrupt
        //===========================================================================
        // ADCCON1:
        //  +--Default: continue in idle mode, integer format
        //  +--Enable ADC, Conversion Trigger Source Auto, Auto sampling on
        ADCON1bits.FORM = 0;                //[0:integer]; [2 fractional]; [3 siged fractional]
        ADCON1bits.SSRC = 7;                //auto covert, using internal clock source
        ADCON1bits.ASAM = 1;                //auto setting of SAMP bit
        ADCON1bits.ADON = 1;                //Turn on module
        //===========================================================================
        adc_buf_ptr = &ADCBUF0;             //initialize pointer to adc data
        return 0;
    }
}

/*******************************************************************************
 * ADC Interrupt Service Rountine
 * Function: copy the adc data to adc_queue[adc_queue_ptr]
 * This routine requires about 7us for execution (outdated)
 *******************************************************************************/
void _ISR _ADCInterrupt(void)
{
    //update queue ptr
    adc_queue_ptr++;
    if(adc_queue_ptr==ADC_QSIZE) adc_queue_ptr = 0;
    //copy selected channel data to queue
    unsigned int channel = 0;
    unsigned int buffer = 0;
    for (; channel<ADC_MAX_CH; channel++)
    {
        if(adc_ch_status[channel])
        {
            adc_queue[adc_queue_ptr][channel] = adc_buf_ptr[buffer];
            buffer++;
        }
    }
    adc_data_ready = 1;
    //==========================================================
    DISI_PROTECT(_ADIF = 0);  //Clear adc interrupt
}

#endif //end dsPic30



#ifdef MPLAB_DSPIC33_PORT
/************************************************************************************************
 * OPERATION OF ADC IN DSPIC33
 * ===========================
 * 
 *    CH          0   2   3        0   2   3        0   2   3        0   2   3
 *                |   |   |        |   |   |        |   |   |        |   |   |
 *                |   |   |   ...  |   |   |   ...  |   |   |   ...  |   |   | 
 *               \|/ \|/ \|/      \|/ \|/ \|/      \|/ \|/ \|/      \|/ \|/ \|/ 
 *          --------------------------------------------------------------------------> Time
 * 
 *                DMA BufA         DMA BufB         DMA BufA         DMA BufB
 *              +----------+     +----------+     +----------+     +----------+
 *              | ch0(n-3) |     | ch0(n-2) |     | ch0(n-1) |     |  ch0(n)  |
 *              |   ---    |     |   ---    |     |   ---    |     |   ---    |
 *              | ch2(n-3) |     | ch2(n-2) |     | ch2(n-1) |     |  ch2(n)  |
 *              | ch3(n-3) |     | ch3(n-2) |     | ch3(n-1) |     |  ch3(n)  |
 *              |   ...    |     |   ...    |     |   ...    |     |   ...    |
 *              +----------+     +----------+     +----------+     +----------+
 *                    |                |                |                |
 * INTERRUPT          |                |                |                |
 *                   \|/              \|/              \|/              \|/
 *              adc_queue[0]     adc_queue[1]     adc_queue[2]     adc_queue[0]
 *              +----------+     +----------+     +----------+     +----------+
 *              | ch0(n-3) |     | ch0(n-2) |     | ch0(n-1) |     |  ch0(n)  |
 *              |   ---    |     |   ---    |     |   ---    |     |   ---    |
 *              | ch2(n-3) |     | ch2(n-2) |     | ch2(n-1) |     |  ch2(n)  |
 *              | ch3(n-3) |     | ch3(n-2) |     | ch3(n-1) |     |  ch3(n)  |
 *              |   ...    |     |   ...    |     |   ...    |     |   ...    |
 *              +----------+     +----------+     +----------+     +----------+
 ************************************************************************************************/
static unsigned int adc_bufA[ADC_MAX_CH] __attribute__((space(dma),aligned(256)));     //Buffer A in Ping Pong mode, reside in RAM
static unsigned int adc_bufB[ADC_MAX_CH] __attribute__((space(dma),aligned(256)));     //Buffer B in Ping Pong mode, reside in RAM
static unsigned int which_dma = 0;         //indicate which adc_buf to be used


/************************************************************************************************
 * Name:                int adc_open(int flags)
 * 
 * Input:               flags: accessing mode
 * 
 * Output:              -1 if write mode is selected (errno = EROFS)
 *                      0 if ok
 * 
 * Function:            Initialize ADC for dsPic33
 ************************************************************************************************/
int adc_open(int flags)
{
    if(flags & O_RDWR || flags & O_WRONLY){
        errno = EROFS;
        return -1;
    }
    else
    {
        adc_io_flag = flags;
        //===========================================================================
        // Configure interrupt
        _ADIF = 0;              //clear ADC interrupt flag
        _ADIE = 0;              //disable adc interrupt
        //===========================================================================
        // Configure CH0 Sample and Hold 
        //  +-- Use MUX A only
        //  +-- Set CH0 S/H -ve to VRef-
        ADCHSbits.CH0NA = 0;
        //===========================================================================
        // Configure analog i/o  
        _TRISB15 = 1;    
        ADPCFG = 0x7FFF;        //0 => Enabled, 1 => Disabled
        ADPCFGH = 0xFFFF;       //AN16-AN31: Disabled
        adc_ch_status[15] = 1;
        //===========================================================================
        // Configure scan input channels    
        ADCSSL = 0x8000;    //0 => Skip, 1 => Scan
        ADCSSH = 0x0000;    //Skipping AN16-AN31
        //===========================================================================
        // ADCCON4:
        ADCON4bits.DMABL = 0;    // Each buffer contains 1 word
        //===========================================================================
        // ADCCON3:
        //  +--Auto Sample Time = 1TAD
        //  +--A/D Conversion Clock Source = Use system clk
        //  +--A/D Conversion Clock Select ADCS<5:0>= (TAD/TCY)-1
        ADCON3bits.SAMC = ADC_ACQ_TIME; //1TAD for sampling time
        ADCON3bits.ADRC = 0;            //Use system clk
        ADCON3bits.ADCS = ADC_ADCS;     //each conversion requires 14TAD
        //===========================================================================
        // ADCCON2:
        //  +--Default: Use MUX A, No splitting of Buffer
        //  +--Voltage Reference Configuration AVDD and AVSS
        //  +--Scan Input Selections
        ADCON2bits.VCFG = 0;    //AVDD and AVSS
        ADCON2bits.CSCNA = 1;   //Scan input
        ADCON2bits.SMPI = 0;    //1 channel is scanned initially
        //===========================================================================
        // ADCCON1:
        //  +--Default: continue in idle mode, integer format
        //  +--Enable ADC, Conversion Trigger Source Auto, Auto sampling on
        ADCON1bits.FORM = 0;        //[0:integer]; [2 fractional]; [3 siged fractional]
        ADCON1bits.SSRC = 7;        //auto covert, using internal clock source
        ADCON1bits.ASAM = 1;        //auto setting of SAMP bit
        ADCON1bits.AD12B = 1;       //12-bit, 1-channel ADC operation
        ADCON1bits.ADDMABM = 0;     // DMA buffers are built in scatter/gather mode
        ADCON1bits.ADON = 1;        // Turn on the A/D converter    
        //===========================================================================
        // DMA0 Configuration:
        DMA0CONbits.AMODE = 2;      // Configure DMA for Peripheral indirect mode
        DMA0CONbits.MODE  = 2;      // Configure DMA for Continuous Ping-Pong mode
        DMA0PAD=(int)&ADC1BUF0;     
        DMA0CNT = 0;                // generate dma interrupt every 1 samples 
                                    // same as SMPI because only 1 dma buffer per channel         
        DMA0REQ = 13;               // Select ADC1 as DMA Request source
        DMA0STA = __builtin_dmaoffset(adc_bufA);     
        DMA0STB = __builtin_dmaoffset(adc_bufB);
        _DMA0IF = 0;                // Clear the DMA interrupt flag bit
        _DMA0IE = 1;                // Set the DMA interrupt enable bit
        DMA0CONbits.CHEN=1;         // Enable DMA        
        return 0;
    }
}

/*******************************************************************************
 * ADC Interrupt Service Rountine
 * Function: keep ADC16Ptr up-to-date
 * This routine requires about 6us for execution
 *******************************************************************************/
void _ISR _DMA0Interrupt(void)
{
    //update dma pointer
    adc_buf_ptr = (which_dma == 0)? adc_bufA : adc_bufB;
    which_dma ^= 1;
    //update queue ptr
    adc_queue_ptr++;
    if(adc_queue_ptr==ADC_QSIZE) adc_queue_ptr = 0;
    //copy selected channel data to queue
    unsigned int channel = 0;
    for (; channel<ADC_MAX_CH; channel++)
    {
        if(adc_ch_status[channel] == 1)
        {
            adc_queue[adc_queue_ptr][channel] = adc_buf_ptr[channel];
        }
    }
    adc_data_ready = 1;
    _DMA0IF = 0;
}
#endif //end dsPic33  




/********************************************************************************************
 * Function:    int adc_read(unsigned int* buf, int count)
 * 
 * Description: copy adc queue data for channel adc_ch_select to $buf
 * 
 * Input:       buf: pointer to buffer for reading
 *              count: number of bytes to copy
 * 
 * Output:      number of data copied
 *              2n bytes: copied n data, 2 bytes each
 *              -1: data not ready (errno = EAGAIN)
 ********************************************************************************************
 * Results:     most recent data at location[0]
 *  
 *            +----------+
 *    buf[0]  |  chX(n)  |
 *    buf[1]  | chX(n-1) |   
 *    buf[2]  | chX(n-2) |
 *            |   ...    |
 *            +----------+
 *********************************************************************************************/
static int adc_read_channel(unsigned int* buf, int count)
{
    int num_of_samples = count/2;
    int queue_index = adc_queue_ptr;
    int i;
    for(i=0; (i<num_of_samples) && (i<ADC_QSIZE); i++){
        buf[i] = adc_queue[queue_index][adc_ch_select];
        if(--queue_index < 0) queue_index = ADC_QSIZE-1;
    }
    return 2*i;    
}
int adc_read(unsigned int* buf, int count)
{
    //Non-Blocking io
    if(adc_io_flag & O_NONBLOCK)
    {
        if(adc_data_ready == 1)
        {
            return adc_read_channel(buf, count);
        }
        errno = EAGAIN;
        return -1;  //data is not ready
    }
    //Blocking io
    else
    {
        while(adc_data_ready == 0);
        return adc_read_channel(buf, count);
    }
}



/*******************************************************************************
 * Function:        int adc_ioctl(int request, unsigned char* argp)
 * 
 * Description:     change the config of adc module
 * 
 * Input:           request: Request code - defined in ioctl.h
 *                    +--ADC_ADD_CH
 *                  argp: pointer for control config, request code dependent.
 *                    +--argp[0]: channel id to be added, take values of 0-15
 * 
 * Output:          on success zero is returned, -1 indicates error          
 ********************************************************************************/
#ifdef MPLAB_DSPIC33_PORT
#define ADD_ON_CODE()       DMA0CNT++   //Increment counter
#else
#define ADD_ON_CODE()       //Do nothing
#endif
#define adcAdd(ch)  {\
    adc_ch_status[ch] = 1;\
    unsigned int mask;\
    mask = 0x0001 << ch;\
    /*Enable i/o pin as input==============================================*/\
    TRISB = TRISB | mask;\
    /*=====================================================================*/\
    /*Scan select:*/\
    /*  +-- 0 => Skip, 1 => Scan*/\
    /*  +-- Example: ch = 4, ADCSSL = 0000 1111 0000 0100 (0x0F04)*/\
    /*      +--  0000 1111 0000 0100 | 0000 0000 0001 0000 = 0000 1111 0001 0100 (0x0F14)*/\
    ADCSSL = ADCSSL | mask;\
    /*=====================================================================*/\
    /*Port config:*/\
    /*  +-- 0 => Analog, 1 => Digital*/\
    /*  +-- Example: ch = 4, ADPCFG = 1111 0000 1111 1011 (0xF0FB)*/\
    /*      +--  ~0000 1111 0001 0100 = 1111 0000 1110 1011 (0xF0EB)*/\
    ADPCFG = ~ADCSSL;\
    /*=====================================================================*/\
    ADCON2bits.SMPI++;  /*take one more sample per interrupt*/\
    ADD_ON_CODE();\
}
int adc_ioctl(int request, unsigned char* argp)
{
    switch(request)
    {
        case ADC_ADD_CH:
            //ADD channels to current set==========================
            cli();                      //Disable global interrupt
            if(adc_ch_status[argp[0]] == 0){     //If channel not in scan list
                adcAdd((unsigned int)argp[0]);  //Add individual channel to scan list
                adc_data_ready = 0;              //First data not ready yet, until interrupt occurs
            }
            adc_ch_select = (unsigned int)argp[0];    //Select current channel for reading
            sti();                      //Enable global interrupt
            break;
        default:
            return -1;      //request code not recognised   
    }
    return 0;
}
#endif //ADC_MOD
