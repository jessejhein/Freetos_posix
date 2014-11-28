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

#include <fcntl.h>
#include <stdio.h>
#include <diskio.h>
#include <ff.h>

//#define FATFS_DEBUG                     1

/** maximum supported volume */
#define FATFS_MAX_VOLUME                _VOLUMES

/** sector size */
#define FATFS_SECTOR_SIZE               _MAX_SS

/** maximum supported files per drive */
#ifndef FOPEN_MAX
  #define FOPEN_MAX                     8
#endif
#define FATFS_MAX_FILE                  (FOPEN_MAX - 3)       // 8 - 3 = 5

/** work area (file system object) for logical drives */
static FATFS file_system[FATFS_MAX_VOLUME];

/** store the file info */
static FIL file[FATFS_MAX_VOLUME][FATFS_MAX_FILE];

/** points to file or NULL */
static FIL* file_ptr[FATFS_MAX_VOLUME][FATFS_MAX_FILE];


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
 * \remarks
 * Limitations: open up to 5 files only (FOPEN_MAX defined as 8 for stdio.h, 3 file reserved)
 * \verbatim
                                                                                                                          Capability
    fopen    fatfs_open (open)                                       f_open                                               Create | Read  | Write | End
    r        => 0x4000 (O_NONBLOCK | O_RDONLY)                       => FA_READ | FA_OPEN_EXISTING (0x01)                    X   |   X   |   X   |  X
    r+       => 0x4002 (O_NONBLOCK | O_RDWR)                         => FA_READ | FA_WRITE | FA_OPEN_EXISTING (0x03)         X   |   O   |   O   |  X
    w        => 0x4301 (O_NONBLOCK | O_CREAT | O_EXCL | O_WRONLY)    => FA_WRITE | FA_CREATE_ALWAYS (0x0a)                   O   |   X   |   O   |  X
    w+       => 0x4302 (O_NONBLOCK | O_CREAT | O_EXCL | O_RDWR)      => FA_READ | FA_WRITE | FA_CREATE_ALWAYS (0x0b)         O   |   O   |   O   |  X
    a        => 0x4109 (O_NONBLOCK | O_CREAT | O_APPEND | O_WRONLY)  => FA_WRITE | FA_OPEN_ALWAYS (0x12)                     O   |   X   |   O   |  O
    a+       => 0x410a (O_NONBLOCK | O_CREAT | O_APPEND | O_RDWR)    => FA_READ | FA_WRITE | FA_OPEN_ALWAYS (0x13)           O   |   O   |   O   |  O
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
              //read/write
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

                      //Append mode: move to end of file
                      if (flag & O_APPEND)
                        {
                          //assume always successful
                          f_lseek (file_ptr[drive][i], f_size(file_ptr[drive][i]));
                        }
                      return i;
                    }
