/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup pwm PWM
 * @{
 */

/**
 * \file
 * PWM Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

/*
 * Copyright (C) 2007-2011  Dennis Tsang <dennis@amonics.com>
 *
 * This file is part of freertos_posix.
 *
 * freertos_posix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * freertos_posix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with freertos_posix.  If not, see <http://www.gnu.org/licenses/>
 */

#include <define.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

/*
 * Local Variables 
 */
static unsigned int pwm_channel = 0;                  //selected PWM channel
static unsigned int pwm_status = 0;                   //store the bitwise status of PWM channel, 0=OFF, 1=ON
static unsigned int pwm_prescale[] = {1, 8, 64, 256}; //prescale factors
static int pwm_io_flag;

/*
 * Local Functions
 */
static int setPeriodNPrescale (unsigned long value_ns);
static unsigned int calcDCycle (unsigned long value_ns);


int 
pwm_open(int flags)
{
  pwm_io_flag = flags;
  //Disable all output compare modules
  OC1CON = 0; OC2CON = 0;
  OC3CON = 0; OC4CON = 0;
  OC5CON = 0; OC6CON = 0;
  OC7CON = 0; OC8CON = 0;
  //Prepare Timer
  TMR2 = 0;
  PR2 = 0xFFFF;           //100% duty cycle
  _T2IP = 7;
  _T2IF = 0;
  _T2IE = 0;
  T2CONbits.TCS = 0;      // Use internal clock source
  T2CONbits.TCKPS = 0;    // Prescale Select 1:1
  T2CONbits.TON = 1;      // Start the timer  
  return 0;
}


int 
pwm_write (unsigned long* buf)
{
  //Perform Write if write operation is enabled
  if (pwm_io_flag & O_RDWR || pwm_io_flag & O_WRONLY)
    {
      if ((pwm_status & (0x01 << pwm_channel)) == 0)
        {
          errno = EIO;
          return -1;
        }
      switch (pwm_channel)
        {
          case 0:
            OC1RS = calcDCycle (buf[0]);
            OC1R = OC1RS; 
            OC1CONbits.OCM = 6;
            break;
          case 1:
            OC2RS = calcDCycle (buf[0]);
            OC2R = OC2RS; 
            OC2CONbits.OCM = 6;
            break;
          case 2:
            OC3RS = calcDCycle (buf[0]);
            OC3R = OC3RS; 
            OC3CONbits.OCM = 6;
            break;
          case 3:
            OC4RS = calcDCycle (buf[0]);
            OC4R = OC4RS; 
            OC4CONbits.OCM = 6;
            TMR2 = 0;
            break;
          case 4:
            OC5RS = calcDCycle (buf[0]);
            OC5R = OC5RS; 
            OC5CONbits.OCM = 6;
            break;
          case 5:
            OC6RS = calcDCycle (buf[0]);
            OC6R = OC6RS; 
            OC6CONbits.OCM = 6;
            break;
          case 6:
            OC7RS = calcDCycle (buf[0]);
            OC7R = OC7RS; 
            OC7CONbits.OCM = 6;
            break;      
          case 7:
            OC8RS = calcDCycle (buf[0]);
            OC8R = OC8RS; 
            OC8CONbits.OCM = 6;
            break;  
          default:
            errno = ENXIO;
            return -1;
        }
      return sizeof (unsigned long);
    }
  //Error, raise error flag
  else
    {
      errno = EBADF;
      return -1;
    }
}


int 
pwm_ioctl (int request, void* argp)
{
  unsigned int value;
  unsigned int mask;
  switch (request)
    {
      case PWM_SET_PERIOD:
        return setPeriodNPrescale (*((unsigned long*)argp));
      case PWM_SELECT_CH:
        pwm_channel = *((unsigned int*)argp);
        mask = 0x01 << pwm_channel;
        pwm_status = pwm_status | mask;
        return 0;
      default:
        return -1;
    }
}

/**
 * \brief converts ns to register value for timer (with rounding)
 * \param value_ns: pwm_period (in ns) [0, 4,294,967,295]
 * \retval 0 success
 * \retval -1 error
 * \remarks select the lowest usable prescale factor
 * \n       if prescale > 256, an error is generated -1, and no change is done
 * \n       Calculations:
 * \n       T_pwm = (Reg +1)*T_cy*Prescale
 * \n       Reg = (T_pwm/(T_cy*Prescale))-1
 * \n       Reg = (T_pwm*F_cy)/Prescale -1          ;T_pwm in second, F_cy in Hz 
 * \n       Reg = (T_pwm*F_cy)/(1000*Prescale) -1   ;T_pwm in ns, F_cy in MHz
 */
static int 
setPeriodNPrescale (unsigned long value_ns)
{
  unsigned long ans;
  unsigned long long numerator = ((unsigned long long)value_ns*(SYSTEM_CLK_HZ/1000000));
  int index= -1;
  unsigned long denominator;
  do
    {
      denominator = (unsigned long)1000*pwm_prescale[++index];
      ans = (unsigned long)(((long double)numerator/denominator) + 0.5) - 1; //rounding to nearest integer
    }
  while(ans > 0x0000FFFF && index < 3);
	
  if(ans > 0x0000FFFF) return -1;

  //Set Timer
  T2CONbits.TON = 0;
  T2CONbits.TCKPS = index;      // Change prescale factor
  PR2 = (unsigned int) ans;     // Set period
  TMR2 = 0;                     // Reset counter
  T2CONbits.TON = 1; 	
  return 0;
}

/**
 * \brief converts ns to register value for timer (with rounding)
 * \param value_ns pwm_period (in ns) [0, 4,294,967,295]
 * \retval the corresponding value for timer register
 * \remarks
 * \n       Calculations:
 * \n       T_pwm = (Reg +1)*T_cy*Prescale
 * \n       Reg = (T_pwm/(T_cy*Prescale))-1
 * \n       Reg = (T_pwm*F_cy)/Prescale -1          ;T_pwm in second, F_cy in Hz 
 * \n       Reg = (T_pwm*F_cy)/(1000*Prescale) -1   ;T_pwm in ns, F_cy in MHz
 */
static unsigned int 
calcDCycle (unsigned long value_ns)
{
  if(value_ns == 0) return 0;
  unsigned long long numerator = ((unsigned long long)value_ns*(SYSTEM_CLK_HZ/1000000));
  unsigned int index = T2CONbits.TCKPS;
  unsigned long denominator = (unsigned long)1000*pwm_prescale[index];
  return (unsigned int)(((long double)numerator/denominator) + 0.5) - 1; //rounding to nearest integer
}

/** @} */
/** @} */
