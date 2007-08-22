/************************************************************************************************
 * File:            07_gpio.c
 * Description:     test enter key and rotary key
 ***********************************************************************************************/

#include <pthread.h>
#include <define.h>
#include <fcntl.h>

extern void* tskFlashLED(void* ptr);
extern void* tskDAC(void* ptr);
extern void* tskKB(void* ptr);

/************************************************************************************************
 * Hardware setup 
 ************************************************************************************************/
int fd_uart, fd_dac, fd_kb;

void vSetupHardware( void ){
    led_init();

    //open uart0 in READ-WRITE mode
    fd_uart = open(UARTA, O_RDWR); 
    //open dac in WRITE-ONLY mode                 
    fd_dac = open(I2C_DAC, O_WRONLY);
    //open keyboard in READ-ONLY mode                 
    fd_kb = open(KB, O_RDONLY);
}

/************************************************************************************************
 * User main 
 ************************************************************************************************/
void vUserMain(){
    //Identify your threads here
    pthread_t th_led, th_dac, th_kb;

    static unsigned int arg_led = 0;  //Index, must be declared static or global
    
    //Create your threads here
    pthread_create(&th_led, NULL, tskFlashLED, &arg_led);
    pthread_create(&th_dac, NULL, tskDAC, NULL);
    pthread_create(&th_kb, NULL, tskKB, NULL);
    
    //Main program thread should waits here while user threads are running  
    pthread_join(th_led, NULL);
    pthread_join(th_dac, NULL);
    pthread_join(th_kb, NULL);
}
