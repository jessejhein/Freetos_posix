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

extern tskFlashLED();

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
tskEEPROM()
{
    static int done = 0, number = 0;
    static unsigned char uart_tx[30];
    static unsigned char page[I2C_EEPROM_PAGE_SIZE];
    static unsigned int i;
    static unsigned char data = 0;
        
    //=======================================================================
    start_process();
    //=======================================================================
    
    sleep(5);
    
    //Erase the entire eeprom, page by page 
    if(done == 0){

        lseek(fd_eeprom, 0, SEEK_SET);

        number = sprintf(uart_tx, "%s%c", "Programming EEPROM...", 0x0d);
        write(fd_uart, uart_tx, number);
    

        for(i = 0; i<I2C_EEPROM_SIZE/I2C_EEPROM_PAGE_SIZE; i++){
            while(write(fd_eeprom, page, I2C_EEPROM_PAGE_SIZE) != I2C_EEPROM_PAGE_SIZE)
                usleep(0);
        }
    
        number = sprintf(uart_tx, "%s%c", "Completed.", 0x0d);
        write(fd_uart, uart_tx, number);
        
        done == 1;
    }
    
    //Wait for an input character  
    while(read(fd_uart, uart_tx, 1) <= 0)
        usleep(0);

    number = sprintf(uart_tx, "Checking... %c", 0x0d);
    write(fd_uart, uart_tx, number);
    
    //Reset pointer and read
    lseek(fd_eeprom, 0, SEEK_SET);
    for(i = 0; i<I2C_EEPROM_SIZE; i++){
        while(read(fd_eeprom, &data, 1) != 1)
            usleep(0);
            
        //If content is not 0x00, print to console            
        if(data != 0){
            number = sprintf(uart_tx, "[%d] = %d%c", i, data, 0x0d);
            write(fd_uart, uart_tx, number);
            usleep(20000UL);
        }            
    }
    
    //Process is successful
    number = sprintf(uart_tx, "%s%c", "Completed.", 0x0d);
    write(fd_uart, uart_tx, number);
    
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
