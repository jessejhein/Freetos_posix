/**
 * \file
 * Interface between POSIX thread, MUTEX and FreeRTOS task, semaphore
 * \author Dennis Tsang <dennis@amonics.com>
 * \author Thai
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

#include <termios.h>
#include <sys/ioctl.h>


int
cfsetospeed (struct termios* termios_p, speed_t speed)
{  
  switch (speed)
    {
      case B9600:
      case B19200:
      case B38400:
      case B57600:
      case B115200:
      case B230400:
        { 
          termios_p->c_cflag &= 0xFFE0;
          termios_p->c_cflag |= speed;
          break;
        }
      default:
        return -1;
    }
  return 0;
}


int
cfsetispeed (struct termios* termios_p, speed_t speed)
{
  switch (speed)
    {
      case B9600:
      case B19200:
      case B38400:
      case B57600:
      case B115200:
      case B230400:
        { 
          termios_p->c_cflag &= 0xFFE0;
          termios_p->c_cflag |= speed;
          break;
        }
      default:
        return -1;
    }
  return 0;
}


int
tcgetattr (int fildes, struct termios *termios_p)
{
  return ioctl (fildes, UART_GET_OPTION, termios_p);
}


int
tcsetattr (int fildes, int optional_actions, const struct termios *termios_p)
{
  // wait until finished receiver and transmitter
  close (fildes);
  return ioctl (fildes, UART_SET_OPTION, termios_p);
}
