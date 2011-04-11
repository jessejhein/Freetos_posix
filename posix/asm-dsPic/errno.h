/**
 * \file
 * Error Numbers
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

#ifndef ERRNO_H_
# error "Never use <asm/errno.h> directly; include <errno.h> instead."
#endif /* ERRNO_H_ */

/*
 * Error constants for read() write()
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


