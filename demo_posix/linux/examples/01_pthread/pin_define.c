/**
 * \file
 * emulate control LED for module board (non-POSIX compliant)
 * \author Dennis Tsang <dennis@amonics.com>
 */

#include <stdio.h>

/** store states of led */
static unsigned int led_state[3];


/**
 * \brief turn on led
 * \param led number
 */
void 
led_on(unsigned short led)
{
  led_state[led] = 1; 
  printf("LED %d is ON.\n", (int)led);
}


/**
 * \brief turn off led
 * \param led number
 */
void 
led_off( unsigned short led)
{
  led_state[led] = 0; 
  printf("LED %d is OFF.\n", (int)led);
}


/**
 * \brief read status of LED
 * \param led number
 * \retval -1 error
 */
int 
led_status(unsigned short led)
{
  switch(led)
    {
      case 0:	
      case 1:	
      case 2: 
        return led_state[led];					
      default:
        return -1;
    }
}


/**
 * \brief initialize led
 */
void 
led_init(void)
{
  led_state[0] = 0;
  led_state[1] = 0;
  led_state[2] = 0;
  printf("ALL LEDs initialised successfully.\n");
}
