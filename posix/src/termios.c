/**
 * \file
 * Interface between POSIX thread, MUTEX and FreeRTOS task, semaphore
 * \author Dennis Tsang <dennis@amonics.com>
 * \author Thai
 */

#include <termios.h>
#include <sys/ioctl.h>

int
cfsetospeed (struct termios* termios_p, speed_t speed)
{  
  switch (speed)
    {
      case B0:
        { 
          termios_p->c_ospeed = 0;
          break;
        }
      case B600:
        { 
          termios_p->c_ospeed = 600;
          break;
        }
      case B1200:
        { 
          termios_p->c_ospeed = 1200;
          break;
        }
      case B2400:
        { 
          termios_p->c_ospeed = 2400;
          break;
        }
      case B4800:
        { 
          termios_p->c_ospeed = 4800;
          break;
        }
      case B9600:
        { 
          termios_p->c_ospeed = 9600;
          break;
        }
      case B19200:
        { 
          termios_p->c_ospeed = 19200;
          break;
        }
      case B38400:
        { 
          termios_p->c_ospeed = 38400;
          break;
        }
      case B57600:
        { 
          termios_p->c_ospeed = 57690;
          break;
        }
      case B115200:
        { 
          termios_p->c_ospeed = 115200;
          break;
        }
      case B230400:
        { 
          termios_p->c_ospeed = 230400;
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
      case B0:
        { 
          termios_p->c_ispeed = 0;
          break;
        }
      case B600:
        { 
          termios_p->c_ispeed = 600;
          break;
        }
      case B1200:
        { 
          termios_p->c_ispeed = 1200;
          break;
        }
      case B2400:
        { 
          termios_p->c_ispeed = 2400;
          break;
        }
      case B4800:
        { 
          termios_p->c_ispeed = 4800;
          break;
        }
      case B9600:
        { 
          termios_p->c_ispeed = 9600;                 
          break;
        }
      case B19200:
        { 
          termios_p->c_ispeed = 19200;
          break;
        }
      case B38400:
        { 
          termios_p->c_ispeed = 38400;
          break;
        }
      case B57600:
        { 
          termios_p->c_ispeed = 57690;
          break;
        }
      case B115200:
        { 
          termios_p->c_ispeed = 115200;
          break;
        }
      case B230400:
        { 
          termios_p->c_ispeed = 230400;
          break;
        }
      default:
        return -1;
    }
  return 0;
}


int
cfsetspeed (struct termios* termios_p, speed_t speed)
{
  if (cfsetospeed (termios_p, speed) < 0) return -1;
  if (cfsetispeed (termios_p, speed) < 0) return -1;
  return 0;
}


int
tcgetattr (int fildes, struct termios *termios_p)
{
  __u32 speed;
  int retv = ioctl (fildes, UART_GET_BAUDRATE, &speed);
  termios_p->c_ispeed = speed;
  termios_p->c_ospeed = speed;
  return retv;  
}


int
tcsetattr (int fildes, int optional_actions, const struct termios *termios_p)
{  
  __u32 speed;
  // speed have error  
  if ( (termios_p->c_ispeed < 0) || (termios_p->c_ospeed < 0) || (termios_p->c_ispeed != termios_p->c_ospeed) )
    return -1;
  speed = termios_p->c_ispeed;   
  // wait until finished receiver and transmitter
  close (fildes);
  return ioctl (fildes, UART_SET_BAUDRATE, &speed);
}
