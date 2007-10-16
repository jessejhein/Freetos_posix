/************************************************************************************************
 * File: 			04_http.c
 * Description:		test program for using freeRTOS and Posix thread API
 ***********************************************************************************************
 * DESCRIPTION:
 * This program creates 6 threads (tasks). 
 * 1)	Thread 1 flashes an led every 2 sec.
 * 2)	Thread 2 hostes a web server
 ***********************************************************************************************/

#include <pthread.h>
#include <define.h>
#include <fcntl.h>

/************************************************************************************************
 * Thread/Coroutine Functions 
 ************************************************************************************************/
extern void* tskFlashLED(void* ptr);
extern void* tskHTTPServer(void* ptr);
extern void* tskLaserCtrl(void* ptr);

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
	pthread_t th_led1, th_http, th_laser_ctrl;
	static unsigned int arg_led1 = 0; //Index, must be declared static or global

	//Create your threads here
	pthread_create(&th_led1, NULL, tskFlashLED, &arg_led1);
	pthread_create(&th_http, NULL, tskHTTPServer, NULL);
    pthread_create(&th_laser_ctrl, NULL, tskLaserCtrl, NULL);
}
