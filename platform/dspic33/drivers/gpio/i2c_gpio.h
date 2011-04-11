/**
 * \addtogroup drivers Drivers
 * @{
 *
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup i2c_gpio I2C GPIO
 * @{
 *
 * Control I2C GPIO MCP23017
 * \li The I2C module uses SCL and SDA, located at pin 36 and 37 respectively.
 * \li All I2C devices shares a common communication speed (default: 400kHz)
 * \li The driver has a POSIX-like interface with open(), read(), write(), ioctl()
 * \li When i2c has multiple devices, read(), write() cannot be used in ISR (Interrupt routine)
 */

/**
 * \file
 * I2C GPIO Driver
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

#ifndef I2C_GPIO_H_
#define I2C_GPIO_H_

#ifdef I2C_GPIO_MOD

/**
 * \brief Initialise I2C GPIO
 * \param flags accessing mode
 * \retval 0 GPIO opened
 */
extern int i2c_gpio_open (int flags);


/**
 * \brief write 1 byte to GPIO
 * \param buf pointer of data to write to GPIO
 * \return number of bytes written
 * \retval 0 no data has been written
 * \retval 1 one byte has been written
 * \retval -1 not opened for writing error (errno = EBADF)
 *
 * \remarks
 * \li example
 * \verbatim
    Mst/Slv    _______ M ___M___ M S ____M___ S ____M___ S M ________
    SDA (Data)        |S|       | |A|        |A|        |A|S|
                      |T|address|W|C|register|C|  data  |C|T|
                      |A|1000000|0|K|xxxxxxxx|K|xxxxxxxx|K|P|
   \endverbatim
 */
extern int i2c_gpio_write (unsigned char *buf);


/**
 * \brief read 1 byte from GPIO
 * \param buf pointer of data to read from GPIO
 * \return number of bytes read
 * \retval 0 no data has been read
 * \retval 1 one byte has been written
 * \retval -1 device is busy (communication problem) (errno = EAGAIN)
 * \n         device is not opened for reading (errno = EBADF)
 * \verbatim
    Mst/Slv    ___ M ___M___ M S ____M___ S M ___M___ M S ____S___ M M ____
    SDA (Data)    |S|       | |A|        |A|R|       | |A|        |N|S|
                  |T|address|W|C|register|C|E|address|R|C|  data  |A|T|
                  |A|1000000|0|K|xxxxxxxx|K|S|1000000|1|K|xxxxxxxx|K|P|
   \endverbatim
 */
extern int i2c_gpio_read (unsigned char *buf);


/**
 * \brief change setting for GPIO
 * \param request Request code defined in ioctl.h
 * \param argp pointer for control configuration, request code dependent.
 * \retval 0 success
 * \retval -1 error
 */
extern int i2c_gpio_ioctl (int request, unsigned char* argp);

#endif /* I2C_GPIO_MOD */

#endif /* I2C_GPIO_H_ */

/** @} */
/** @} */

