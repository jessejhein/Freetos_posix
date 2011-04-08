/**
 * \addtogroup drivers Drivers
 * @{
 *
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup i2c_adc I2C ADC
 * @{
 *
 * Control I2C ADC ADS7823 (12-bit I2C ADC)
 * \li The I2C module uses SCL and SDA, located at pin 36 and 37 respectively.
 * \li All I2C devices shares a common communication speed (default: 400kHz)
 * \li The driver has a POSIX-like interface with open(), read(), write(), ioctl()
 * \li When i2c has multiple devices, read(), write() cannot be used in ISR (Interrupt routine)
 * \li At most 4 ADC data (8 bytes) can be read per operation.
 */

/**
 * \file
 * I2C ADC Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

#ifndef I2C_ADC_H_
#define I2C_ADC_H_

#ifdef I2C_ADC_MOD

/**
 * \brief Initialise I2C ADC
 * \param flags accessing mode
 * \retval 0 ADC opened
 */
extern int i2c_adc_open (int flags);


/**
 * \brief read 2 bytes from ADC
 * \param buf pointer of data to read from ADC
 * \param count number of bytes to read
 * \return number of bytes read
 * \retval 0 no data has been read
 * \retval -1 not opened for reading error (errno = EBADF)
 *
 * \remarks
 * \li example
 * \verbatim
                                        |<--ADC Conversion-->|
    Mst/Slv    _______ M ___M___ M S ____M___ S M ___M___ M S ___S____ M ___S____ M          ___S____ M ___S____ M M _____
    SDA (Data)        |S|       | |A|        |A|R|       | |A|        |A|        |R|                 |A|        |N|S|
                      |T|address|W|C|command |C|E|address|R|C|Data0 H |C|Data0 L |E|   ...   Data3 H |C|Data3 L |A|T|
                      |A|1001000|0|K|000xxxxx|K|S|1001000|1|K|0000xxxx|K|xxxxxxxx|S|         0000xxxx|K|xxxxxxxx|K|P|
   \endverbatim
 */
extern int i2c_adc_read (unsigned int *buf, int count);


/**
 * \brief change setting for ADC
 * \param request Request code defined in ioctl.h
 * \param argp pointer for control configuration, request code dependent.
 * \retval 0 success
 * \retval -1 error
 */
extern int i2c_adc_ioctl (int request, unsigned char* argp);


#endif /* I2C_ADC_MOD */

#endif /* I2C_ADC_H_ */

/** @} */
/** @} */

