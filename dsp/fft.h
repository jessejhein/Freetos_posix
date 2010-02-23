/**
 * \addtogroup dsp Digital Signal Processing Library
 * @{
 * 
 * Implementation of DSP API
 */

/**
 * \defgroup fft_lib Fast Fourier Transform
 * @{
 * 
 * Implementation of Fast Fourier Transform
 */

/**
 * \file
 * Fast Fourier Transform
 * \author Dennis Tsang <dennis@amonics.com>
 */

/**
 * \page fft_page
 * \section fft_sec1 INTRODUCTION
 * \li This FFT library assume computation to be in-place to reduce RAM usage (i.e. original data are overwritten) 
 * \section fft_sec2 COMPILE TIME CONFIGURATION
 * \par Set the following before compiling
 * \subsection fft_sec1_1 <.config_app>:
 * \verbatim
    CONFIG_ADC_SAMPLE_RATE            =   20480
    CONFIG_ADC_ACQ_TIME               =   31
    CONFIG_ADC_MAX_CH                 =   1
    CONFIG_ADC_QSIZE                  =   512
    CONFIG_ADC_OUTPUT_FORMAT          =   SIGNED_FRACTIONAL

    CONFIG_FFT                        =   y
    CONFIG_FFT_BLOCK_LENGTH           =   512
    CONFIG_FFT_LOG2_BLK_LEN           =   9                     //= log2(FFT_BLOCK_LENGTH)
   \endverbatim 
 */

#include <asm/types.h>

/**
 * \brief Assign sampled value to data point
 * \param index data point id
 * \param value value of sample data (adc raw data)
 */
extern void fft_set_data (int index, real_t value);


/**
 * \brief Perform a Fast Fourier Transform
 * \return pointer to result
 */
extern complex_t* fft (void);


/**
 * \brief Get the square magnitude distribution from an FFT result
 * \param data pointer to source (fft-ed) data
 * \param index store the index corresponding the maximum value
 * \return pointer to result
 */
extern real_t* fft_sq_magnitude (complex_t* data, int* index);

/** @} */
/** @} */

