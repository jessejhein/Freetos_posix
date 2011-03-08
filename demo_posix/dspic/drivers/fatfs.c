/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of POSIX Wrapper for FatFs
 */

/**
 * \defgroup file_system File System
 * @{
 * Contains 
 * \li middle layer [fatfs_xxx()] interfacing posix open(), read(), write(), lseek()
 * \remarks
 * \li support only 1 volume
 * \li open upto 5 files at one time
 */

/**
 * \file
 * POSIX wrapper for FatFs
 * \author Dennis Tsang <dennis@amonics.com>
 */

#include <fcntl.h>
#include <stdio.h>


/**
 * \brief initialise file system, mount volume
 */
void
fatfs_init (void)
{
  return;
}


/**
 * \brief open a file in the specified drive
 * \param drive drive id
 * \param pathname pathname under the drive, begin with '\'
 * \param flag flag used to open the file
 * \return a usable file descriptor [0 - (FATFS_MAX_FILE - 1)]
 * \retval -1 error
 * \remarks
 * Limitations: For "a" and "a+", we do not consider writing operations append data at the end of the file.
 * \verbatim
    fopen    fatfs_open (open)                                       f_open
    r        => 0x4000 (O_NONBLOCK | O_RDONLY)                       => FA_READ | FA_OPEN_EXISTING (0x01)
    r+       => 0x4002 (O_NONBLOCK | O_RDWR)                         => FA_READ | FA_WRITE | FA_OPEN_EXISTING (0x03)
    w        => 0x4301 (O_NONBLOCK | O_CREAT | O_EXCL | O_WRONLY)    => FA_WRITE | FA_CREATE_ALWAYS (0x0a)
    w+       => 0x4302 (O_NONBLOCK | O_CREAT | O_EXCL | O_RDWR)      => FA_READ | FA_WRITE | FA_CREATE_ALWAYS (0x0b)
    a        => 0x4109 (O_NONBLOCK | O_CREAT | O_APPEND | O_WRONLY)  => FA_WRITE | FA_OPEN_ALWAYS (0x12)
    a+       => 0x410a (O_NONBLOCK | O_CREAT | O_APPEND | O_RDWR)    => FA_READ | FA_WRITE | FA_OPEN_ALWAYS (0x13)
   \endverbatim
 */
char
fatfs_open (int drive, const char *pathname, int flag)
{
  return -1;
}


/**
 * \brief close a file in the specified drive
 * \param drive drive id
 * \param fd file descriptor
 * \retval 0 success
 * \retval -1 error
 */
char
fatfs_close (int drive, int fd)
{
  return -1;
}


/**
 * \brief write count bytes from buffer
 * \param drive drive id
 * \param fd file descriptor
 * \param buf source buffer
 * \param count number of bytes to write
 * \return number of bytes written
 */
char
fatfs_write (int drive, int fd, char* buf, int count)
{
  return 0;
}


/**
 * \brief read count bytes to buffer
 * \param drive drive id
 * \param fd file descriptor
 * \param buf destination buffer
 * \param count number of bytes to read
 * \return number of bytes read
 */
char
fatfs_read (int drive, int fd, char* buf, int count)
{
  return 0;
}


/**
 * \brief reposition read/write file offset
 * \param drive drive id
 * \param fd file descriptor
 * \param offset offset bytes
 * \param whence reference point
 * \return new position
 */
char
fatfs_seek (int drive, int fd, int offset, int whence)
{
  //TODO: map to f_seek
  return 0;
}

/** @} */
/** @} */
