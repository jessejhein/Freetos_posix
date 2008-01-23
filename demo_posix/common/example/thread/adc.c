/************************************************************************************************
 * File: 			app_adc_eeprom.c
 * Description:		adc task employing posix thread and file API
 ***********************************************************************************************
 * SUMMARY:
 * 	1) 	obtain adc value every 1 sec
 * 	2) 	store value to eeprom
 * 	3)	retrieve from eeprom
 * 	4)	print to uart
 * User should ensure an adequate stack size in FreeRTOSConfig.h for implementing sprintf()
 ***********************************************************************************************/

#include <define.h>
#include <unistd.h>
#include <stdio.h>

#if (ADC_MOD == 0)
#error "ADC_MOD has been disabled."
#endif

#ifndef UART_MOD
#error "UART_MOD has been disabled."
#endif

/************************************************************************************************
 * Global Variables
 ***********************************************************************************************/
extern int fd_adc;
extern int fd_eeprom;
extern int fd_uart;

/************************************************************************************************
 * tskADC()
 ***********************************************************************************************/
#if 1
void* tskADC(void *ptr)
{
	static char adc_uart_tx[20];
	static unsigned int adc_reading, eeprom_reading; 	//Store integer value for adc values [0-4095]
	static unsigned char adc_channel = ADC_PRIMARY;	//channel to read from
	static float output = 0.0;							//Store voltage value
	ioctl(fd_adc, ADC_ADD_CH, &adc_channel);

	//======================================================================
	start_process();
    //======================================================================
    
    while(1){
    	sleep(1);   //Update every 1 sec
        //read to buffer
    	while(read(fd_adc, &adc_reading, sizeof(unsigned int)) <= 0)
    		usleep(0);
    
    #if (NVM_MOD > 0)
        //Use eeprom-----------------------------------------------------
    	while(lseek(fd_eeprom, 0, SEEK_SET) < 0) usleep(0);
    	while(write(fd_eeprom, &adc_reading, sizeof(unsigned int)) < 1)
            usleep(1000);
    	while(lseek(fd_eeprom, 0, SEEK_SET) < 0) usleep(0);
    	while(read(fd_eeprom, &eeprom_reading, sizeof(unsigned int)) < 1)
            usleep(1000);
    	output = SYSTEM_VOLT*((float)eeprom_reading/4095);		//compute voltage value
    #else
        //Use adc---------------------------------------------------------
    	output = SYSTEM_VOLT*((float)adc_reading/4095);		//compute voltage value
    #endif
        //Display Value-------------------------------------------------------
    	int number = sprintf(adc_uart_tx, "%5.3f%c", output, 0x0d); //convert to string for display
    	write(fd_uart, adc_uart_tx, number);		//send the value to screen			
    }

    //======================================================================
	end_process();
    //======================================================================
}

#else

void* tskADC(void *ptr)
{
    static unsigned int analog_reading;    //Store integer value for adc values [0-4095]
    static unsigned char adc_ctrl = 0;     //Control values, 4=>add/remove channel 4
    static float output = 0.0;             //Store voltage value at Channle 4 
    //======================================================================
    static char uart_rx[1];               //Receive buffer
    static char uart_tx[10];              //Transmit buffer for display
    //======================================================================
    start_process();
    
    while(1){
        while(read(fd_uart, uart_rx, 1) < 1)     //Wait for user input of a character
            usleep(0);
        //---------------------------------------------------
        switch(uart_rx[0]){
            case '0': adc_ctrl = 0; break;
            case '1': adc_ctrl = 1; break;
            case '2': adc_ctrl = 2; break;
            case '3': adc_ctrl = 3; break;
            case '4': adc_ctrl = 4; break;
            case '5': adc_ctrl = 5; break;
            case '6': adc_ctrl = 6; break;
            case '7': adc_ctrl = 7; break;
            case '8': adc_ctrl = 8; break;
            case '9': adc_ctrl = 9; break;
            case 'A': case 'a': adc_ctrl = 10; break;
            case 'B': case 'b': adc_ctrl = 11; break;
            case 'C': case 'c': adc_ctrl = 12; break;
            case 'D': case 'd': adc_ctrl = 13; break;
            case 'E': case 'e': adc_ctrl = 14; break;
            case 'F': case 'f': adc_ctrl = 15; break;
            default:
                adc_ctrl = 0;
        }
        ioctl(fd_adc, ADC_ADD_CH, &adc_ctrl);               //Add channel
        //---------------------------------------------------
        while(read(fd_adc, &analog_reading, 2) < 1)      //read to buffer
            usleep(0);
        output = SYSTEM_VOLT*((float)analog_reading/4095);  //compute volage value
        int number = sprintf(uart_tx, "%.3f %c %c", output, 0x0A, 0x0D); //convert to string for display        
        write(fd_uart, uart_tx, number);        //send the value to screen
        //---------------------------------------------------
    }

    end_process();
}
#endif
