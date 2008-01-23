/************************************************************************************************
 * File:            08_rs485.c
 * Description:     test program for using freeRTOS and Posix thread API
 ***********************************************************************************************
 * DESCRIPTION:
 * This program creates 2 threads (tasks). 
 * 1)   Thread 1 flashes an led every 2 sec.
 * 2)   Thread 2 checks a character from uart (rs485) every 100ms
 ***********************************************************************************************/

#include <pthread.h>
#include <define.h>
#include <fcntl.h>
#include <asm/delay.h>

/************************************************************************************************
 * Thread/Coroutine Functions 
 ************************************************************************************************/
extern void* tskFlashLED(void* ptr);
extern void* tskComPort(void* ptr);

/************************************************************************************************
 * Hardware setup 
 ************************************************************************************************/
int fd_232, fd_uart;

void vSetupHardware( void )
{
    led_init();

    //open uart0 in READ-WRITE mode
    fd_232 = open(UARTA, O_RDWR); 

    //open uart1 in READ-WRITE mode
    fd_uart = open(UARTB, O_RDWR); 

}

/************************************************************************************************
 * User main 
 ************************************************************************************************/
void vUserMain()
{
    //Identify your threads here
    pthread_t th_led1, th_uart;
    static unsigned int arg_led1 = 0; //Index, must be declared static or global

    //Create your threads here
    pthread_create(&th_led1, NULL, tskFlashLED, &arg_led1);
    pthread_create(&th_uart, NULL, tskComPort, NULL);
}
