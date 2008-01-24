/************************************************************************************************
 * File:            led.c
 * Description:     control LED for benchtop board(non-POSIX compliant)
 * Board Version:   0.06.01
 ***********************************************************************************************/

#include <define.h>

static unsigned int buzzer_status;

/************************************************************************************************
 * Turn LED On
 * +-- case 0: LED1, Power indication
 * +-- case 1: LED2, Laser 0 status indication
 * +-- case 2: LED3, Laser 1 status indication
 * +-- case 3: LED4, Temperature overheat
 * +-- case 4: Buzzer
 ************************************************************************************************/
void led_on(unsigned int led){
    switch (led) {
        case 0:
            LATC |= 0x1000; 
            break;
        case 1:
            LATC |= 0x0002; 
            break;
        case 2:
            LATC |= 0x0004; 
            break;
        case 3:
            LATC |= 0x8000; 
            break;
        case 4:
            LATB |= 0x0020;
            buzzer_status = 1; 
            break;
    }
}

/************************************************************************************************
 * Turn LED Off
 * +-- case 0: LED1, Power indication
 * +-- case 1: LED2, Laser 0 status indication
 * +-- case 2: LED3, Laser 1 status indication
 * +-- case 3: LED4, Temperature overheat
 * +-- case 4: Buzzer
 ************************************************************************************************/
void led_off(unsigned int led){
    switch (led) {
        case 0:                         //LED1 off, Power indication 
            LATC &= 0xEFFF; 
            break;
        case 1:                         //LED2 on
            LATC &= 0xFFFD; 
            break;
        case 2:                         //LED3 on
            LATC &= 0xFFFB; 
            break;
        case 3:                         //LED4 on
            LATC &= 0x7FFF; 
            break;
        case 4:                         //Buzzer off 
            LATB &= 0xFFDF; 
            buzzer_status = 0; 
            break;       
    }
}

/************************************************************************************************
 * Current status of LED
 ************************************************************************************************/
int led_status(unsigned int led){
    switch(led){
        case 0: return _RC15;
        case 1: return _RC1;
        case 2: return _RC2;
        case 3: return _RC12;
        case 4: return buzzer_status;                
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
 * +-- Set RC12 (LED1) as output
 * +-- Set RC1  (LED2) as output
 * +-- Set RC2  (LED3) as output
 * +-- Set RB5  (Buzzer) as output
 * +-- Set RC15 (LED4) as output 
 ************************************************************************************************/
void led_init( void ){
    TRISB &= 0xFFDF;            
    TRISC &= 0x6FF9;   
}
