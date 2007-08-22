/************************************************************************************************
 * File: 			app_pwm.c
 * Description:		pwm task employing posix thread and file API
 ***********************************************************************************************
 * SUMMARY:
 * 	1) 	Generate PWM signal of 250Hz
 * 	2)	Duty cycle alternate between 0.5 and 0.25 every 2sec
 ***********************************************************************************************/

#include <define.h>
#include <unistd.h>
#include <sys/ioctl.h>

#if (PWM_MOD == 0)
#error "PWM_MOD has been disabled."
#endif

/************************************************************************************************
 * Global Variables
 ***********************************************************************************************/
extern int fd_pwm;		//File descriptor for uart (RS232)

/************************************************************************************************
 * tskPWM()
 ***********************************************************************************************/
void* tskPWM(void *ptr)
{
	static unsigned long channelA = PWM_PRIMARY;					
	ioctl(fd_pwm, PWM_SELECT_CH, &channelA);		//Select channel
	static unsigned long period = 4000000;
	ioctl(fd_pwm, PWM_SET_PERIOD, &period);			//set period 4ms, 250Hz
	static unsigned long duty = 2000000;

	//=======================================================================
	start_process();
    //=======================================================================

	sleep(2);          //sleep every 2s
	duty = 2000000;
	write(fd_pwm, &duty, sizeof(unsigned long));	//set duty cycle to 0.5
	
    sleep(2);           //sleep every 2s
	duty = 1000000;
	write(fd_pwm, &duty, sizeof(unsigned long));	//set duty cycle to 0.25	

    //=======================================================================
	end_process();
    //=======================================================================
}
