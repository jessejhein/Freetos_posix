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
 * \li The driver has a POSIX-compliance interface with open(), read(), and ioctl().
 * \li Users should take note that:
 * \n  a) On open(), no sampling is performed. Sampling only begins after the first successful ioctl()
 * \n  b) The highest sampling frequency for dsPic33F is 500kbps respectively
 * \n  c) Users should first add the channel using ioctl() before reading from the channel.
 * \n  d) There could be a delay between execution of ioctl() and read() to obtain the data,
 *        the delay is indicated by returning -1 from read(). User may poll the return value
 *        to ensure a valid reading.
 * \n  e) The interrupt routine for dsPic33F requires about 30us for execution.
 *        User should make sure that the ADC sampling time is at least greater than this value,
 *        and it has adequate processing time for other processes.
 */

/**
 * \file
 * ADC Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

#ifndef ADC_H_
#define ADC_H_

#ifdef ADC_MOD

/**
 * \brief Initialise ADC
 * \param flags accessing mode
 * \retval -1 error, write mode is selected (errno = EROFS)
 * \retval 0 ok
 * \verbatim
   OPERATION OF ADC IN DSPIC33
   ===========================

      CH          0   2   3        0   2   3        0   2   3        0   2   3
                  |   |   |        |   |   |        |   |   |        |   |   |
                  |   |   |   ...  |   |   |   ...  |   |   |   ...  |   |   |
                 \|/ \|/ \|/      \|/ \|/ \|/      \|/ \|/ \|/      \|/ \|/ \|/
            --------------------------------------------------------------------------> Time

                  DMA BufA         DMA BufB         DMA BufA         DMA BufB
                +----------+     +----------+     +----------+     +----------+
                | ch0(n-3) |     | ch0(n-2) |     | ch0(n-1) |     |  ch0(n)  |
                |   ---    |     |   ---    |     |   ---    |     |   ---    |
                | ch2(n-3) |     | ch2(n-2) |     | ch2(n-1) |     |  ch2(n)  |
                | ch3(n-3) |     | ch3(n-2) |     | ch3(n-1) |     |  ch3(n)  |
                |   ...    |     |   ...    |     |   ...    |     |   ...    |
                +----------+     +----------+     +----------+     +----------+
                      |                |                |                |
   INTERRUPT          |                |                |                |
                     \|/              \|/              \|/              \|/
                adc_queue[0]     adc_queue[1]     adc_queue[2]     adc_queue[0]
                +----------+     +----------+     +----------+     +----------+
                | ch0(n-3) |     | ch0(n-2) |     | ch0(n-1) |     |  ch0(n)  |
                |   ---    |     |   ---    |     |   ---    |     |   ---    |
                | ch2(n-3) |     | ch2(n-2) |     | ch2(n-1) |     |  ch2(n)  |
                | ch3(n-3) |     | ch3(n-2) |     | ch3(n-1) |     |  ch3(n)  |
                |   ...    |     |   ...    |     |   ...    |     |   ...    |
                +----------+     +----------+     +----------+     +----------+
   \endverbatim
 */
extern int adc_open (int flags);


/**
 * \brief copy ADC queue data for channel adc_ch_select to buffer
 * \param buf pointer to buffer for reading
 * \param count number of bytes to copy
 * \return number of data copied
 * \retval >0 2n bytes: copied n data, 2 bytes each
 * \retval -1 data not ready (errno = EAGAIN)
 * \remarks Results: most recent data at location[0]
 * \verbatim
             +----------+
     buf[0]  |  chX(n)  |
     buf[1]  | chX(n-1) |
     buf[2]  | chX(n-2) |
             |   ...    |
             +----------+
   \endverbatim
 */
extern int adc_read (unsigned int* buf, int count);


/**
 * \brief change the configuration of ADC module
 * \param request Request code - defined in ioctl.h (ADC_ADD_CH)
 * \param argp channel id to be added
 * \retval 0 success
 * \retval -1 error
 */
extern int adc_ioctl (int request, unsigned char* argp);

#endif /* ADC_MOD */

#endif /* ADC_H_ */

/** @} */
/** @} */