#ifdef FATFS_DEBUG
                  case FR_NO_FILE:
                    {
                      printf ("[OPEN ERR]: Could not find the file.\n");
                      break;
                    }
                  case FR_NO_PATH:
                    {
                      printf ("[OPEN ERR]: Could not find the path.\n");
                      break;
                    }
                  case FR_INVALID_NAME:
                    {
                      printf ("[OPEN ERR]: The given string is invalid as the path name.\n");
                      break;
                    }
                  case FR_INVALID_DRIVE:
                    {
                      printf ("[OPEN ERR]: Invalid drive number is specified.\n");
                      break;
                    }
                  case FR_EXIST:
                    {
                      printf ("[OPEN ERR]: Any object that has the same name is already existing.\n");
                      break;
                    }
                  case FR_DENIED:
                    {
                      //The required access was denied due to one of the following reasons:
                      //  Write mode open against the file with read-only attribute.
                      //  Deleting the file or directory with read-only attribute.
                      //  Deleting the non-empty directory or current directory.
                      //  Reading the file opened without FA_READ flag.
                      //  Any modification to the file opened without FA_WRITE flag.
                      //  Could not create the file or directory due to the directory table is full.
                      //  Could not create the directory due to the volume is full.
                      printf ("[OPEN ERR]: The required access was denied.\n");
                      break;
                    }
                  case FR_NOT_READY:
                    {
                      printf ("[OPEN ERR]: The disk drive cannot work due to incorrect medium removal or disk_initialize function failed.\n");
                      break;
                    }
                  case FR_WRITE_PROTECTED:
                    {
                      printf ("[OPEN ERR]: Any write mode action against write-protected media.\n");
                      break;
                    }
                  case FR_DISK_ERR:
                    {
                      printf ("[OPEN ERR]: An unrecoverable error occured in the lower layer (disk I/O functions).\n");
                      break;
                    }
                  case FR_INT_ERR:
                    {
                      //One of the following possibilities are suspected.
                      //  There is any error of the FAT structure on the volume.
                      //  Work area (file system object, file object or etc...) is broken by stack overflow or any other application. This is the reason in most case.
                      //  An FR_DISK_ERR has occurred on the file object.
                      printf ("[OPEN ERR]: Assertion failed. An insanity is detected in the internal process.\n");
                      break;
                    }
                  case FR_NOT_ENABLED:
                    {
                      printf ("[OPEN ERR]: Work area for the logical drive has not been registered by f_mount function.\n");
                      break;
                    }
                  case FR_NO_FILESYSTEM:
                    {
                      printf ("[OPEN ERR]: There is no valid FAT volume on the drive.\n");
                      break;
                    }
                  case FR_LOCKED:
                    {
                      printf ("[OPEN ERR]: The file access is rejected by file sharing control.\n");
                      break;
                    }
                  case FR_TIMEOUT:
                    {
                      printf ("[OPEN ERR]: The function cancelled due to a timeout of thread-safe control.\n");
                      break;
                    }
                  case FR_NOT_ENOUGH_CORE:
                    {
                      //There is one of the following reasons:
                      //  Could not allocate a memory for LFN working buffer. (Related option: _USE_LFN)
                      //  Given table size is insufficient for the required size.
                      printf ("[OPEN ERR]: Not enough memory for the operation.\n");
                      break;
                    }
                  case FR_TOO_MANY_OPEN_FILES:
                    {
                      printf ("[OPEN ERR]: Number of open files has been reached maximum value and no more file can be opened.\n");
                      break;
                    }
                  default:
                    {
                      printf ("[OPEN ERR]: Unknown\n");
                      break;
                    }
#endif /* FATFS_DEBUG */
                }
              //When f_open returns values other than FR_OK
              return -1;
            }
        }
    }
  //All file pointer used OR drive > volume
  return -1;
}


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
#ifdef FATFS_DEBUG
          case FR_DISK_ERR:
            {
              printf ("[CLOSE ERR]: An unrecoverable error occurred in the lower layer (disk I/O functions).\n");
              break;
            }
          case FR_INT_ERR:
            {
              //One of the following possibilities are suspected.
              //  There is any error of the FAT structure on the volume.
              //  Work area (file system object, file object or etc...) is broken by stack overflow or any other application. This is the reason in most case.
              //  An FR_DISK_ERR has occurred on the file object.
              printf ("[CLOSE ERR]: Assertion failed. An insanity is detected in the internal process.\n");
              break;
            }
          case FR_NOT_READY:
            {
              printf ("[CLOSE ERR]: The disk drive cannot work due to incorrect medium removal or disk_initialize function failed.\n");
              break;
            }
          case FR_INVALID_OBJECT:
            {
              printf ("[CLOSE ERR]: The given file or directory object structure is invalid.\n");
              break;
            }
          case FR_TIMEOUT:
            {
              printf ("[CLOSE ERR]: The function cancelled due to a timeout of thread-safe control.\n");
              break;
            }
          default:
            {
              printf ("[CLOSE ERR]: Unknown\n");
              break;
            }
#endif /* FATFS_DEBUG */
        }
    }
  return -1;
}


