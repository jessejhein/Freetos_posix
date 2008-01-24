/*
 * app_clock.c
 * This task prints the time and adc value every 5sec
 */

#include <define.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <asm/delay.h>

#if (ADC_MOD == 0)
#error "ADC_MOD has been disabled."
#endif

#ifndef UART_MOD
#error "UART_MOD has been disabled."
#endif

/************************************************************************************************
 * Gloable Variables
 ***********************************************************************************************/
extern int fd_uart;     //File descriptor for uart (RS232)
extern int fd_adc;      //File descriptor for adc
extern int fd_eeprom;   //File descripter for eeprom
extern pthread_mutex_t myMutex;

time_t sec_t;

/************************************************************************************************
 * tskComPort()
 ***********************************************************************************************/
void* tskClock(void* ptr)
{
    static char adc_uart_tx[40];
    static unsigned char adc_channel = ADC_PRIMARY; //channel to read from
    static unsigned int adc_reading = 0;
    static float output = 0.0;
    static int last_save = 0;
    static int wr_eeprom_ptr = 0;
    
    ioctl(fd_adc, ADC_ADD_CH, &adc_channel);

    //======================================================================
    start_process();
    //======================================================================
    
    while(1){
        sleep(5);
    
        while(read(fd_adc, &adc_reading, sizeof(unsigned int)) <= 0)
            usleep(0);
        output = SYSTEM_VOLT*((float)adc_reading/4095);     //compute voltage value
    
        while(pthread_mutex_lock(&myMutex) != 0) usleep(0);
        sec_t = time(NULL);
        pthread_mutex_unlock(&myMutex);
    
        //print result
        int day = (sec_t/(time_t)86400);
        int hour =(sec_t-day*(time_t)86400)/3600;
        int min = (sec_t-day*(time_t)86400-(time_t)hour*3600)/60;
        int sec = (sec_t-day*(time_t)86400-(time_t)hour*3600-(time_t)min*60);        
    //    int number = sprintf(adc_uart_tx, "[%d] %d:%d:%d  %5.3f%c", day, hour, min, sec, output, 0x0d);
    //    write(fd_uart, adc_uart_tx, number);
    
        if((min%15==0) && (min!=last_save)){
            while(pthread_mutex_lock(&myMutex) != 0) usleep(0);
            while(lseek(fd_eeprom, wr_eeprom_ptr, SEEK_SET) < 0) usleep(0);
            write(fd_eeprom, &sec_t, sizeof(time_t));
            last_save = min;
            wr_eeprom_ptr += sizeof(time_t);
            pthread_mutex_unlock(&myMutex);
        }
    }
    //======================================================================
    end_process();
    //======================================================================
}


/************************************************************************************************
 * Gloable Variables
 ***********************************************************************************************/
extern int fd_uart;     //File descriptor for uart (RS232)
extern int fd_eeprom;   //File descriptor for eeprom
extern pthread_mutex_t myMutex;
extern time_t sec_t;

#if (NVM_MOD > 0)
#ifndef EEPROM_SIZE
#define EEPROM_SIZE     1024
#endif
#endif

/************************************************************************************************
 * tskComPort()
 ***********************************************************************************************/
void* tskComPort(void* ptr)
{
    static char eeprom_uart_tx[40];
    static unsigned char uart_rx;
    static time_t data;
    static unsigned int rd_eeprom_ptr = 0;

    //=======================================================================
    start_process();
    //=======================================================================
    
    while(1){
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
    }
    //=======================================================================
    end_process();
    //=======================================================================
}

