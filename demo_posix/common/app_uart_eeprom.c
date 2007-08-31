/************************************************************************************************
 * File:            app_uart_eeprom.c
 * Description:     uart task employing posix thread and file API
 ***********************************************************************************************
 * SUMMARY:
 *  1) Use UARTA (RS232) at default baud rate (19200bps). 
 *  2) Check for a user character
 *  3) Prints content in eeprom
 ***********************************************************************************************/

#include <define.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <asm/delay.h>
#include <pthread.h>

#if (UART_MOD == 0)
#error "UART_MOD has been disabled."
#endif

/************************************************************************************************
 * Gloable Variables
 ***********************************************************************************************/
extern int fd_uart;     //File descriptor for uart (RS232)
extern int fd_eeprom;   //File descriptor for eeprom
extern pthread_mutex_t myMutex;
extern time_t sec_t;

#if (defined(MPLAB_DSPIC33_PORT) & (I2C_EEPROM_MOD > 0))
#define EEPROM_SIZE     1024
#endif

/************************************************************************************************
 * tskComPort()
 ***********************************************************************************************/
void* tskComPort(void* ptr)
{
    static unsigned char eeprom_uart_tx[40];
    static unsigned char uart_rx;
    static time_t data;
    static unsigned int rd_eeprom_ptr = 0;

    //=======================================================================
    start_process();
    //=======================================================================

    while(read(fd_uart, &uart_rx, 1) <= 0)
        usleep(0);
        
    /*
     * Print current time
     */
    while(pthread_mutex_lock(&myMutex) != 0) usleep(0);
    data = sec_t;
    pthread_mutex_unlock(&myMutex);

    int day = (data/(time_t)86400);
    int hour =(data-day*(time_t)86400)/3600;
    int min = (data-day*(time_t)86400-(time_t)hour*3600)/60;
    int sec = (data-day*(time_t)86400-(time_t)hour*3600-(time_t)min*60);        
    int number = sprintf(eeprom_uart_tx, "[%d] %d:%d:%d%c%c", day, hour, min, sec, 0x0a, 0x0d);
    write(fd_uart, eeprom_uart_tx, number);

    /*
     * Print data in eeprom
     */    
    int i;
    rd_eeprom_ptr = 0;
    for(i = 0; i<EEPROM_SIZE/sizeof(time_t); i++){
        
        //read eeprom
        while(pthread_mutex_lock(&myMutex) != 0) usleep(0);
        while(lseek(fd_eeprom, rd_eeprom_ptr, SEEK_SET) < 0) usleep(0);;
        while(read(fd_eeprom, &data, sizeof(time_t)) != sizeof(time_t));
        rd_eeprom_ptr += sizeof(time_t);
        pthread_mutex_unlock(&myMutex);

        //print result
        day = (data/(time_t)86400);
        hour =(data-day*(time_t)86400)/3600;
        min = (data-day*(time_t)86400-(time_t)hour*3600)/60;
        sec = (data-day*(time_t)86400-(time_t)hour*3600-(time_t)min*60);        
        number = sprintf(eeprom_uart_tx, "%d = [%d] %d:%d:%d%c%c", i, day, hour, min, sec, 0x0a, 0x0d);
        write(fd_uart, eeprom_uart_tx, number);
        
        usleep(20000UL);
    }

    //=======================================================================
    end_process();
    //=======================================================================
}
