/************************************************************************************************
 * File: 			led.c
 * Description:		control LED for module board(non-POSIX compliant)
 ***********************************************************************************************/

#include <define.h>
#include <asm/system.h>

static unsigned int status[3];

/************************************************************************************************
 * Turn LED On
 * +-- case 0: red led
 * +-- case 1: green led
 * +-- case 2: laser led
 ************************************************************************************************/
void led_on(unsigned int led){
	switch(led){
        case 0 :        
            LATC |= 0x2000;                     // red led (RC13)
            break;
        case 1 :        
            LATC |= 0x4000;                     // green led (RC14)
            break;
        case 2 :
            LATC |= 0x8000;                     // Laser Led (RC15)
            break;
	}
    if(led<3) status[led] = 1;
}

/************************************************************************************************
 * Turn LED Off
 * +-- case 0: red led
 * +-- case 1: green led
 * +-- case 2: laser led
 ************************************************************************************************/
void led_off(unsigned int led){
	switch(led){
        case 0 :                                    
            LATC &= 0xDFFF;                     // red led (RC13)
            break;
        case 1 :                                    
            LATC &= 0xBFFF;                     // green led (RC14)
            break;
        case 2 :
            LATC &= 0x7FFF;                     // Laser Led (RC15)
            break;
	}
    if(led<3) status[led] = 0;
}

/************************************************************************************************
 * Current status of LED
 ************************************************************************************************/
int led_status(unsigned int led){
	return (led<3)? status[led] : -1;	
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
 * +-- Set RC13 (Red LED) as output
 * +-- Set RC14 (Green LED) as output
 * +-- Set RC15 (Laser LED) as output 
 ************************************************************************************************/
void led_init( void ){
    TRISC &= 0x1FFF;   
}
