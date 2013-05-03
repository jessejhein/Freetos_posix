/**
 * \file
 * Implementation of circular buffer
 * \author Cheng Chung Yan <yan@amonics.com>
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

#ifndef CIRBUF_H_
#define CIRBUF_H_


/** indicate write buffer is full */
#define CIRBUF_WR_FULL                  0xFF
/** indicate read buffer is empty */
#define CIRBUF_RD_EMPTY                 0xFF


/**
 * \brief Determine the next valid position to write to the buffer
 * \param wr current write position
 * \param rd current read position
 * \param max maximum size of buffer
 * \retval CIRBUF_WR_FULL
 * \retval next position of buffer otherwise
 * \remarks
 * \verbatim
   Design Concept
     Make a circular buffer template.
     this is only a template, not for real code to work, please implement this code into
     your application area.

               | wr ptr
    top        V                              bottom
     |<------------------------------------>| buffer                 wr and rd ptr, they both
                             ^                                       wr ptr must go prior rd         must go upward, then circular
                             | rd ptr
   \endverbatim
 */
extern __u8 cirbuf_wr (__u8 wr, __u8 rd, __u8 max);


/**
 * \brief Determine the next valid position to read from the buffer
 * \param wr current write position
 * \param rd current read position
 * \param max maximum size of buffer
 * \retval CIRBUF_RD_EMPTY
 * \retval next position of buffer otherwise
 */
extern __u8 cirbuf_rd (__u8 wr, __u8 rd, __u8 max);


#endif /* CIRBUF_H_ */
