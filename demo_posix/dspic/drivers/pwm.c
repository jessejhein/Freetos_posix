/************************************************************************************************
 * File: 			pwm.c
 * Description:		control pwm channels output
 ***********************************************************************************************
 * SUMMARY:
 * 1)	The PWM module consists of 8 channels using the output compare module of dsPic.
 * 2)	These channels are locate at pin 46 (OC1), 49 (OC2), 50 (OC3), 51 (OC4), 52 (OC5), 
 * 		53 (OC6), 54 (OC7), 55 (OC8). These pins are shared with port D.
 * 3) 	The driver has a POSIX-compliant interface with open(), write(), and ioctl().
 * 4)	The range of PWM freqeuencies obtainable is 2Hz to 5MHz. Suggested range of operation 
 * 		is 2Hz to 1MHz. 
 * 5)	Users should take note that:
 * 		a) 	dsPic should use system clock at 30MHz
 * 		b) 	All PWM channels share the same timer (Timer 2)
 * 		c) 	When changing the PWM Period using ioctl(), all channels will be affected.
 * 		d)	At steady state, all channels will be triggered simulteously (all rise to HIGH at 
 * 			the same time)
 * 		d) 	Accuracy on pwm period and duty cycle timings is not guaranteed. Unstability may 
 * 			arise due to rise/fall times and resolution of timer, which becomes critical for 
 * 			high frequency PWMs. Please observe the suggested frequency range in point (4) above.
 * 		e)	There is a time delay between write() and actual change of pwm pulses. Change will
 * 			take effect on the next timer cycle.
 * 		f)	A higher pwm frequency means a coarser resolution of duty cycle (See Table Below)
 * 6)	When incorporating with FreeRTOS, the kernel clock may impose limitation to the 
 * 		stability of high frequency PWM (since the kernel disable all interrupt at each kernel
 * 		clock tick). Please evaluate the trade-off between PWM frequency range and kernel clock
 * 		configuration. 
 ***********************************************************************************************
 *  Rough estimate of relationship between pwm frequency and resolution of duty cycle
 *  +------------------------------+-----+----+------+------+------+
 *  | Frequency (Hz)               | 5M  | 1M | 500k | 200k | <20k |
 * 	+------------------------------+-----+----+------+------+------+  
 *  | Resolution of Duty Cycle (%) | 25  |  6 |   3  |  0.8 | >0.1 |
 *  +------------------------------+-----+----+------+------+------+
 * 
 ***********************************************************************************************/

#include <define.h>
#include <sys/ioctl.h>
#include <asm/system.h>
#include <fcntl.h>
#include <errno.h>

#if (PWM_MOD == 0)
//do not include pwm module if disabled

#else 

/************************************************************************************************
 * PWM Data
 ************************************************************************************************/
static unsigned int pwm_channel = 0;		//selected pwm channel
static unsigned int pwm_status = 0;		//store status of pwm channel, 0=OFF, 1=ON
const unsigned int pwm_prescale[] = {1, 8, 64, 256}; //prescale factors 
static int pwm_io_flag;

/************************************************************************************************
 * PWM local Functions
 ************************************************************************************************/
static int setPeriodNPrescale(unsigned long value_ns);
static unsigned int calcDCycle(unsigned long value_ns);


/************************************************************************************************
 * Name:                int pwm_open(int flags)
 * 
 * Input:               flags: accessing mode
 * 
 * Output:              0: pwm opened
 * 
 * Function:            Initialize PWM
 ************************************************************************************************/
int pwm_open(int flags)
{
    pwm_io_flag = flags;
    OC1CON = 0; OC2CON = 0; //Disable all output compare modules
    OC3CON = 0; OC4CON = 0;
    OC5CON = 0; OC6CON = 0;
    OC7CON = 0; OC8CON = 0;
    //============================================================
    TMR2 = 0;               // Clear register
    PR2 = 0xFFFF;           // Set to Maximum
    //============================================================
    _T2IP = 7;              // Set priority level to 7 (7 Highest)
    _T2IF = 0;              // Clear interrupt flag
    _T2IE = 0;              // Disable interrupts
    //============================================================
    T2CONbits.TCS = 0;      // Use internal clock source
    T2CONbits.TCKPS = 0;    // Prescale Select 1:1
    //============================================================
    T2CONbits.TON = 1;      // Start the timer  
    return 0;
}


/************************************************************************************************
 * Name:                int pwm_write(unsigned long* buf)
 * 
 * Input:               *buf: pointer of data to write to pwm
 * 
 * Output:              number of bytes written
 *                      +-- 4 bytes: (type long)
 *                      +-- -1: an error has occurred
 *                              a) channel has note been enabled (erron = EIO)
 *                              b) pwm not opened for writing (errno = EBADF)
 *                              c) no such channel exist (errno = ENXIO)
 * 
 * Function:            change the duty cycle specified by buf[0] (in ns)
 * 
 ************************************************************************************************/
