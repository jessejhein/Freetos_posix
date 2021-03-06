/**
 * \addtogroup drivers Drivers
 * @{
 *
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup i2c_temp I2C Temperature Sensor
 * @{
 *
 * Control I2C Temperature Sensor (TCN75A)
 * \li The I2C module uses SCL and SDA, located at pin 36 and 37 respectively.
 * \li All I2C devices shares a common communication speed (default: 400kHz)
 * \li The driver has a POSIX-like interface with open(), read(), write(), ioctl()
 * \li When i2c has multiple devices, read(), write() cannot be used in ISR (Interrupt routine)
 *
 * Capabilities
 * \li 12-bit resolution temperature sensor
 * \li Accuracy: +/-1oC (typical), +/-2oC (max)
 */

/**
 * \file
 * I2C Temperature Sensor Driver
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

#ifndef I2C_TEMP_H_
#define I2C_TEMP_H_

#ifdef I2C_TEMP_MOD

/**
 * \brief Initialise I2C Temperature Sensor
 * \param id channel ID
 * \param flags accessing mode
 * \retval 0 Temperature Sensor opened
 * \remarks configure resolution
 * \li example
 * \verbatim
    Mst/Slv    _______ M ___M___ M S ____M___ S ___M____ M __   __ M ___M___ M S ____M___ S
    SDA (Data)        |S|       | |A|        |A|        |S|       |S|       | |A|        |A|
                      |T|address|W|C| CONFIG |C| 12-bit |T|  ...  |T|address|W|C|  DATA  |C|  ---+
                      |A|1001001|0|K|00000001|K|01100000|P|       |A|1001001|0|K|00000000|K|     |
                                                                                                 |
                                                                                                 |
                       +-------------------------------------------------------------------------+
                       |
                      \|/

    Mst/Slv            M ___M___ M S ___S____ M ___S____ M M _____
    SDA (Data)        |S|       | |A|        |A|        |N|S|
                      |T|address|R|C| Data H |C| Data L |A|T|
                      |A|1001001|1|K|xxxxxxxx|K|xxxx0000|K|P|
   \endverbatim
 */
extern int i2c_temp_open (int id, int flags);


/**
 * \brief read 2 bytes from Temperature Sensor
 * \param id channel ID
 * \param buf pointer of data to read from Temperature Sensor
 * \return number of bytes read
 * \retval 0 no data has been read
 * \retval -1 not opened for reading error (errno = EBADF)
 *
 * \remarks
 * \li *buf should be interpreted as signed integer and multiplied by I2C_TEMP_CONVERT_FACTOR (2^-8) to get the floating point number
 * \li example
 * \verbatim
    Mst/Slv            M ___M___ M S ___S____ M ___S____ M M _____
    SDA (Data)        |S|       | |A|        |A|        |N|S|
                      |T|address|R|C| Data H |C| Data L |A|T|
    (Successive)      |A|1001001|1|K|xxxxxxxx|K|xxxx0000|K|P|
   \endverbatim
 */
extern int i2c_temp_read (int id, __u16* buf);


#endif /* I2C_TEMP_MOD */

#endif /* I2C_TEMP_H_ */

/** @} */
/** @} */

