/**
 * \addtogroup posix POSIX
 * @{
 * 
 * Implementation of POSIX API using FreeRTOS API
 */

/**
 * \defgroup unistd System Call
 * @{
 * 
 * Implementation of System calls, interfacing between user applications and OS
 */

/**
 * \file
 * POSIX standard API for file stream example
 * \author Dennis Tsang <dennis@amonics.com>
 */

#include <define.h>
#include <unistd.h>
#include <stdio.h>

int fd_uart;   //File descriptor for uart (RS232)

void* 
tskComPort(void *ptr)
{
  static char uart_rx = 0x55;
  static char uart_tx[11];

  start_process();
    {
      //open com port for read and write
      fd_uart = open(UARTA, O_RDWR);
      
      while(1)
        {
          //read a character
          while(read(fd_uart, &uart_rx, 1) <= 0) usleep(0);
          //write string
          int number = sprintf(uart_tx, "%c%s%c", ++uart_rx, "@Amonics", 0x0d);
          while(write(fd_uart, uart_tx, number) != number) usleep(0);
        }
    }
  end_process();
}

/** @} */
/** @} */
