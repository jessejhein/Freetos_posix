/**
 * \addtogroup drivers Drivers
 * @{
 *
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup uart UART
 * @{
 *
 * Control U1ART (RS485) and U2ART (RS232)
 * \li The UART module consists of 2 channels located at U1RX (34), U1TX, (33), U2RX (31) and U1TX (32).
 * \li U2ART is referenced by COM0, U1ART is referenced by COM1. By default, U2ART is used for RS232
 *     communication, and U1ART is used for RS485 communication.
 * \li Default communication speed is 19200bps. Autobaud detection is disabled.
 * \li The driver has a POSIX-compliance interface with open(), read(), and write().
 * \li When U2RX detects a break character, it resets the system for bootloader application
 */

/**
 * \file
 * UART Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

#ifndef UART_H_
#define UART_H_

#ifdef UART_MOD

/**
 * \brief Initialise UART Ports
 * \param device UART port id
 * \n             device 0 = Primary: U2ART (RS232)
 * \n             device 1 = Secondary: U1ART (RS485)
 * \param flags accessing mode
 * \retval 0 UART opened
 */
extern int uart_open (unsigned char device, int flags);


/**
 * \brief copy count bytes of the data from buffer and transmit
 * \param device UART port id
 * \param buf pointer to buffer for writing
 * \param count number of bytes to write
 * \retval >=0 the number of bytes written
 * \retval -1 error
 * \n errno = EBADF: UART is not opened for writing
 */
extern int uart_write (unsigned char device, unsigned char *buf, int count);


/**
 * \brief copy the received data to buffer
 * \param device UART port id
 * \param buf pointer to buffer for reading
 * \retval 1 indicating 1 byte has been read
 * \retval 0 indicating no data is available
 * \retval -1 error, UART is not opened for read operation (errno = EBADF)
 */
extern int uart_read (unsigned char device, unsigned char *buf);

/**
 * \brief close UART
 * \param device UART port id
 * \retval 0 success
 * \retval -1 error
 */
extern int uart_close (unsigned char device);

/**
 * \brief change setting for UART
 * \param device UART id
 * \param request Request code defined in ioctl.h
 * \param argp pointer for control configuration, request code dependent.
 * \retval 0 success
 * \retval -1 error
 */
extern int uart_ioctl (int device, int request, void* argp);

#endif /* UART_MOD */

#endif /* UART_H_ */

/** @} */
/** @} */
