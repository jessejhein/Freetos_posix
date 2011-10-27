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

#ifndef _TERMIOS_H
# error "Never use <bits/termios.h> directly; include <termios.h> instead."
#endif

typedef __u16   tcflag_t;
typedef __u16   speed_t;


/** speed of serial port */
struct termios
{
  tcflag_t c_cflag;             /* control mode flags */
  //TODO: if these flags are used, remove from union
  union
    {
      tcflag_t c_oflag;         /* output mode flags */
      tcflag_t c_lflag;         /* local mode flags */
      tcflag_t c_iflag;         /* input mode flags */
    };
};


/* c_cflag bit meaning */
#define B9600                   0x0001
#define B19200                  0x0002
#define B38400                  0x0004
#define B57600                  0x0006
#define B115200                 0x000C
#define B230400                 0x0018
#define CSIZE                   0x0060  /* Bit mask for data bits */
#define   CS5                   0x0000
#define   CS6                   0x0020
#define   CS7                   0x0040
#define   CS8                   0x0060
#define CSTOPB                  0x0100  /* 2 stop bits (1 otherwise) */
#define CREAD                   0x0200  /* Enable receiver */
#define PARENB                  0x0400  /* Enable parity bit */
#define PARODD                  0x1000  /* Use odd parity instead of even */
#define HUPCL                   0x2000
#define CLOCAL                  0x4000  /* Local line - do not change "owner" of port */
#define CRTSCTS                 0x8000  /* flow control */

/* c_iflag bits */
#define PARMRK                  0001
#define INPCK                   0002    /* Enable parity check */
#define ISTRIP                  0004    /* Strip parity bits */
#define IUCLC                   0010
#define IXON                    0020    /* Enable software flow control (outgoing) */
#define IXANY                   0040    /* Enable software flow control (incoming) */
#define IXOFF                   0100    /* Allow any character to start flow again */
#define IMAXBEL                 0200
#define IUTF8                   0400


/* c_oflag bits */
#define OPOST                   0001    /* Postprocess output (not set = raw output) */
#define OLCUC                   0002
#define ONLCR                   0004
#define OCRNL                   0010
#define ONOCR                   0020
#define ONLRET                  0040
#define OFILL                   0100
#define OFDEL                   0200


/* c_lflag bits */
#define ISIG                    0001    /* Enable SIGINTR, SIGSUSP, SIGDSUSP, and SIGQUIT signals */
#define ICANON                  0002    /* Enable canonical input (else raw) */
#define ECHO                    0010    /* Enable echoing of input characters */
#define ECHOE                   0020    /* Echo erase character as BS-SP-BS */
#define ECHOK                   0040
#define ECHONL                  0100
#define NOFLSH                  0200
#define TOSTOP                  0400


/* tcsetattr uses these */
#define TCSANOW                 0       /* Make changes now without waiting for data to complete */
#define TCSADRAIN               1       /* Wait until everything has been transmitted */
#define TCSAFLUSH               2       /* Flush input and output buffers and make the change */
