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

#include <define.h>
#include "cirbuf.h"

__u8
cirbuf_wr (__u8 wr, __u8 rd, __u8 max)
{
  //next position, if reach the end of buffer, loop back
  if (++wr == max) wr = 0;
  //Buffer is full
  return (wr == rd)? CIRBUF_WR_FULL : wr;
}

__u8
cirbuf_rd (__u8 wr, __u8 rd, __u8 max)
{
  //all data read
  if (rd == wr)
    {
      return CIRBUF_RD_EMPTY;
    }
  else
    {
      //next position, if reach the end of buffer, loop back
      if (++rd == max) rd = 0;
      return rd;
    }
}

