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

/**
 * \brief Determine the next valid position to write to the buffer
 * \param wr
 * \param rd
 * \param max
 * \retval <254 next position of buffer
 * \retval 255 buffer is full
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
extern unsigned char pre_wr_cir254buf (unsigned char wr, unsigned char rd, unsigned char max);

/**
 * \brief Determine the next valid position to read from the buffer
 * \param wr
 * \param rd
 * \param max
 * \retval <254 next position of buffer
 * \retval 255 no data
 */
extern unsigned char pre_rd_cir254buf (unsigned char wr, unsigned char rd, unsigned char max);

#endif /* CIRBUF_H_ */
