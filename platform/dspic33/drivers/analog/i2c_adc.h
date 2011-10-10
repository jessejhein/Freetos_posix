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
 *
 * Capabilities
 * \li At most 4 ADC data (8 bytes) can be read per operation.
 */

/**
 * \file
 * I2C ADC Driver
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
extern int i2c_adc_read (__u16* buf, int count);

#endif /* I2C_ADC_MOD */

#endif /* I2C_ADC_H_ */

/** @} */
/** @} */

