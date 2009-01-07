/************************************************************************************************
 * File:            errno.h
 * Description:     error constants
 ***********************************************************************************************/

#ifndef ERRNO_H_
# error "Never use <asm/errno.h> directly; include <errno.h> instead."
#endif

/*
 * Error constanst for read() write()
 */
#define EINTR       4
#define EIO         5
#define EBADF       9       //fd is not a valid file descriptor or is not open for reading.
#define EAGAIN      11
#define EFAULT      14
#define EINVAL      22
#define EFBIG       27      //write a file that exceeds the implementation-defined maximum file size or the process' file size limit 
#define ENOSPC      28
#define EPIPE       32

/*
 * Error constanst for open()
 */
#define ENXIO       6       //no such device exist
                             //open with read mode but file is write only
#define EACCES      13      //permission deny or file does not exist 
#define EROFS       30      //open with write mode but file is read only


