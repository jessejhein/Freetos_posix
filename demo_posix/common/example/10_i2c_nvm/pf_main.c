/************************************************************************************************
 * File:            10_i2c_nvm.c
 * Description:     This program is used to erase the external I2C eeprom 24LC256
 *                  The program first erase the eeprom page by page,
 *                  waits for a user character, and then
 *                  validate the all content of eeprom is 0x00
 ***********************************************************************************************/

#include <pthread.h>
#include <define.h>
#include <fcntl.h>
#include <unistd.h>

/************************************************************************************************
 * Thread/Coroutine Functions 
 ************************************************************************************************/
extern void* tskFlashLED(void* ptr);
extern void* tskEEPROM(void* ptr);

/************************************************************************************************
 * Hardware setup 
 ************************************************************************************************/
int fd_uart;
int fd_eeprom;
void vSetupHardware( void ){
    led_init();

    //open uart0 in READ-WRITE mode
    fd_uart = open(UARTA, O_RDWR); 

    fd_eeprom = open(EEPROM, O_RDWR | O_NONBLOCK);
}

/************************************************************************************************
 * User main 
 ************************************************************************************************/
void vUserMain(){
    //Identify your threads here
    pthread_t thread_led1, thread_eeprom;
    
    static unsigned int arg_led1[] = {0, 1); 

    //Create your threads here
    pthread_create(&thread_led1, NULL, tskFlashLED, arg_led1);
    pthread_create(&thread_eeprom, NULL, tskEEPROM, NULL);
}
