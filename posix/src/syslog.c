/**
 * \file
 * System Log File
 * \author Dennis Tsang <dennis@amonics.com>
 */


/*
 * Copyright (C) 2012  Dennis Tsang <dennis@amonics.com>
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
#include <stdio.h>
#include <sys/time.h>

#define LF_CHAR                 0x0a
#define CR_CHAR                 0x0d

/** file object for system log file */
static FILE* syslogFile;
/** prevent calling append in multi-thread environment, causing system to hang */
static __u8 busy;


//---------------------------------------------------------------------------
int
syslog_open (void)
{
  busy = 0;
  syslogFile = fopen (FS_ROOT"log.txt", "a");
  if (syslogFile == NULL) return 0;
  return 1;
}


//---------------------------------------------------------------------------
void
syslog_close (void)
{
  if (syslogFile == NULL) return;
  fclose (syslogFile);
}


//---------------------------------------------------------------------------
int
syslog_append (char* msg)
{
  if (syslogFile == NULL) return 0;
  if (busy) return 1;
  busy = 1;

  //check for timeout error
  struct timeval currentTime;
  gettimeofday (&currentTime, NULL);

  //add time-stamp and windows newline
  int i = fprintf (syslogFile, "[%10ld.%06ld] %s%c%c",
                                  currentTime.tv_sec, currentTime.tv_usec,
                                  msg,
                                  CR_CHAR, LF_CHAR);
  fclose (syslogFile);
  syslog_open ();
  busy = 0;
  return 0;
}
