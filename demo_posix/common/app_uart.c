/************************************************************************************************
 * File: 			app_uart.c
 * Description:		uart task employing posix thread and file API
 ***********************************************************************************************
 * SUMMARY:
 * 	1) Use UARTA (RS232) at default baud rate (19200bps). 
 * 	2) Check for a user character
 * 	3) Prints 'X@Amonics ' on Hyperterminal, where X equals input character +1
 * User should ensure an adequate stack size in FreeRTOSConfig.h for implementing sprintf()
 ***********************************************************************************************/

#include <define.h>
#include <unistd.h>
#include <stdio.h>

#ifndef UART_MOD
#error "UART_MOD has been disabled."
#endif

/************************************************************************************************
 * Gloable Variables
 ***********************************************************************************************/
extern int fd_uart;		//File descriptor for uart (RS232)

/************************************************************************************************
 * tskComPort()
 ***********************************************************************************************/
void* tskComPort(void *ptr)
{
    static char uart_rx = 0x55;
    static char uart_tx[11];

    //=======================================================================
    start_process();
    //=======================================================================

	while(read(fd_uart, &uart_rx, 1) <= 0)
        usleep(0);
	uart_rx++;
	int number = sprintf(uart_tx, "%c%s%c", uart_rx, "@Amonics", 0x0d);
	while(write(fd_uart, uart_tx, number) != number) usleep(0);

    //=======================================================================
    end_process();
    //=======================================================================
}
