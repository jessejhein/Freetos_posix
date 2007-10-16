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

    fd_eeprom = open(EEPROM, O_RDWR | O_NONBLOCK);
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
        while(lseek(fd_eeprom, 0, SEEK_SET) < 0) usleep(0);
    
        while(printStr("Programming EEPROM...")<0);
        while(newline()<0);
    
        for(i = 0; i<I2C_EEPROM_SIZE/I2C_EEPROM_PAGE_SIZE; i++){
            while(write(fd_eeprom, page, I2C_EEPROM_PAGE_SIZE) != I2C_EEPROM_PAGE_SIZE)
                usleep(0);
        }

        while(printStr("Completed.")<0);
        while(newline()<0);
        while(newline()<0);
    }

    
    else if(uart_rx == 'C' || uart_rx == 'c'){
    
        while(printStr("Checking...")<0);
        while(newline()<0);
            
        //Reset pointer and read
        while(lseek(fd_eeprom, 0, SEEK_SET) < 0) usleep(0);
        for(i = 0; i<I2C_EEPROM_SIZE; i++){
            while(read(fd_eeprom, &data, 1) != 1)
                usleep(0);
                
            //If content is not 0x00, print to console            
            if(data != 0){
                while(printStr("[")<0); 
                while(printDec(i)<0);
                while(printStr("] = 0x")<0); 
                while(printHex(data, 2)<0);
                while(newline()<0);
                while(newline()<0);
                usleep(20000UL);
            }            
        }

        //Process is successful
        while(printStr("Completed.")<0);
        while(newline()<0);
        while(newline()<0);
    }
    
    else{
        while(newline()<0);
        while(printStr("Help Screen")<0);
        while(newline()<0);
        while(printStr("===========")<0);
        while(newline()<0);
        while(printStr("Enter the Following Key:")<0);
        while(newline()<0);
        while(printStr(" e: Erase EEPROM")<0);
        while(newline()<0);
        while(printStr(" c: Check EEPROM Content as zero")<0);
        while(newline()<0);
        while(newline()<0);
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
}
