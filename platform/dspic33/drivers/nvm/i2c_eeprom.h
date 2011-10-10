/**
 * \addtogroup drivers Drivers
 * @{
 *
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup i2c_eeprom I2C EEPROM
 * @{
 *
 * Control I2C EEPROM (24LC256) 32KB
 * \li The I2C module uses SCL and SDA, located at pin 36 and 37 respectively.
 * \li All I2C devices shares a common communication speed (default: 400kHz)
 * \li The driver has a POSIX-like interface with open(), read(), write(), ioctl()
 * \li When i2c has multiple devices, read(), write() cannot be used in ISR (Interrupt routine)
 */

/**
 * \file
 * I2C EEPROM Driver
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

#ifndef I2C_EEPROM_H_
#define I2C_EEPROM_H_

#ifdef NVM_I2C

/**
 * \brief open i2c EEPROM
 * \param flags accessing mode
 * \retval 0 EEPROM open
 * \remarks In NON-BLOCKING mode, application can only write upto 64 bytes at a time
 */
extern int i2c_eeprom_open (int flags);


/**
 * \brief write $count bytes to EEPROM
 * \param buf pointer of data to write to EEPROM
 * \param count number of bytes to be written
 * \return number of bytes written
 * \retval 0 i2c is busy (used by other device)
 * \retval -1 EEPROM is busy (communication problem) (errno = EAGAIN)
 * \n         EEPROM is not opened for reading (errno = EBADF)
 * \verbatim
    Mst/Slv    _______ M ___M___ M S ____M___ S ____M___ S ____M___ S ____M___ S M ________
    SDA (Data)        |S|       | |A|        |A|        |A|        |A|        |A|S|
                      |T|address|W|C|High Add|C|Low Add |C| Data 0 |C| Data 1 |C|T|
                      |A|1010000|0|K|XFFFFFFF|K|FFFFFFFF|K|10101010|K|10101010|K|P|
   \endverbatim
 */
extern int i2c_eeprom_write (__u8* buf, int count);


/**
 * \brief read count bytes from EEPROM
 * \param buf pointer of data to read from EEPROM
 * \param count number of bytes to be written
 * \return number of bytes read
 * \retval 0 i2c is busy (used by other device)
 * \retval -1 EEPROM is busy (communication problem) (errno = EAGAIN)
 * \n         EEPROM is not opened for reading (errno = EBADF)
 * \verbatim
    Mst/Slv    ___ M ___M___ M S ____M___ S ____M___ S M ___M___ M S ____S___ M ____S___ M M ____
    SDA (Data)    |S|       | |A|        |A|        |A|R|       | |A|        |A|        |N|S|
                  |T|address|W|C|High Add|C|Low Add |C|E|address|R|C| Data 0 |C| Data 1 |A|T|
                  |A|1010000|0|K|XFFFFFFF|K|FFFFFFFF|K|S|1010000|1|K|10101010|K|10101010|K|P|
   \endverbatim
 */
extern int i2c_eeprom_read (__u8* buf, int count);


/**
 * \brief change the current pointer from base address
 * \param offset offset from location determined by whence
 * \param whence
 * \n SEEK_SET: BASE (0) + offset
 * \n SEEK_CUR: unimplemented
 * \n SEEK_END: unimplemented
 * \return current offset from base address (can be greater than I2C_EEPROM_SIZE)
 * \retval -1 another read/write is in progress
 */
extern int i2c_eeprom_lseek (int offset, int whence);

#endif /* NVM_I2C */

#endif /* I2C_EEPROM_H_ */

/** @} */
/** @} */