int
fatfs_write (int drive, int fd, char* buf, int count)
{
  if ( (drive < FATFS_MAX_VOLUME) && (fd < FATFS_MAX_FILE) )
    {
      //store the actual number of bytes written
      UINT byteWritten;

      //perform write operation
      FRESULT result = f_write (file_ptr[drive][fd], buf, count, &byteWritten);

#ifdef FATFS_DEBUG
      //check result
      switch (result)
        {
          case FR_OK:
            {
              break;
            }
          case FR_DISK_ERR:
            {
              printf ("[WRITE ERR]: An unrecoverable error occurred in the lower layer (disk I/O functions).\n");
              break;
            }
          case FR_INT_ERR:
            {
              //One of the following possibilities are suspected.
              //  There is any error of the FAT structure on the volume.
              //  Work area (file system object, file object or etc...) is broken by stack overflow or any other application. This is the reason in most case.
              //  An FR_DISK_ERR has occurred on the file object.
              printf ("[WRITE ERR]: Assertion failed. An insanity is detected in the internal process.\n");
              break;
            }
          case FR_NOT_READY:
            {
              printf ("[WRITE ERR]: The disk drive cannot work due to incorrect medium removal or disk_initialize function failed.\n");
              break;
            }
          case FR_INVALID_OBJECT:
            {
              printf ("[WRITE ERR]: The given file or directory object structure is invalid.\n");
              break;
            }
          case FR_TIMEOUT:
            {
              printf ("[WRITE ERR]: The function cancelled due to a timeout of thread-safe control.\n");
              break;
            }
          default:
            {
              printf ("[WRITE ERR]: Unknown\n");
              break;
            }
        }
#endif /* FATFS_DEBUG */
      return byteWritten;
    }
  else
    {
      //since drive and/or fd exceed range, nothing can be write
      return 0;
    }
}


int
fatfs_read (int drive, int fd, char* buf, int count)
{
  if ( (drive < FATFS_MAX_VOLUME) && (fd < FATFS_MAX_FILE) )
    {
      //store the actual number of bytes read
      UINT byteRead;
      
      //perform read operation
      FRESULT result = f_read (file_ptr[drive][fd], buf, count, &byteRead);
      
#ifdef FATFS_DEBUG
      //check result
      switch (result)
        {
          case FR_OK:
            {
              break;
            }
          case FR_DISK_ERR:
            {
              printf ("[READ ERR]: An unrecoverable error occurred in the lower layer (disk I/O functions).\n");
              break;
            }
          case FR_INT_ERR:
            {
              //One of the following possibilities are suspected.
              //  There is any error of the FAT structure on the volume.
              //  Work area (file system object, file object or etc...) is broken by stack overflow or any other application. This is the reason in most case.
              //  An FR_DISK_ERR has occurred on the file object.
              printf ("[READ ERR]: Assertion failed. An insanity is detected in the internal process.\n");
              break;
            }
          case FR_NOT_READY:
            {
              printf ("[READ ERR]: The disk drive cannot work due to incorrect medium removal or disk_initialize function failed.\n");
              break;
            }
          case FR_INVALID_OBJECT:
            {
              printf ("[READ ERR]: The given file or directory object structure is invalid.\n");
              break;
            }
          case FR_TIMEOUT:
            {
              printf ("[READ ERR]: The function cancelled due to a timeout of thread-safe control.\n");
              break;
            }
          default:
            {
              printf ("[READ ERR]: Unknown\n");
              break;
            }
        }
#endif /* FATFS_DEBUG */
      return byteRead;
    }
  else
    {
      //since drive and/or fd exceed range, nothing can be read
      return 0;
    }
}


/**
 * \remarks There is a bug in stdio.h which swap the value of offset and whence
 * \remarks lseek() in stdio.h will always return 0 irrespective of error
 */
