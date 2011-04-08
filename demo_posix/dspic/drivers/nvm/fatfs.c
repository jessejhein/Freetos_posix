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
#include <diskio.h>
#include <ff.h>


/** maximum supported volume */
#define FATFS_MAX_VOLUME                _VOLUMES

/** sector size */
#define FATFS_SECTOR_SIZE               _MAX_SS

/** maximum supported files per drive */
#define FATFS_MAX_FILE                  (FOPEN_MAX - 3)       // 8 - 3 = 5

/** work area (file system object) for logical drives */
static FATFS file_system[FATFS_MAX_VOLUME];

/** store the file info */
static FIL file[FATFS_MAX_VOLUME][FATFS_MAX_FILE];

/** points to file or NULL */
static FIL* file_ptr[FATFS_MAX_VOLUME][FATFS_MAX_FILE];


/**
 * \brief initialise file system, mount volume
 */
void
fatfs_init (void)
{
  int i, j;
  for (i = 0; i < FATFS_MAX_VOLUME; i++)
    {
      //mount logical drives
      f_mount (i, &file_system[i]);
      
      //initialise all file pointers to NULL (free to use)
      for (j = 0; i < FATFS_MAX_FILE; i++)
        {
          file_ptr[i][j] = NULL; 
        }
    }
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
  if (drive < FATFS_MAX_VOLUME)
    {
      //search for an available file pointer
      int i;
      for (i = 0; i < FATFS_MAX_FILE; i++)
        {
          if (file_ptr[drive][i] == NULL)
            {
              //setup flags
              BYTE mode = 0;
              //Read/Write
              if (flag & O_RDWR) mode |= (FA_READ | FA_WRITE);
              else if (flag & O_WRONLY) mode |= FA_WRITE;
              else mode |= FA_READ;
              //open/create
              if (flag & O_CREAT)
                {
                  //create new file
                  if (flag & O_EXCL) mode |= FA_CREATE_ALWAYS;
                  //open existing file, error if not exist
                  else if (flag & O_APPEND) mode |= FA_OPEN_ALWAYS;
                }
              else
                {
                  //open existing file, create if not exist
                  mode |= FA_OPEN_EXISTING;
                }

              //create file
              FRESULT result = f_open (&file[drive][i], pathname, mode);
              switch (result)
                {
                  case FR_OK:
                    {
                      //The function succeeded and the file object is valid.
                      file_ptr[drive][i] = &file[drive][i];
                      return i;
                    }
                  case FR_NO_FILE:
                    {
                      //Could not find the file.
                      break;
                    }
                  case FR_NO_PATH:
                    {
                      //Could not find the path.
                      break;
                    }
                  case FR_INVALID_NAME:
                    {
                      //The file name is invalid.
                      break;
                    }
                  case FR_INVALID_DRIVE:
                    {
                      //The drive number is invalid.
                      break;
                    }
                  case FR_EXIST:
                    {
                      //The file is already existing.
                      break;
                    }
                  case FR_DENIED:
                    {
                      //The required access was denied due to one of the following reasons:
                      //Write mode open against a read-only file.
                      //File cannot be created due to a directory or read-only file is existing.
                      //File cannot be created due to the directory table is full.
                      break;
                    }
                  case FR_NOT_READY:
                    {
                      //The disk drive cannot work due to no medium in the drive or any other reason.
                      break;
                    }
                  case FR_WRITE_PROTECTED:
                    {
                      //Write mode open or creation under the medium is write protected.
                      break;
                    }
                  case FR_DISK_ERR:
                    {
                      //The function failed due to an error in the disk function.
                      break;
                    }
                  case FR_INT_ERR:
                    {
                      //The function failed due to a wrong FAT structure or an internal error.
                      break;
                    }
                  case FR_NOT_ENABLED:
                    {
                      //The logical drive has no work area.
                      break;
                    }
                  case FR_NO_FILESYSTEM:
                    {
                      //There is no valid FAT volume on the drive.
                      break;
                    }
                  case FR_LOCKED:
                    {
                      //The function was rejected due to file shareing policy (_FS_SHARE).
                      break;
                    }

                }
              //When f_open returns values other than FR_OK
              return -1;
            }
        }
    }
  //All file pointer used OR drive > volume
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
  if ( (drive < FATFS_MAX_VOLUME) && (fd < FATFS_MAX_FILE) )
    {
      //perform close operation
      FRESULT result = f_close (file_ptr[drive][fd]);

      //check result
      switch (result)
        {
          case FR_OK:
            {
              //The file object has been closed successfully.
              //free up the file pointer for reuse
              file_ptr[drive][fd] = NULL;
              return 0;
            }
          case FR_DISK_ERR:
            {
              //The function failed due to an error in the disk function.
              break;
            }
          case FR_INT_ERR:
            {
              //The function failed due to a wrong FAT structure or an internal error.
              break;
            }
          case FR_NOT_READY:
            {
              //The disk drive cannot work due to no medium in the drive or any other reason.
              break;
            }
          case FR_INVALID_OBJECT:
            {
              //The file object is invalid.
              break;
            }
        }
    }
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
  if ( (drive < FATFS_MAX_VOLUME) && (fd < FATFS_MAX_FILE) )
    {
      //store the actual number of bytes read
      unsigned int byteWritten;
      
      //perform write operation
      FRESULT result = f_write (file_ptr[drive][fd], buf, count, &byteWritten);
      
      //check result
      switch (result)
        {
          case FR_OK:
            {
              break;
            }
          case FR_DENIED:
            {
              //The function denied due to the file has been opened in non-write mode
              break;
            }
          case FR_DISK_ERR:
            {
              //The function failed due to an error in the disk function.
              break;
            }
          case FR_INT_ERR:
            {
              //The function failed due to a wrong FAT structure or an internal error
              break;
            }
          case FR_NOT_READY:
            {
              //The disk drive cannot work due to no medium in the drive or any other reason
              break;
            }
          case FR_INVALID_OBJECT:
            {
              //The file object is invalid
              break;
            }
          default:
            {
            }
        }
      return byteWritten;
    }
  else
    {
      //since drive and/or fd exceed range, nothing can be write
      return 0;
    }
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
  if ( (drive < FATFS_MAX_VOLUME) && (fd < FATFS_MAX_FILE) )
    {
      //store the actual number of bytes read
      unsigned int byteRead;
      
      //perform read operation
      FRESULT result = f_read (file_ptr[drive][fd], buf, count, &byteRead);
      
      //check result
      switch (result)
        {
          case FR_OK:
            {
              break;
            }
          case FR_DENIED:
            {
              //The function denied due to the file has been opened in non-read mode
              break;
            }
          case FR_DISK_ERR:
            {
              //The function failed due to an error in the disk function.
              break;
            }
          case FR_INT_ERR:
            {
              //The function failed due to a wrong FAT structure or an internal error
              break;
            }
          case FR_NOT_READY:
            {
              //The disk drive cannot work due to no medium in the drive or any other reason
              break;
            }
          case FR_INVALID_OBJECT:
            {
              //The file object is invalid
              break;
            }
          default:
            {
            }
        }
      return byteRead;
    }
  else
    {
      //since drive and/or fd exceed range, nothing can be read
      return 0;
    }
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
