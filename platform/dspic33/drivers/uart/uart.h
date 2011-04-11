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

/*
 * Copyright (C) 2007-2011  Dennis Tsang <dennis@amonics.com>
 *
 * This file is part of freertos_posix.
 *
 * freertos_posix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * freertos_posix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with freertos_posix.  If not, see <http://www.gnu.org/licenses/>
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
