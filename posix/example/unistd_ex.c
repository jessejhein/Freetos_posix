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
int fd_adc;

void* 
tskComPort(void *ptr)
{
  static char uart_rx = 0x55;
  static char uart_tx[11];

  start_process();
    {
      //open com port for read and write
      fd_uart = open(UARTA, O_RDWR);
      //open adc port for read only and non-blocking mode
      fd_adc = open(ADC, O_RDONLY | O_NONBLOCK);
      
      while(1)
        {
          //read a character
          while(read(fd_uart, &uart_rx, 1) <= 0) usleep(0);

          //read adc value from AN15
          unsigned char ctrl_buf = 15;
          ioctl(fd_adc, ADC_ADD_CH, &ctrl_buf);
          int raw_adc;
          while(read(fd_adc, &adc_raw, sizeof(adc_raw) ) != sizeof(adc_raw)) usleep(0);
          
          //write the adc value
          int len = sprintf(uart_tx, "%d%c", ++uart_rx, adc_raw, 0x0d);
          while(write(fd_uart, uart_tx, len) != len) usleep(0);
        }
    }
  end_process();
}

/** @} */
/** @} */
