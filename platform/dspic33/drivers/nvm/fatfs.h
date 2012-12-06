/**
 * \addtogroup drivers Drivers
 * @{
 *
 * Implementation of POSIX Wrapper for FatFs
 */

/**
 * \defgroup file_system File System
 * @{
 * \section file_system_sec1 INTRODUCTION
 * \li middle layer [fatfs_xxx()] interfacing posix open(), read(), write(), lseek()
 *
 * \section file_system_sec2 COMPILE TIME CONFIGURATION
 * \subsection file_system_sec2_1  In <.config_freertos_posix>, set:
 * \verbatim
    CONFIG_FILE_SYSTEM          =       y
   \endverbatim
 * \subsection file_system_sec2_2 In <define.h>, define:
 * \verbatim
    #define FS_ROOT                     "0:"
   \endverbatim
 * \subsection file_system_sec2_3 In <pin_define.c>, implement:
 * \verbatim
    void mmc_card_detect_pin_init (void)
    int mmc_card_detect (void)
   \endverbatim
 *
 * \section file_system_sec3 LIMITATIONS
 * \li support only 1 volume
 * \li open upto 5 files at one time
 */


/**
 * \file
 * POSIX wrapper for FatFs
 * \author Dennis Tsang <dennis@amonics.com>
 */


/*
 * Copyright (C) 2011  Dennis Tsang <dennis@amonics.com>
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

#ifndef FATFS_H_
#define FATFS_H_

/**
 * \brief initialise file system, mount volume
 */
extern void fatfs_init (void);


/**
 * \brief open a file in the specified drive
 * \param drive drive id
 * \param pathname pathname under the drive, begin with '\'
 * \param flag flag used to open the file
 * \return a usable file descriptor [0 - (FATFS_MAX_FILE - 1)]
 * \retval -1 error
 */
extern char fatfs_open (int drive, const char *pathname, int flag);


/**
 * \brief close a file in the specified drive
 * \param drive drive id
 * \param fd file descriptor
 * \retval 0 success
 * \retval -1 error
 */
extern char fatfs_close (int drive, int fd);


/**
 * \brief write count bytes from buffer
 * \param drive drive id
 * \param fd file descriptor
 * \param buf source buffer
 * \param count number of bytes to write
 * \return number of bytes written
 */
extern int fatfs_write (int drive, int fd, char* buf, int count);


/**
 * \brief read count bytes to buffer
 * \param drive drive id
 * \param fd file descriptor
 * \param buf destination buffer
 * \param count number of bytes to read
 * \return number of bytes read
 * \remarks count is always 512 by stdio.h
 * \remarks stdio.h will always return number of byte read = 1, independent of value of byteRead
 */
extern int fatfs_read (int drive, int fd, char* buf, int count);


/**
 * \brief reposition read/write file offset
 * \param drive drive id
 * \param fd file descriptor
 * \param offset offset bytes
 * \param whence reference point
 * \return new position
 */
extern int fatfs_seek (int drive, int fd, int offset, int whence);


#endif /* FATFS_H_ */

/** @} */
/** @} */
