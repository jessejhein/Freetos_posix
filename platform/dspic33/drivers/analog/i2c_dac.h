/**
 * \addtogroup drivers Drivers
 * @{
 *
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup i2c_dac I2C DAC
 * @{
 *
 * Control I2C DAC DAC6574 (10-bit) / DAC7574 (12-bit)
 * \li The I2C module uses SCL and SDA, located at pin 36 and 37 respectively.
 * \li All I2C devices shares a common communication speed (default: 400kHz)
 * \li The driver has a POSIX-like interface with open(), read(), write(), ioctl()
 * \li When i2c has multiple devices, read(), write() cannot be used in ISR (Interrupt routine)
 */

/**
 * \file
 * I2C DAC Driver
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

#ifndef I2C_DAC_H_
#define I2C_DAC_H_

#ifdef I2C_DAC_MOD

/**
 * \brief Initialise I2C DAC
 * \param flags accessing mode
 * \retval 0 DAC opened
 */
extern int i2c_dac_open (int flags);


/**
 * \brief write 2 bytes to DAC
 * \param buf pointer of data to write to DAC
 * \return number of bytes written
 * \retval 0 no data has been written
 * \retval 2 two bytes has been written
 * \retval -1 not opened for writing error (errno = EBADF)
 *
 * \remarks
 * \li example DAC6574
 * \verbatim
    Mst/Slv    _______ M ___M___ M S ____M___ S ____M___ S ___M____ S M ________
    SDA (Data)        |S|       | |A|        |A|        |A|        |A|S|
                      |T|address|W|C|channelA|C| Data H |C| Data L |C|T|
                      |A|1001111|0|K|00010000|K|10101010|K|10XXXXXX|K|P|
   \endverbatim
 * \li example DAC7574
 * \verbatim
    Mst/Slv    _______ M ___M___ M S ____M___ S ____M___ S ___M____ S M ________
    SDA (Data)        |S|       | |A|        |A|        |A|        |A|S|
                      |T|address|W|C|channelA|C| Data H |C| Data L |C|T|
                      |A|1001111|0|K|00010000|K|10101010|K|1010XXXX|K|P|
   \endverbatim
 */
extern int i2c_dac_write (__u16* buf);


/**
 * \brief read 2 bytes from DAC
 * \param buf pointer of data to read from DAC
 * \return number of bytes read
 * \retval 0 no data has been read
 * \retval 2 two bytes has been read
 * \retval -1 not opened for reading error (errno = EBADF)
 *
 * \remarks
 * \li example DAC6574
 * \verbatim
    Mst/Slv    _______ M ___M___ M S ____M___ S M ___M___ M S ___S____ M ___S____ M M _____
    SDA (Data)        |S|       | |A|        |A|R|       | |A|        |A|        |N|S|
                      |T|address|W|C|channelA|C|E|address|R|C| Data H |C| Data L |A|T|
                      |A|1001111|0|K|00010010|K|S|1001111|1|K|10101010|K|10XXXXXX|K|P|
   \endverbatim
 * \li example DAC7574
 * \verbatim
    Mst/Slv    _______ M ___M___ M S ____M___ S M ___M___ M S ___S____ M ___S____ M M _____
    SDA (Data)        |S|       | |A|        |A|R|       | |A|        |A|        |N|S|
                      |T|address|W|C|channelA|C|E|address|R|C| Data H |C| Data L |A|T|
                      |A|1001111|0|K|00010010|K|S|1001111|1|K|10101010|K|1010XXXX|K|P|
   \endverbatim
 */
extern int i2c_dac_read (__u16* buf);


/**
 * \brief change setting for DAC
 * \param request Request code defined in ioctl.h
 * \param argp pointer for control configuration, request code dependent.
 * \retval 0 success
 * \retval -1 error
 */
extern int i2c_dac_ioctl (int request, unsigned char* argp);

#endif /* I2C_DAC_MOD */

#endif /* I2C_DAC_H_ */

/** @} */
/** @} */

