/**
 * \addtogroup drivers Drivers
 * @{
 *
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup i2c I2C
 * @{
 *
 * Control I2C
 * \li The I2C module uses SCL and SDA, located at pin 36 and 37 respectively.
 * \li All I2C devices shares a common communication speed (default: 400kHz)
 * \li The driver has a POSIX-like interface with open(), read(), write(), ioctl()
 */

/**
 * \file
 * I2C Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

#ifndef I2C_H_
#define I2C_H_

#ifdef I2C_MOD

#if (I2C_NUM > 1)
#include <pthread.h>
/**
 * \brief Semaphore for multiple I2C devices
 * \remarks program must acquire i2c_busy before read/write operation (from start bit to stop bit)
 */
extern pthread_mutex_t i2c_mutex;
#endif /* I2C_NUM > 1 */


/**
 * \brief Initialise I2C with assigned baudrate
 */
extern void i2c_open (void);


/**
 * \brief send a byte via I2C
 * \param buf pointer to data for transmission
 * \retval 0 failure
 * \retval 1 success
 * \remarks Precondition:
 * \li use i2c_ioctl() to set i2c_status appropriately (START or RESTART)
 * \verbatim
   Mst/Slv    _______ M ____M___ S M ________
   SDA (Data)        |S|  data  |A|S|
                     |T|        |C|T|
                     |A|XXXXXXXX|K|P|
   \endverbatim
 */
extern int i2c_write (unsigned char *buf);


/**
 * \brief read a byte via I2C with ACK/NACK
 * \param buf pointer to data for reception
 * \retval 0 failure
 * \retval 1 success
 * \remarks  Precondition:
 * \li use i2c_ioctl() to set i2c_status appropriately (NACK and/or STOP if required)
 * \li target device has been set appropriately
 * \verbatim
   Mst/Slv     ____ ___S____ M M _____
   SDA (Data)      |  data  |A|S|
                   |        |C|T|
                   |XXXXXXXX|K|P|
   \endverbatim
 */
extern int i2c_read (unsigned char *buf);


/**
 * \brief set the local parameters of i2c
 * \param request Request code - defined in ioctl.h
 * \param argp pointer for control configuration, I2C_SET_STATUS
 * \retval -1 failure
 * \retval 0 success
 */
extern int i2c_ioctl (int request, unsigned char* argp);

#endif /* I2C_MOD */

#endif /* I2C_H_ */

/** @} */
/** @} */
