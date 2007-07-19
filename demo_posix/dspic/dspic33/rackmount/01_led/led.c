/************************************************************************************************
 * File:            led.c
 * Description:     control LED for rackmount board(non-POSIX compliant)
 ***********************************************************************************************/

#include <p33FJ128GP306.h>
#include <asm/system.h>

/************************************************************************************************
 * Turn LED On
 ************************************************************************************************/
void led_on(unsigned int led){
    switch(led){
        case 0: LATG |= 0x2000; break;
        case 1: LATG |= 0x1000; break;
        case 2: LATG |= 0x8000; break;
        case 3: LATC |= 0x8000; break;
        case 4: LATG |= 0x0001; break; //Buzzer
    }
}

/************************************************************************************************
 * Turn LED Off
 ************************************************************************************************/
void led_off(unsigned int led){
    switch(led){
        case 0: LATG &= 0xDFFF; break;
        case 1: LATG &= 0xEFFF; break;
        case 2: LATG &= 0x7FFF; break;
        case 3: LATC &= 0x7FFF; break;
        case 4: LATG &= 0xFFFE; break; //Buzzer
    }
}

/************************************************************************************************
 * Current status of LED
 ************************************************************************************************/
int led_status(unsigned int led){
    switch(led){
        case 0: return _RG13;
        case 1: return _RG12;
        case 2: return _RG15;
        case 3: return _RC15;
        case 4: return _RG0;                
        default:return -1;
    }   
}

/************************************************************************************************
 * Toggle LED
 ************************************************************************************************/
void led_toggle(unsigned int led){
    if(led_status(led) == 0)
        led_on(led);
    else
        led_off(led);
}

/************************************************************************************************
 * Initialize LED-pins for Digital Outputs
 ************************************************************************************************/
void led_init( void ){
    //Set RC15 (LED4) as output
    TRISC &= 0x7FFF;
    //Set RG13 (LED1) as output
    //Set RG12 (LED2) as output
    //Set RG15 (LED3) as output
    //Set RG0  (Buzzer) as output
    TRISG &= 0x4FFE;
    
    LATC &= 0x7FFF;
    LATG &= 0x4FFE;    
}
