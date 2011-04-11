/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup i2c_mod_master I2C MODULATION DSPIC (MASTER) 
 * @{
 * 
 * Control I2C MODULATION DSPIC (MASTER)
 * \li The I2C module uses SCL and SDA, located at pin 36 and 37 respectively.
 * \li All I2C devices shares a common communication speed (default: 400kHz) 
 * \li The driver has a POSIX-like interface with open(), read(), write(), ioctl()
 * \li When i2c has multiple devices, read(), write() cannot be used in ISR (Interrupt routine)
 */

/**
 * \file
 *  I2C MODULATION DSPIC (MASTER) Driver
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

#ifndef I2C_MOD_MASTER_DSPIC_H_
#define I2C_MOD_MASTER_DSPIC_H_

#ifdef I2C_MOD_MASTER_DSPIC

/**
 * \brief Initialise i2c modulation master
 * \param flags accessing mode
 * \retval 0 i2c modulation master opened
 */
extern int i2c_mod_master_open (int flags);


/**
 * \brief write 4 bytes to i2c modulation slave
 * \param buf pointer of data to write
 * \return number of bytes written
 * \retval 0 no data has been written
 * \retval 4 two bytes has been written
 * \retval -1 not opened for writing error (errno = EBADF)
 * 
 * \remarks
 * \li example
 * \verbatim
    Mst/Slv    _______ M ___M___ M S ____M___ S ____M___ S ___M____ S ___M____ S ___M ___ S M ____
    SDA (Data)        |S|       | |A|        |A|        |A|        |A|        |A|        |A|S|
                      |T|address|W|C|channelA|C| Data 0 |C| Data 1 |C| Data 2 |C| Data 3 |C|T|
                      |A|1011000|0|K|00010000|K|10101010|K|10101010|K|10101010|K|10101010|K|P|
   \endverbatim
 */
extern int i2c_mod_master_write (float *buf);


/**
 * \brief read 4 bytes from i2c modulation slave
 * \param buf pointer of data to read
 * \return number of bytes read
 * \retval 0 no data has been read
 * \retval 4 two bytes has been read
 * \retval -1 not opened for reading error (errno = EBADF)
 * 
 * \remarks
 * \li example
 * \verbatim
    Mst/Slv    _______ M ___M___ M S ____M___ S M ___M___ M S ___S____ M ___S____ M ___S____ M ___S____ M M _____ 
    SDA (Data)        |S|       | |A|        |A|R|       | |A|        |A|        |A|        |A|        |N|S|
                      |T|address|W|C|channelA|C|E|address|R|C| Data 0 |C| Data 1 |C| Data 2 |C| Data 3 |A|T|
                      |A|1011000|0|K|00010010|K|S|1011000|1|K|10101010|K|10101010|K|10101010|K|10101010|K|P|
   \endverbatim
 */
extern int i2c_mod_master_read (float *buf);


/**
 * \brief change setting for i2c modulation master
 * \param request Request code defined in ioctl.h
 * \param argp pointer for control configuration, request code dependent.
 * \retval 0 success
 * \retval -1 error
 */
extern int i2c_mod_master_ioctl (int request, unsigned char* argp);


#endif /* I2C_MOD_MASTER_DSPIC */

#endif /* I2C_MOD_MASTER_DSPIC_H_ */

/** @} */
/** @} */

