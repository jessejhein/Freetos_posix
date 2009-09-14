/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup adc ADC
 * @{
 * 
 * Control ADC Port
 * \li The driver has a POSIX-compliant interface with open(), read(), and ioctl().
 * \li Operation of ADC in dsPic30F and dsPic33F is different. For scanning of multiple channels,
 *     dsPic30F uses ADC interrupt, while dsPic33F uses DMA interrupt.
 * \li Users should take note that:
 * \n  a) On open(), no sampling is performed. Sampling only begins after the first successful ioctl()
 * \n  b) The highest sampling frequency for dsPic30F and dsPic33F are 200kbps and 500kbps respectively 
 * \n  c) Users should first add the channel using ioctl() before reading from the channel.
 * \n  d) There could be a delay between execution of ioctl() and read() to obtain the data,
 *        the delay is indicated by returning -1 from read(). User may poll the return value
 *        to ensure a valid reading. 
 * \n  e) The interrupt routine for dsPic30F and dsPic33F requires about ?us and 30us for execution.
 *        User should make sure that the adc sampling time is at least greater than this value, 
 *        and it has adequate processing time for other processes.
 */

/**
 * \file
 * ADC Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */


#ifdef ADC_MOD

#include <define.h>
#include <sys/ioctl.h>
#include <asm/system.h>
#include <asm/types.h>
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
/** bitwise adc status: 1=on, 0=off */
static unsigned long adc_ch_status = 0;
/** indicate the current selected channel for read() */
static unsigned int adc_ch_select = 0;
/** Indicate if RAM data is ready for output */
static unsigned int adc_data_ready = 0;
/** Pointer to ADC register buffer, volatile because data is changed by hardware */
static volatile unsigned int* adc_buf_ptr;
/** Store most updated data, index of array referenced by channel ID */
static unsigned int adc_queue[ADC_QSIZE][ADC_MAX_CH];
/** pointer to the most updated sample */
static unsigned int adc_queue_ptr = ADC_QSIZE - 1;

/**
 * \brief get the adc channel status
 * \param ch channel id
 * \ret 0 not turned on
 * \ret >1 turned on
 */
#define GET_CH_STATUS(ch)                           (((unsigned long)0x0001 << ch) & adc_ch_status)

/**
 * \brief set the adc channel to on state
 * \param ch channel id
 */
#define SET_CH_STATUS_ON(ch)                        (adc_ch_status |= ((unsigned long)0x0001 << ch))


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


//--------------------------------------------------------------------------------------
/**
 * \brief Initialize ADC for dsPic30
 * \param flags accessing mode
 * \retval -1 error, write mode is selected (errno = EROFS)
 * \retval 0 ok
 */
int 
adc_open (int flags)
{
  if (flags & O_RDWR || flags & O_WRONLY)
    {
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
      ADCON3bits.ADRC = 0;                //Use system clk
      ADCON3bits.SAMC = ADC_ACQ_TIME;     //1TAD for sampling time
      ADCON3bits.ADCS = ADC_ADCS;         //each conversion requires 14TAD
      //===========================================================================
      // ADCCON2:
      //  +--Default: Use MUX A, No splitting of Buffer
      //  +--Voltage Reference Configuration Vref+ and Vref-
      //  +--Scan Input Selections
      //  +--5 samples between interrupt
      ADCON2bits.VCFG = 0;                //AVSS, AVDD
      ADCON2bits.CSCNA = 1;               //Scan input
      //===========================================================================
      // ADCCON1:
      //  +--Default: continue in idle mode, integer format
      //  +--Enable ADC, Conversion Trigger Source Auto, Auto sampling on
      ADCON1bits.FORM = ADC_OUTPUT_FORMAT;
      ADCON1bits.SSRC = 7;                //auto covert, using internal clock source
      ADCON1bits.ASAM = 1;                //auto setting of SAMP bit
      ADCON1bits.ADON = 0;
      //===========================================================================
      adc_buf_ptr = (int) &ADCBUF0;       //initialize pointer to adc data
      return 0;
    }
}


//--------------------------------------------------------------------------------------
/**
 * \brief ADC Interrupt Service Rountine: copy the adc data to adc_queue[adc_queue_ptr]
 * \remarks This routine requires about ?us for execution
 * \remarks this routine is outdate and not tested
 */