int pwm_write(unsigned long* buf)
{
    //Perform Write if write operation is enabled
    if(pwm_io_flag & O_RDWR || pwm_io_flag & O_WRONLY)
    {
        if((pwm_status & (0x01 << pwm_channel)) == 0){
            errno = EIO;
            return -1;              //Channel has not been enabled
        }
        switch(pwm_channel){
            case 0:
                OC1RS = calcDCycle(buf[0]); OC1R = OC1RS; 
                OC1CONbits.OCM = 6; //Simple PWM, Fault pin disabled
                break;
            case 1:
                OC2RS = calcDCycle(buf[0]); OC2R = OC2RS; 
                OC2CONbits.OCM = 6; //Simple PWM, Fault pin disabled
                break;
            case 2:
                OC3RS = calcDCycle(buf[0]); OC3R = OC3RS; 
                OC3CONbits.OCM = 6; //Simple PWM, Fault pin disabled
                break;
            case 3:
                OC4RS = calcDCycle(buf[0]); OC4R = OC4RS; 
                OC4CONbits.OCM = 6; //Simple PWM, Fault pin disabled
                TMR2 = 0;
                break;
            case 4:
                OC5RS = calcDCycle(buf[0]); OC5R = OC5RS; 
                OC5CONbits.OCM = 6; //Simple PWM, Fault pin disabled
                break;
            case 5:
                OC6RS = calcDCycle(buf[0]); OC6R = OC6RS; 
                OC6CONbits.OCM = 6; //Simple PWM, Fault pin disabled
                break;
            case 6:
                OC7RS = calcDCycle(buf[0]); OC7R = OC7RS; 
                OC7CONbits.OCM = 6; //Simple PWM, Fault pin disabled
                break;      
            case 7:
                OC8RS = calcDCycle(buf[0]); OC8R = OC8RS; 
                OC8CONbits.OCM = 6; //Simple PWM, Fault pin disabled
                break;  
            default:
                errno = ENXIO;  //no such channel exist
                return -1;
        }
        return 4;        
    }
    //Error, raise error flag
    else
    {
        errno = EBADF;  //io not opened for writing
        return -1;
    }
}

/*******************************************************************************
 * pwm_ioctl
 *******************************************************************************
 * Inputs:
 * 	+--request: Request code - defined in ioctl.h
 * 	+--argp: pointer for control config, request code dependent.
 * 		+--PWM_SET_PERIOD:
 * 			+--argp[0]: set the period for pwm frequency (in ns)
 * 		+--PWM_SELECT_CH:
 * 			+--argp[0]: select the channel to be used
 *******************************************************************************
 * Return:
 * 	on success zero is returned, -1 indicates error
 ********************************************************************************/
int pwm_ioctl(int request, unsigned long* argp){
	unsigned int value;
	unsigned int mask;
	switch(request){
		case PWM_SET_PERIOD:
			return setPeriodNPrescale(argp[0]);
		case PWM_SELECT_CH:
			pwm_channel = (unsigned int) argp[0];
			mask = 0x01 << pwm_channel;
			pwm_status = pwm_status | mask;
			return 0;
		default:
			return -1;
	}
}

/*******************************************************************************
 * setPeriodNPrescale
 *******************************************************************************
 * Function: 	converts ns to register value for timer (with rounding) 
 * 				select the lowest usable prescale factor
 * 				if prescale > 256, an error is generated -1, and no change is done
 * T_pwm = (Reg +1)*T_cy*Prescale
 * Reg = (T_pwm/(T_cy*Prescale))-1
 * Reg = (T_pwm*F_cy)/Prescale -1			;T_pwm in second, F_cy in Hz 
 * Reg = (T_pwm*F_cy)/(1000*Prescale) -1	;T_pwm in ns, F_cy in MHz
 *******************************************************************************
 * Input:
 * 	+--value_ns: pwm_period (in ns) [0, 4,294,967,295]
 *******************************************************************************
 * Return:
 * 	on success, 0
 * 	on error, -1
 ********************************************************************************/
static int setPeriodNPrescale(unsigned long value_ns){
	unsigned long ans;
	unsigned long long numerator = ((unsigned long long)value_ns*(SYSTEM_CLK_HZ/1000000));
	int index= -1;
	unsigned long denominator;
	//-------------------------------------------------
	do{
		denominator = (unsigned long)1000*pwm_prescale[++index];
		ans = (unsigned long)(((long double)numerator/denominator) + 0.5) - 1; //rounding to nearest integer
	} while(ans > 0x0000FFFF && index < 3);
	//-------------------------------------------------
	if(ans > 0x0000FFFF)
		return -1;
	//-------------------------------------------------
	T2CONbits.TON = 0;						// Turn off the timer
	T2CONbits.TCKPS = index;				// Change prescale factor
	PR2 = (unsigned int) ans;				// Set to Maximum
	TMR2 = 0;								// Reset counter
	T2CONbits.TON = 1;						// Turn on the timer 	
	//-------------------------------------------------
	return 0;
}

/*******************************************************************************
 * calcDCycle
 *******************************************************************************
 * Function: 	converts ns to register value for timer (with rounding) 
 * 				if ans > 0xFFFF, return 0xFFFF
 * T_pwm = (Reg +1)*T_cy*Prescale
 * Reg = (T_pwm/(T_cy*Prescale))-1
 * Reg = (T_pwm*F_cy)/Prescale -1			;T_pwm in second, F_cy in Hz 
 * Reg = (T_pwm*F_cy)/(1000*Prescale) -1	;T_pwm in ns, F_cy in MHz
 *******************************************************************************
 * Input:
 * 	+--value_ns: pwm_period (in ns) [0, 4,294,967,295]
 *******************************************************************************
 * Return:
 * 	the corresponding value for timer register
 ********************************************************************************/
static unsigned int calcDCycle(unsigned long value_ns){
	unsigned long long numerator = ((unsigned long long)value_ns*(SYSTEM_CLK_HZ/1000000));
	unsigned int index = T2CONbits.TCKPS;
	unsigned long denominator = (unsigned long)1000*pwm_prescale[index];
	return (unsigned int)(((long double)numerator/denominator) + 0.5) - 1; //rounding to nearest integer
}

#endif //PWM_MOD
