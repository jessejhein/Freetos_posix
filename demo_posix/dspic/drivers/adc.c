/************************************************************************************************
 * File:            adc.c
 * Description:     control ADC input ports
 ***********************************************************************************************
 * SUMMARY:
 * 1)   The 12-bit ADC module consists of 12 channels located at pin 16 (AN0), 15 (AN1), 14 (AN2), 
 *      13 (AN3), 12 (AN4), 11 (AN5), 17 (AN6), 18 (AN7), 21 (AN8), 22 (AN9), 23 (AN10), 24 (AN11),
 *      27 (AN12), 28 (AN13), 29 (AN14), 30 (AN15). These pins are shared with port B, PGC and PGD.
 * 2)   AN0, AN1 are devoted to Reference Voltage and should not be used. The pins of AN6 and AN7 
 *      are used by PGC and PGD for ICSP. These channels should also not be used. Hence, only 8
 *      channels are available.
 * 3)   The driver has a POSIX-compliant interface with open(), read(), and ioctl().
 * 4)   Operation of ADC in dsPic30F and dsPic33F is different. For scanning of multiple channels,
 *      dsPic30F uses ADC interrupt, while dsPic33F uses DMA interrupt.
 * 5)   Users should take note that:
 *      a) On open(), AN0 (Vref+) and AN1 (Vref-) are opened.
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

#include <define.h>
#include <sys/ioctl.h>
#include <asm/system.h>
#include <errno.h>
#include <fcntl.h>

#if (ADC_MOD == 0)
//do not include ADC module if disabled

#else 

/************************************************************************************************
 * Local Functions Declaration
 ************************************************************************************************/
static void adcAdd(unsigned int ch);               //Add channels to current set
static void adcRm(unsigned int ch);                //remove channels from current set
static int adc_read_channel(unsigned int* buf, int count);

#ifdef MPLAB_DSPIC30_PORT
/************************************************************************************************
 * ADC Data for dsPic30
 ************************************************************************************************/
static unsigned int adc_buf[ADC_MAX_CH];            //Store most updated data, 
                                                      //index of array referenced by channel ID 
static unsigned char adc_ch_status[ADC_MAX_CH];     //1=on, 0=off
static volatile unsigned int* ADC16Ptr = &ADCBUF0; //Pointer to ADC register buffer, 
                                                      //volatile because data is changed by hardware 
static unsigned int adc_ch_select = 0;              //Pointer to channel to be read from
static unsigned int adc_data_ready = 0;             //Indicate if RAM data is ready for output

static int adc_io_flag;

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
        _TRISB0 = 1;
        _TRISB1 = 1;    
        ADPCFG = 0xFFFC;                    //Enable AN0 (Vref+) and AN1 (Vref-)
        //===========================================================================
        // Configure scan input channels    
        ADCSSL = 0x0003;    //0 => Skip, 1 => Scan
        adc_ch_status[0] = 1;
        adc_ch_status[1] = 1;
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
        ADCON2bits.VCFG = 3;                //External Vref+, Vref-
        ADCON2bits.CSCNA = 1;               //Scan input
        ADCON2bits.SMPI = 1;                //take 2 samples (one sample per channel) per interrupt
        //===========================================================================
        // ADCCON1:
        //  +--Default: continue in idle mode, integer format
        //  +--Enable ADC, Conversion Trigger Source Auto, Auto sampling on
        ADCON1bits.FORM = 0;                //[0:integer]; [2 fractional]; [3 siged fractional]
        ADCON1bits.SSRC = 7;                //auto covert, using internal clock source
        ADCON1bits.ASAM = 1;                //auto setting of SAMP bit
        ADCON1bits.ADON = 1;                //Turn on module
        //===========================================================================
        return 0;
    }
}

/*******************************************************************************
 * ADC Interrupt Service Rountine
 * Function: copy the adc data to buf
 * This routine requires about 7us for execution
 *******************************************************************************/