void _IRQ 
_ADCInterrupt (void)
{
  //update queue ptr
  adc_queue_ptr++;
  if (adc_queue_ptr == ADC_QSIZE) adc_queue_ptr = 0;
  //copy selected channel data to queue
  unsigned int channel;
  unsigned int buffer = 0;
  for (channel = 0; channel < ADC_MAX_HW_CH; channel++)
    {
      if (GET_CH_STATUS (channel))
        {
          int local_buf_id = parse_adc_ch(channel);
          if ( (local_buf_id != -1) && (local_buf_id < ADC_MAX_CH) )
            {
              adc_queue[adc_queue_ptr][local_buf_id] = adc_buf_ptr[buffer];
              buffer++;
            }
        }
    }
  adc_data_ready = 1;
  //==========================================================
  DISI_PROTECT(_ADIF = 0);  //Clear adc interrupt
}
#endif /* MPLAB_DSPIC30_PORT */



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
/** Buffer A in Ping Pong mode, reside in RAM */
static _DMA unsigned int adc_bufA[ADC_MAX_HW_CH];
/** Buffer B in Ping Pong mode, reside in RAM */
static _DMA unsigned int adc_bufB[ADC_MAX_HW_CH];
/** indicate which adc_buf to be used */
static unsigned int which_dma = 0;


//--------------------------------------------------------------------------------------
/**
 * \brief Initialize ADC for dsPic33
 * \param flags accessing mode
 * \retval -1 error, write mode is selected (errno = EROFS)
 * \retval 0 ok
 */
int 
adc_open (int flags)
{
  if (flags & O_RDWR || flags & O_WRONLY)
    {
      errno = EROFS;
      return -1;
    }
  else
    {
      adc_io_flag = flags;
      //===========================================================================
      // Configure interrupt
      _AD1IF = 0;
      _AD1IE = 0;
      //===========================================================================
      // Configure Sample and Hold
      //  +-- Use MUX A only (no interleaving)
      //  +-- Set CH0 S/H -ve to VRef-
      AD1CHS0bits.CH0NA = 0;
      //===========================================================================
      // ADCCON4:
      //  +-- Each buffer contains 1 word
      AD1CON4bits.DMABL = 0;
      //===========================================================================
      // ADCCON3:
      //  +--A/D Conversion Clock Source = Use system clk
      //  +--Set Auto Sample Time = # of TAD
      //  +--A/D Conversion Clock Select ADCS<5:0>= (TAD/TCY)-1
      AD1CON3bits.ADRC = 0;
      AD1CON3bits.SAMC = ADC_ACQ_TIME;
      AD1CON3bits.ADCS = ADC_ADCS;
      //===========================================================================
      // ADCCON2:
      //  +--Default: Use MUX A, No splitting of Buffer
      //  +--Voltage Reference Configuration AVDD and AVSS
      //  +--Scan Input Selections
      AD1CON2bits.CSCNA = 1;
      AD1CON2bits.VCFG = 0;
      //===========================================================================
      // ADCCON1:
      //  +--Default: continue in idle mode, integer format
      //  +--Enable ADC, Conversion Trigger Source Auto, Auto sampling on
      AD1CON1bits.FORM = ADC_OUTPUT_FORMAT;
      AD1CON1bits.SSRC = 7;        //auto covert, using internal clock source
      AD1CON1bits.ASAM = 1;        //auto setting of SAMP bit
      AD1CON1bits.AD12B = 1;       //12-bit, 1-channel ADC operation
      AD1CON1bits.ADDMABM = 0;     //DMA buffers are built in scatter/gather mode
      AD1CON1bits.ADON = 0;
      //===========================================================================
      // DMA0 Configuration:
      DMA0CONbits.AMODE = 2;      // Configure DMA for Peripheral indirect mode
      DMA0CONbits.MODE  = 2;      // Configure DMA for Continuous Ping-Pong mode
      DMA0PAD = (int)&ADC1BUF0;
      DMA0REQ = 13;               // Select ADC1 as DMA Request source
      DMA0STA = __builtin_dmaoffset(adc_bufA);     
      DMA0STB = __builtin_dmaoffset(adc_bufB);
      _DMA0IF = 0;                // Clear the DMA interrupt flag bit
      _DMA0IE = 1;                // Set the DMA interrupt enable bit
      DMA0CONbits.CHEN = 1;
      return 0;
    }
}


