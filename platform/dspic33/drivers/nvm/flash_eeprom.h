/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup flash_eeprom Flash-emulated EEPROM
 * @{
 * 
 * Control Flash-emulated EEPROM
 * \li The driver has a POSIX-like interface with open(), read(), write(), ioctl()
 */

/**
 * \file
 * Flash-emulated EEPROM Driver
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

#ifndef FLASH_EEPROM_H_
#define FLASH_EEPROM_H_

#ifdef NVM_FLASH

/**
 * \brief open EEPROM
 * \param flags accessing mode
 * \retval 0 EEPROM open
 */
extern int flash_eeprom_open (int flags);


/**
 * \brief write $count bytes to EEPROM
 * \param buf pointer of data to write to EEPROM
 * \param count number of bytes to be written
 * \return number of bytes written
 * \retval -1 EEPROM is busy (errno = EAGAIN)
 * \n         EEPROM is not opened for reading (errno = EBADF)
 */
extern int flash_eeprom_write (__u8* buf, int count);


/**
 * \brief read count bytes from EEPROM
 * \param buf pointer of data to read from EEPROM
 * \param count number of bytes to be written
 * \return number of bytes read
 * \retval -1 EEPROM is busy (errno = EAGAIN)
 * \n         EEPROM is not opened for reading (errno = EBADF)
 */
extern int flash_eeprom_read (__u8* buf, int count);


/**
 * \brief change the current pointer from base address
 * \param offset offset from location determined by whence
 * \param whence
 * \n SEEK_SET: BASE (0) + offset
 * \n SEEK_CUR: unimplemented
 * \n SEEK_END: unimplemented
 * \return current offset from base address (can be greater than FLASH_EEPROM_SIZE)
 * \retval -1 another read/write is in progress
 */
extern int flash_eeprom_lseek (int offset, int whence);

#endif /* NVM_FLASH */

#endif /* FLASH_EEPROM_H_ */

/** @} */
/** @} */
