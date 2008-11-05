/************************************************************************************************
 * File: 			02_drivers.c
 * Description:		test program for using freeRTOS and Posix thread API
 ***********************************************************************************************
 * DESCRIPTION:
 * This program creates 6 threads (tasks). 
 * 1)	Thread 1 flashes an led every 2 sec.
 * 2)	Thread 2 checks a character from uart every 100ms
 * 3) 	Thread 3 display a sine wave of 2Hz.
 * 4)	Thread 4 updates the ADC reading every 1 sec, store to and retrieve from eeprom.
 * 5)	Thread 5 generates a PWM signal, which changes duty cycle b/w 0.5 to 0.25 every 2 sec.
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
extern void* tskDAC(void* ptr);
extern void* tskADC(void* ptr);
extern void* tskPWM(void* ptr);

/************************************************************************************************
 * Hardware setup 
 ************************************************************************************************/
int fd_uart, fd_dac, fd_adc, fd_eeprom, fd_pwm;

void vSetupHardware( void )
{
	led_init();

    //open uart0 in READ-WRITE mode
	fd_uart = open(UARTA, O_RDWR); 
    //open dac in WRITE-ONLY mode                 
	fd_dac = open(I2C_DAC, O_WRONLY);
    //open adc in READ-ONLY, NON-BLOCKING IO mode	
	fd_adc = open(ADC, O_RDONLY | O_NONBLOCK);
#if (NVM_MOD > 0)
    //open eeprom in READ-WRITE mode
	fd_eeprom = open(EEPROM, O_RDWR | O_NONBLOCK);
#endif
    //open pwm in WRITE-ONLY mode
	fd_pwm = open(PWM, O_WRONLY);
}

/************************************************************************************************
 * User main 
 ************************************************************************************************/
void vUserMain()
{
	//Identify your threads here
	pthread_t th_led1, th_uart, th_dac, th_adc, th_pwm;
	static unsigned int arg_led1[] = {0, 1};

	//Create your threads here
	pthread_create(&th_led1, NULL, tskFlashLED, arg_led1);
	pthread_create(&th_uart, NULL, tskComPort, NULL);
	pthread_create(&th_dac, NULL, tskDAC, NULL);
	pthread_create(&th_adc, NULL, tskADC, NULL);
	pthread_create(&th_pwm, NULL, tskPWM, NULL);
}
