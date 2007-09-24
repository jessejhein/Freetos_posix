/************************************************************************************************
 * File:    05_disable_sche.c
 ***********************************************************************************************/

#include <define.h>
#include <asm/delay.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

int fd_uart, fd_adc, fd_eeprom, fd_pwm;

#ifndef portTICK_RATE_MS
#define portTICK_RATE_MS            10
#endif

/************************************************************************************************
 * tskComPort()
 ***********************************************************************************************/
void* clock_process(void)
{
    static char uart_rx;
    static char uart_tx[25];
    static int number;

    //-------------------------------------------------------------------
    start_process();
    //-------------------------------------------------------------------

    while(read(fd_uart, &uart_rx, 1) <= 0) usleep(0);
    //received a char, get current time
    time_t tick = os_time(NULL);
    time_t sec_t = time(NULL);
    //print result
    int tick_sec = (tick*portTICK_RATE_MS)/1000;  //compute sec
    int hour = sec_t/3600;
    int min = (sec_t-hour*3600)/60;
    int sec = (sec_t-hour*3600-min*60);        
    number = sprintf(uart_tx, "%d:%d:%d   %d%c%c", 
                         hour, min, sec, tick_sec, 0x0A, 0x0D);
    write(fd_uart, uart_tx, number);
    //-------------------------------------------------------------------
    end_process();
    //-------------------------------------------------------------------
}

/************************************************************************************************
 * Hardware setup 
 ************************************************************************************************/
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
    //open pwm in WRITE-ONLY mode
    fd_pwm = open(PWM, O_WRONLY);
}

/************************************************************************************************
 * User main 
 ************************************************************************************************/
void vUserMain(){
    unsigned int arg_led1 = 0;  //Index, must be declared static or global
    while(1){
        tskFlashLED(&arg_led1);
        tskPWM();
        tskADC();
        clock_process();
        mdelay(100);
    }
}