//--------------------------------------------------------------------------------------
/**
 * \brief ADC Interrupt Service Rountine: keep ADC16Ptr up-to-date
 * \remarks This routine requires about 30us for execution
 */
void _IRQ 
_DMA0Interrupt (void)
{
  //update dma pointer
  adc_buf_ptr = (which_dma == 0)? adc_bufA : adc_bufB;
  which_dma ^= 1;
  //update queue ptr
  adc_queue_ptr++;
  if (adc_queue_ptr == ADC_QSIZE) adc_queue_ptr = 0;
  //copy selected channel data to queue
  unsigned int channel;
  for (channel = 0; channel < ADC_MAX_HW_CH; channel++)
    {
      if (GET_CH_STATUS (channel))
        {
          int local_buf_id = parse_adc_ch(channel);
          if ( (local_buf_id != -1) && (local_buf_id < ADC_MAX_CH) )
            adc_queue[adc_queue_ptr][local_buf_id] = adc_buf_ptr[channel];
        }
    }
  adc_data_ready = 1;
  _DMA0IF = 0;
}
#endif /* MPLAB_DSPIC33_PORT */



//--------------------------------------------------------------------------------------
static int 
adc_read_channel (unsigned int* buf, int count)
{
  int num_of_samples = count/2;
  int queue_index = adc_queue_ptr;
  int i;
  for (i = 0; (i < num_of_samples) && (i < ADC_QSIZE); i++)
    {
      buf[i] = adc_queue[queue_index][adc_ch_select];
      if (--queue_index < 0) queue_index = ADC_QSIZE - 1;
    }
  return 2*i;    
}

/**
 * \brief  copy adc queue data for channel adc_ch_select to $buf
 * \param buf pointer to buffer for reading
 * \param count number of bytes to copy
 * \return number of data copied
 * \retval >0 2n bytes: copied n data, 2 bytes each
 * \retval -1 data not ready (errno = EAGAIN)
 * \remarks 
 * Results:     most recent data at location[0]
 * 
 * \internal
 *  
 *            +----------+
 *    buf[0]  |  chX(n)  |
 *    buf[1]  | chX(n-1) |   
 *    buf[2]  | chX(n-2) |
 *            |   ...    |
 *            +----------+
 */
int 
adc_read (unsigned int* buf, int count)
{
  //Non-Blocking io
  if (adc_io_flag & O_NONBLOCK)
    {
      if (adc_data_ready == 1)
        {
          return adc_read_channel (buf, count);
        }
      errno = EAGAIN;
      return -1;  //data is not ready
    }
  //Blocking io
  else
    {
      while (adc_data_ready == 0);
      return adc_read_channel(buf, count);
    }
}



//--------------------------------------------------------------------------------------
/** keeps the total number of channel being scan */
static char adc_on_ch_num = 0;

#ifdef MPLAB_DSPIC33_PORT
#define ADD_ON_CODE()       (DMA0CNT = adc_on_ch_num - 1)   //Increment counter
#else
#define ADD_ON_CODE()       //Do nothing
#endif

/**
 * \brief change the config of adc module
 * \param request Request code - defined in ioctl.h (ADC_ADD_CH) 
 * \param argp channel id to be added
 * \retval 0 success
 * \retval -1 error
 */
int 
adc_ioctl (int request, unsigned char* argp)
{ 
  switch (request)
    {
      //ADD channels to current set
      case ADC_ADD_CH:
        {
          int ch_id = (unsigned int)argp[0];
          //Select current channel for reading
          int local_buf_id = parse_adc_ch(ch_id);
          if (local_buf_id == -1) return -1;
          adc_ch_select = local_buf_id;

          //add channel if not in scan list
          cli();
          if (GET_CH_STATUS(ch_id) == 0)
            {
              SET_CH_STATUS_ON(ch_id);
              add_adc(ch_id);
              
              //increment interrupt counter
              adc_on_ch_num++;
              ADCON2bits.SMPI = adc_on_ch_num -1;
              ADD_ON_CODE ();
              
              //First data not ready yet, until interrupt occurs
              adc_data_ready = 0;
              //Turn on the A/D converter if not already done so
              if (ADCON1bits.ADON == 0) ADCON1bits.ADON = 1;
            }
          sti();
          break;
        }
      //request code not recognised
      default:
        return -1;   
    }
  return 0;
}
#endif /* ADC_MOD */

/** @} */
/** @} */
