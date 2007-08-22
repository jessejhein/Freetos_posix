/************************************************************************************************
 * File: 			app_mutex.c
 * Description:		mutex task employing posix thread and file API
 ***********************************************************************************************
 * SUMMARY:
 * Increment $counter 5000 times, then prints "Ty: counter = x"
 ***********************************************************************************************/

#include <pthread.h>
#include <define.h>
#include <unistd.h>
#include <stdio.h>

#if (UART_MOD == 0)
#error "UART_MOD has been disabled."
#endif

#define MUTEX_ENABLE	1

/************************************************************************************************
 * Global Variable
 ************************************************************************************************/
extern pthread_mutex_t myMutex;
extern unsigned int counter;
extern int fd_uart;

/************************************************************************************************
 * tskMutex()
 ***********************************************************************************************/
void* tskMutex(void* ptr)
{
	unsigned int index = *((unsigned int*)ptr);
	unsigned char buf[20];
	char done = 0;
    int i=0, j=0;
    
	for(;;){
		//-------------------------------------------------------------------
		//Do Something
		if(done == 0){
            led_on(index-1);
			for(i=0;i<5000;i++){

#if(MUTEX_ENABLE == 1)
                while(pthread_mutex_lock(&myMutex) != 0);
#endif

				counter++;
                for(j=0; j<index*5000; j++);


#if(MUTEX_ENABLE == 1)
				pthread_mutex_unlock(&myMutex);
#endif
			}
            led_off(index-1);
			int number = sprintf(buf, "T%d: counter = %d%c%c", index, counter, 0x0A, 0x0D);
			screen_out(fd_uart, buf, number); 
			done = 1;
		} 		
		//-------------------------------------------------------------------
	}
}
