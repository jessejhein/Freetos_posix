/************************************************************************************************
 * File: 			01_led.c
 * Description:		test program for using freeRTOS and Posix thread API
 ***********************************************************************************************
 * DESCRIPTION:
 * This program creates 2 threads (tasks). 
 * 1)	Thread 1 flashes an led every 2 sec.
 * 2)	Thread 2 flashes an led every 4 sec.
 ***********************************************************************************************/

#include <pthread.h>
#include <define.h>
#include <fcntl.h>

extern void* tskFlashLED(void* ptr);

/************************************************************************************************
 * Hardware setup 
 ************************************************************************************************/
int fd_uart;
void vSetupHardware( void ){
	led_init();

    //open uart0 in READ-WRITE mode
    fd_uart = open(UARTA, O_RDWR); 
}

/************************************************************************************************
 * User main 
 ************************************************************************************************/
void vUserMain(){
	//Identify your threads here
	pthread_t thread_led1, thread_led2;
	
	static unsigned int arg_led1[] = {0, 1};
	static unsigned int arg_led2[] = {1, 2};

	//Create your threads here
	pthread_create(&thread_led1, NULL, tskFlashLED, arg_led1);
	pthread_create(&thread_led2, NULL, tskFlashLED, arg_led2);
}
