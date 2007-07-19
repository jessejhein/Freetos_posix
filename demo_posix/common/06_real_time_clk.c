/************************************************************************************************
 * File:            06_read_time_clk.c
 * Description:     test program for using freeRTOS and Posix thread API
 ***********************************************************************************************
 * DESCRIPTION:
 * This program creates 2 threads (tasks). 
 * 1)   Thread 1 checks a character from uart every 100ms
 * 2)   Thread 2 prints the time and adc value every 5sec
 ***********************************************************************************************/

#include <pthread.h>
#include <define.h>
#include <fcntl.h>
#include <asm/delay.h>

extern tskClock();
extern tskComPort();

pthread_mutex_t myMutex;
unsigned int eeprom_ptr;

/************************************************************************************************
 * Hardware setup 
 ************************************************************************************************/
int fd_uart, fd_adc, fd_eeprom;
void vSetupHardware( void ){
    led_init();

    //open uart0 in READ-WRITE mode
    fd_uart = open(UARTA, O_RDWR); 
    //open adc in READ-ONLY, NON-BLOCKING IO mode   
    fd_adc = open(ADC, O_RDONLY | O_NONBLOCK);

#if (defined(MPLAB_DSPIC30_PORT) & (EEPROM_MOD > 0))
    //open internal eeprom in READ-WRITE mode
    fd_eeprom = open(EEPROM, O_RDWR);
#elif (defined(MPLAB_DSPIC33_PORT) & (I2C_EEPROM_MOD > 0))
    fd_eeprom = open(I2C_EEPROM, O_RDWR | O_NONBLOCK);
#endif
    
    pthread_mutex_init(&myMutex, NULL);
}

/************************************************************************************************
 * User main 
 ************************************************************************************************/
void vUserMain(){
    //Identify your threads here
    pthread_t th_uart, th_clock;
    
    //Create your threads here
    pthread_create(&th_uart, NULL, tskComPort, NULL);
    pthread_create(&th_clock, NULL, tskClock, NULL);
    
    //Main program thread should waits here while user threads are running  
    pthread_join(th_uart, NULL);
    pthread_join(th_clock, NULL);
}