int
fatfs_seek (int drive, int fd, int offset, int whence)
{
  if ( (drive < FATFS_MAX_VOLUME) && (fd < FATFS_MAX_FILE) )
    {
      //swap offset with whence (due to bug in stdio.h)
      int tmp = offset;
      offset = whence;
      whence = tmp;

      //perform seek operation
      DWORD location;
      //SEEK_SET: Beginning of file
      if (whence == 0)
        {
          location = offset;
        }
      //SEEK_CUR: Current position of the file pointer
      else if (whence == 1)
        {
          location = f_tell (file_ptr[drive][fd]) + offset;
        }
      //SEEK_END: End of file
      else
        {
           location = f_size(file_ptr[drive][fd]) + offset;
        }

      FRESULT result = f_lseek (file_ptr[drive][fd], location);

      //check result
      switch (result)
        {
          case FR_OK:
            {
              return offset;
            }
#ifdef FATFS_DEBUG
          case FR_DISK_ERR:
            {
              printf ("[SEEK ERR]: An unrecoverable error occurred in the lower layer (disk I/O functions).\n");
              break;
            }
          case FR_INT_ERR:
            {
              //One of the following possibilities are suspected.
              //  There is any error of the FAT structure on the volume.
              //  Work area (file system object, file object or etc...) is broken by stack overflow or any other application. This is the reason in most case.
              //  An FR_DISK_ERR has occurred on the file object.
              printf ("[SEEK ERR]: Assertion failed. An insanity is detected in the internal process.\n");
              break;
            }
          case FR_NOT_READY:
            {
              printf ("[SEEK ERR]: The disk drive cannot work due to incorrect medium removal or disk_initialize function failed.\n");
              break;
            }
          case FR_INVALID_OBJECT:
            {
              printf ("[SEEK ERR]: The given file or directory object structure is invalid.\n");
              break;
            }
          case FR_TIMEOUT:
            {
              printf ("[SEEK ERR]: The function cancelled due to a timeout of thread-safe control.\n");
              break;
            }
          default:
            {
              printf ("[SEEK ERR]: Unknown\n");
              break;
            }
#endif /* FATFS_DEBUG */
        }
      return -1;
    }
}

#ifdef FATFS_DEBUG
unsigned char streambuf[512];
void
fatfs_test (void)
{
  while (syslog_append ("fatfs_test started"));
  printf ("running fatfs_test\n");
  while (syslog_append ("fatfs_test ended"));
  return;

  FILE *TestWrFile2;
  FILE *TestWrFile;
  FILE *TestRdFile;

  // Test read file
  char RdStr[70];
  TestRdFile = fopen ("0:amonics/to_do.txt", "r+");
  if (TestRdFile)
    {
      fseek (TestRdFile, 9, SEEK_SET);
      char charRead = fgetc (TestRdFile);
      //printf ("char. read: %c\n", charRead);

      int charRd;
      charRd = fread (RdStr, sizeof(char), 30, TestRdFile);
      RdStr[charRd] = 0;
      fclose (TestRdFile);
      //printf ("%d chars read: <%s>\n", charRd, RdStr);
    }

  // Test write file
  TestWrFile = fopen ("0:amonics/fs.txt", "w+");
  if (TestWrFile)
    {
      //setvbuf(TestWrFile, NULL, _IONBF, 0);
      setvbuf(TestWrFile, streambuf, _IOFBF, 125);

      int charWr;
      //fatfs_write (0, 0, (char *)WrStr, 60);
      //charWr = fwrite ("0123456789abcdefghij   Test Log File contains 60 characters.", sizeof(char), 60, TestWrFile);
      //charWr = fprintf (TestWrFile, "%s", RdStr);
      charWr = fprintf (TestWrFile, "write the file for the 1st time");

      fclose (TestWrFile);
      //printf ("%d char written\n", charWr);
    }

  //TestWrFile2 = fopen ("0:amonics/fs2.txt", "w+");
  TestWrFile2 = fopen ("0:amonics/fs.txt", "w+");
  if (TestWrFile2)
    {
      //setvbuf(TestWrFile2, NULL, _IONBF, 0);
      setvbuf(TestWrFile2, streambuf, _IOFBF, 125);

      fprintf (TestWrFile2, "write the file for the 2nd time");

      fclose (TestWrFile2);
    }
}
#endif /* FATFS_DEBUG */
