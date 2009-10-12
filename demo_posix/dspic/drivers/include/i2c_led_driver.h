/**
 * \addtogroup drivers Drivers
 * @{
 *
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup i2c_led_driver I2C LED Driver
 * @{
 *
 * Control I2C LED Driver SC620ULTRT
 * \li The I2C module uses SCL and SDA, located at pin 36 and 37 respectively.
 * \li All I2C devices shares a common communication speed (default: 400kHz)
 * \li The driver has a POSIX-like interface with open(), read(), write(), ioctl()
 * \li When i2c has multiple devices, read(), write() cannot be used in ISR (Interrupt routine)
 */

/**
 * \file
 * I2C LED Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

#ifndef I2C_LED_DRIVER_H_
#define I2C_LED_DRIVER_H_

#ifdef I2C_LED_DRIVER_MOD

/**
 * \brief Initialise I2C LED DRIVER
 * \param flags accessing mode
 * \retval 0 LED DRIVER opened
 */
extern int i2c_led_driver_open (int flags);


/**
 * \brief write 2 bytes to LED DRIVER
 * \param buf pointer of data to write to LED DRIVER
 * \return number of bytes written
 * \retval 0 no data has been written
 * \retval 2 two bytes has been written
 * \retval -1 not opened for writing error (errno = EBADF)
 *
 * \remarks
 * \li example
 * \verbatim
    Mst/Slv    _______ M ___M___ M S ____M___ S ____M___ S M ________
    SDA (Data)        |S|       | |A|        |A|        |A|S|
                      |T|address|W|C|Control |C|  Data  |C|T|
                      |A|1110000|0|K|0000xxxx|K|xxxxxxxx|K|P|
   \endverbatim
 */
extern int i2c_led_driver_write (unsigned char *buf);


/**
 * \brief change setting for LED DRIVER
 * \param request Request code defined in ioctl.h
 * \param argp pointer for control configuration, request code dependent.
 * \retval 0 success
 * \retval -1 error
 */
extern int i2c_led_driver_ioctl (int request, unsigned char* argp);

#endif /* I2C_LED_DRIVER_MOD */

#endif /* I2C_LED_DRIVER_H_ */

/** @} */
/** @} */

