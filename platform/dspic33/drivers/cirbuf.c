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

unsigned char
pre_wr_cir254buf (unsigned char wr, unsigned char rd, unsigned char max)
{
  unsigned char wr_backup = wr + 1;     //normal position of next char to write
  if (wr_backup == max) wr_backup = 0;  //if reach the end of buffer, loop back
  if (wr_backup == rd) return 255;      //Buffer is full
  else return wr_backup;
}

unsigned char
pre_rd_cir254buf (unsigned char wr, unsigned char rd, unsigned char max)
{
  unsigned char rd_backup;
  if (rd == wr) return 255;                     //all data read
  else
    {
      rd_backup = rd + 1;                       //normal position of next char to read
      if (rd_backup == max) rd_backup = 0;      //if reach the end of buffer, loop back
      return rd_backup;
    }
}

