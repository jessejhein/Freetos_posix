/************************************************************************************************
 * File:            10_crthread.c
 * Description:     test program for using freeRTOS and Posix thread API
 ***********************************************************************************************/

#include <pthread.h>
#include <define.h>
#include <fcntl.h>
#include <unistd.h>

extern void* tskFlashLED(void* ptr);
extern void* process_1(void* ptr);
extern void* process_2(void* ptr);

extern void* adj(void* ptr);
extern void* enable(void* ptr);
extern void* disable(void* ptr);

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
    pthread_t thread_1, thread_2;
    
    static unsigned int arg_led1 = 1;  //Index, must be declared static or global
    static unsigned int arg_led2 = 0;  //Index, must be declared static or global
    
    //Create your threads here
    pthread_create(&thread_led1, NULL, tskFlashLED, &arg_led1);
    pthread_create(&thread_led2, NULL, tskFlashLED, &arg_led2);
    pthread_create(&thread_1, NULL, process_1, NULL);
    pthread_create(&thread_2, NULL, process_2, NULL);
}

void* process_1(void* ptr){
    //Identify your threads here
    pthread_t thread_1, thread_2, thread_3;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    
    static unsigned char p1arg_adj[] = {1, 120};  //Index, must be declared static or global
    static unsigned char p1arg_enable = 1;  //Index, must be declared static or global
    static unsigned char p1arg_disable = 1;  //Index, must be declared static or global
    
    unsigned char done =0;

    start_process();

    while(pthread_create(&thread_1, &attr, enable, &p1arg_enable) < 0) usleep(0);
    usleep(100000UL);
    while(pthread_create(&thread_2, &attr, adj, p1arg_adj) < 0) usleep(0);
    usleep(100000UL);
    while(pthread_create(&thread_3, &attr, disable, &p1arg_disable) < 0) usleep(0);
    usleep(100000UL);

    end_process();
}

void* process_2(void* ptr){
    //Identify your threads here
    pthread_t thread_1, thread_2, thread_3;
    
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    
    static unsigned char p2arg_adj[] = {2, 240};  //Index, must be declared static or global
    static unsigned char p2arg_enable = 2;  //Index, must be declared static or global
    static unsigned char p2arg_disable = 2;  //Index, must be declared static or global

    start_process();

    while(pthread_create(&thread_1, &attr, enable, &p2arg_enable) < 0) usleep(0);
    usleep(200000UL);
    while(pthread_create(&thread_2, &attr, adj, p2arg_adj) < 0) usleep(0);
    usleep(200000UL);
    while(pthread_create(&thread_3, &attr, disable, &p2arg_disable) < 0) usleep(0);
    usleep(200000UL);

    end_process();
}

