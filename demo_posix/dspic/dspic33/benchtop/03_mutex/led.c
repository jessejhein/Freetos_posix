/************************************************************************************************
 * File:            led.c
 * Description:     control LED for benchtop board(non-POSIX compliant)
 * Board Version:   0.06.01
 ***********************************************************************************************/

#include <define.h>

static unsigned int buzzer_status;

/************************************************************************************************
 * Turn LED On
 * +-- case 0: LED, Power indication
 * +-- case 1: Buzzer
 ************************************************************************************************/
void led_on(unsigned int led){
    switch (led) {
        case 0 :            // led
            LATC |= 0x8000; 
            break;
        case 1 :            // buzzer
            buzzer_status = 1;
            LATC |= 0x0004;
            break;
    }
}

/************************************************************************************************
 * Turn LED Off
 * +-- case 0: LED, Power indication
 * +-- case 1: Buzzer
 ************************************************************************************************/
void led_off(unsigned int led){
    switch (led) {
        case 0 :                                    
            LATC &= 0x7FFF;
            break;
        case 1 :                                    
            buzzer_status = 0;
            LATC &= 0xFFFB;
            break;
    }
}

/************************************************************************************************
 * Current status of LED
 ************************************************************************************************/
int led_status(unsigned int led){
    switch(led){
        case 0: return _RC15;
        case 1: return buzzer_status;                
        default:return -1;
    }   
}

/************************************************************************************************
 * Toggle LED
 ************************************************************************************************/
void led_toggle(unsigned int led){
    if(led_status(led) == 0){
        led_on(led);
    }
    else{
        led_off(led);
    }
}

/************************************************************************************************
 * Initialize LED-pins for Digital Outputs
 * +-- Set RC2  (Buzzer) as output
 * +-- Set RC15 (LED) as output 
 ************************************************************************************************/
void led_init( void ){
    TRISC &= 0x7FFB;
}
