/************************************************************************************************
 * File: 			08_eeprom.c
 * 
 * Description:     This program is used to erase the external I2C eeprom 24LC256
 *                  The program first erase the eeprom page by page,
 *                  waits for a user character, and then
 *                  validate the all content of eeprom is 0x00
 ***********************************************************************************************/

#include <pthread.h>
#include <define.h>
#include <fcntl.h>
#include <unistd.h>

extern void* tskFlashLED(void* ptr);

/************************************************************************************************
 * Hardware setup 
 ************************************************************************************************/
int fd_uart;
int fd_eeprom;
void vSetupHardware( void ){
	led_init();

    //open uart0 in READ-WRITE mode
    fd_uart = open(UARTA, O_RDWR); 

    fd_eeprom = open(I2C_EEPROM, O_RDWR | O_NONBLOCK);
}


/************************************************************************************************
 * tskEEPROM()
 * erase eeprom
 ***********************************************************************************************/
void* tskEEPROM(void* ptr)
{
    static unsigned char uart_rx;
    static unsigned char page[I2C_EEPROM_PAGE_SIZE];
    static unsigned int i;
    static unsigned char data = 0;
        
    //=======================================================================
    start_process();
    //=======================================================================
    
    //Wait for an input character  
    while(read(fd_uart, &uart_rx, 1) <= 0)
        usleep(0);
    
    if(uart_rx == 'E' || uart_rx == 'e'){
        //Erase the entire eeprom, page by page 
        lseek(fd_eeprom, 0, SEEK_SET);
    
        printStr("Programming EEPROM...");
        newline();
    
        for(i = 0; i<I2C_EEPROM_SIZE/I2C_EEPROM_PAGE_SIZE; i++){
            while(write(fd_eeprom, page, I2C_EEPROM_PAGE_SIZE) != I2C_EEPROM_PAGE_SIZE)
                usleep(0);
        }

        printStr("Completed.");
        newline();
        newline();   
    }

    
    else if(uart_rx == 'C' || uart_rx == 'c'){
    
        printStr("Checking...");
        newline();
            
        //Reset pointer and read
        lseek(fd_eeprom, 0, SEEK_SET);
        for(i = 0; i<I2C_EEPROM_SIZE; i++){
            while(read(fd_eeprom, &data, 1) != 1)
                usleep(0);
                
            //If content is not 0x00, print to console            
            if(data != 0){
                printStr("["); printDec(i); printStr("] = 0x"); printHex(data, 2);
                newline();
                newline();
                usleep(20000UL);
            }            
        }

        //Process is successful
        printStr("Completed.");
        newline();
        newline();
    }
    
    else{
        newline();
        printStr("Help Screen");
        newline();
        printStr("===========");
        newline();
        printStr("Enter the Following Key:");
        newline();
        printStr(" e: Erase EEPROM");
        newline();
        printStr(" c: Check EEPROM Content as zero");
        newline();
        newline();
    }
    
    //=======================================================================
    end_process();
    //=======================================================================
}



/************************************************************************************************
 * User main 
 ************************************************************************************************/
void vUserMain(){
	//Identify your threads here
	pthread_t thread_led1, thread_eeprom;
	
	static unsigned int arg_led1 = 1;  //Index, must be declared static or global

	//Create your threads here
	pthread_create(&thread_led1, NULL, tskFlashLED, &arg_led1);
	pthread_create(&thread_eeprom, NULL, tskEEPROM, NULL);
	
	//Main program thread should waits here while user threads are running	
	pthread_join(thread_led1, NULL);
	pthread_join(thread_led2, NULL);
}