void _ISR _ADCInterrupt(void)
{
    unsigned int channel = 0;
    unsigned int buffer = 0;
    for (; channel<ADC_MAX_CH; channel++)
    {
        if(adc_ch_status[channel])       //Check if channel has been selected
        {
            adc_buf[channel] = ADC16Ptr[buffer];     //Copy data to adc_buf
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
 * ADC Data for dsPic33
 ************************************************************************************************/
static unsigned int adc_bufA[ADC_MAX_CH] __attribute__((space(dma),aligned(256)));     //Buffer A in Ping Pong mode, reside in RAM
static unsigned int adc_bufB[ADC_MAX_CH] __attribute__((space(dma),aligned(256)));     //Buffer B in Ping Pong mode, reside in RAM

static unsigned char adc_ch_status[ADC_MAX_CH];     //1=on, 0=off
static unsigned int* ADC16Ptr;             //Pointer to ADC register buffer, 

static unsigned int adc_ch_select = 0;     //Pointer to channel to be read from
static unsigned int adc_data_ready = 0;    //Indicate if RAM data is ready for output

static unsigned int which_dma = 0;         //indicate which adc_buf to be used

static int adc_io_flag;

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
        _TRISB0 = 1;
        _TRISB1 = 1;    
        ADPCFG = 0xFFFC;        //Enable AN0 (Vref+) and AN1 (Vref-)
        ADPCFGH = 0xFFFF;       //AN16-AN31: Disabled
        //===========================================================================
        // Configure scan input channels    
        ADCSSL = 0x0003;    //0 => Skip, 1 => Scan
        ADCSSH = 0x0000;    //Skipping AN16-AN31
        adc_ch_status[0] = 1;
        adc_ch_status[1] = 1;
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
        //  +--Voltage Reference Configuration Vref+ and Vref-
        //  +--Scan Input Selections
        //  +--5 samples between interrupt
        ADCON2bits.VCFG = 3;    //External Vref+, Vref-
        ADCON2bits.CSCNA = 1;   //Scan input
        ADCON2bits.SMPI = 1;    //2 channels are scanned
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
        DMA0CNT = 1;                // generate dma interrupt every 2 samples 
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
 * This routine requires about 500ns for execution
 *******************************************************************************/
void _ISR _DMA0Interrupt(void)
{
    ADC16Ptr = (which_dma == 0)? adc_bufA : adc_bufB;   //Update pointer
    adc_data_ready = 1;
    which_dma ^= 1;                                     //Next buffer to be used
    _DMA0IF = 0;                                        //Clear the DMA0 Interrupt Flag
}
#endif //end dsPic33  

/********************************************************************************************
 * Name:        int adc_read(unsigned int* buf, int count)
 * 
 * Function:    copy adc data to $buf
 * 
 * Input:       buf: pointer to buffer for reading
 *              count: number of bytes to copy
 * 
 * Ouput:       number of data copied
 *              2n bytes: copied n data, 2 bytes each
 *              -1: data not ready (errno = EAGAIN)
 * 
 *********************************************************************************************/
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

static int adc_read_channel(unsigned int* buf, int count)
{
    //===============================================
    int num_channel = count/2;                  //number of channels to read
    unsigned int channel = adc_ch_select;      //index for adc_buf
    int i = 0;                                  //index for buf
    //===============================================
    while(i<num_channel && channel<ADC_MAX_CH)
    { 
        //Loop only for specified number of channel or all channels (whichever is lower)
#ifdef MPLAB_DSPIC30_PORT
        buf[i++] = adc_buf[channel++];      //use data in local buffer
#endif            
#ifdef MPLAB_DSPIC33_PORT
        buf[i++] = ADC16Ptr[channel++];     //use data in dma
#endif
        while(adc_ch_status[channel]==0)
        {  //increment to next valid channel
            channel++;                  
            if(channel >= ADC_MAX_CH) break;
        }
    }
    //===============================================
    return 2*i;    
}

/*******************************************************************************
 * adc_ioctl
 *******************************************************************************
 * Inputs:
 *  +--request: Request code - defined in ioctl.h
 *  +--argp: pointer for control config, request code dependent.
 *      +--ADC_ADD_CH, ADC_RM_CH
 *          +--argp[0]: channel id to be added/removed, take values of 0-15
 *******************************************************************************
 * Return:
 *  on success zero is returned, -1 indicates error
 ********************************************************************************/
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
        case ADC_RM_CH:
            //REMOVE channels from current set==========================
            cli();                  //Disable global interrupt
            if(adc_ch_status[argp[0]] == 1){     //If channel in scan list       
                adcRm((unsigned int)argp[0]);   //Remove individual channel
                adc_ch_select = 0;               //Reset to AN0
            }
            sti();                  //Enable global interrupt
            break;
        default:
            return -1;      //request code not recognised   
    }
    return 0;
}

/*******************************************************************************
 * adcAdd
 *******************************************************************************
 * Input:
 *  +--ch: channel to be add for scanning
 ********************************************************************************/
static void adcAdd(unsigned int ch)
{
    adc_ch_status[ch] = 1;
    unsigned int mask;
    mask = 0x0001 << ch;
    //Enable i/o pin as input==============================================
    TRISB = TRISB | mask;
    //=====================================================================
    //Scan select:
    //  +-- 0 => Skip, 1 => Scan
    //  +-- Example: ch = 4, ADCSSL = 0000 1111 0000 0100 (0x0F04)
    //      +--  0000 1111 0000 0100 | 0000 0000 0001 0000 = 0000 1111 0001 0100 (0x0F14)  
    ADCSSL = ADCSSL | mask;      
    //=====================================================================
    //Port config:
    //  +-- 0 => Analog, 1 => Digital
    //  +-- Example: ch = 4, ADPCFG = 1111 0000 1111 1011 (0xF0FB)
    //      +--  ~0000 1111 0001 0100 = 1111 0000 1110 1011 (0xF0EB)  
    ADPCFG = ~ADCSSL;
    //=====================================================================
    ADCON2bits.SMPI++;  //take one more sample per interrupt
#ifdef MPLAB_DSPIC33_PORT
    DMA0CNT++; 
#endif
}

/*******************************************************************************
 * adcRm
 *******************************************************************************
 * Input:
 *  +--ch: channel to be removed for scanning
 *  +--after execution, channel will become digital input
 ********************************************************************************/
static void adcRm(unsigned int ch)
{
    adc_ch_status[ch] = 0;
    unsigned int mask;
    mask = 0x0001 << ch;
    //=====================================================================
    //Port config:
    //  +-- 0 => Analog, 1 => Digital
    //  +-- Example: ch = 4, ADPCFG = 1111 0000 1110 1011 (0xF0EB) 
    //      +-- 1111 0000 1110 1011 | 0000 0000 0001 0000 = 1111 0000 1111 1011 (0xF0FB)
    ADPCFG = ADPCFG | mask;
    //=====================================================================
    //Scan select:
    //  +-- 0 => Skip, 1 => Scan
    //  +-- Example: ch = 4, ADCSSL = 0000 1111 0001 0100 (0x0F14) 
    //      +--  ~1111 0000 1111 1011 = 0000 1111 0000 0100 (0x0F04)  
    ADCSSL = ~ADPCFG;
    //=====================================================================
    ADCON2bits.SMPI--;  //take one less sample per interrupt
#ifdef MPLAB_DSPIC33_PORT
    DMA0CNT--; 
#endif
}

#endif //ADC_MOD
