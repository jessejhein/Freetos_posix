/************************************************************************************************
 * File:            test.c
 ***********************************************************************************************/

#include <pthread.h>
#include <define.h>
#include <fcntl.h>

/************************************************************************************************
 * Thread/Coroutine Functions 
 ************************************************************************************************/
extern tskFlashLED();
extern tskHTTPServer();

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
    pthread_t th_led2, th_http;
    static unsigned int arg_led2 = 0; //Index, must be declared static or global

    //Create your threads here
    pthread_create(&th_led2, NULL, tskFlashLED, &arg_led2);
    pthread_create(&th_http, NULL, tskHTTPServer, NULL);
    
    //Main program thread should waits here while user threads are running  
    pthread_join(th_led2, NULL);
    pthread_join(th_http, NULL);
}
