/************************************************************************************************
 * File: 			ioctl.h
 * Description:		definition of $request codes
 ***********************************************************************************************/

#ifndef _IOCTL_H_
#define _IOCTL_H_

#include <define.h>

#if (I2C_MOD>0)
//=============Request Code Definition: For io control=========================
#define I2C_SET_STATUS			0			//set status bits
#define I2C_START				0x01		//send start bit
#define I2C_RESTART				0x02		//send restart bit
#define I2C_STOP				0x04		//send stop bit
#define I2C_NACK				0x08		//send not acknowledgment
 
//=============Request Code Definition: For io control=========================
#define	 DAC_SET_CTL			0			//Select channel (A, B, C, or D)
#endif //I2C_MOD

#if (ADC_MOD>0)
//=============Request Code Definition: For io control=========================
#define ADC_ADD_CH			0			//add a channel to current selection			
#define ADC_RM_CH			1			//remove a channel from current selection
#endif //ADC_MOD

#if (PWM_MOD>0)
//=============Request Code Definition: For io control=========================
#define PWM_SELECT_CH		0			//select channel to use			
#define PWM_SET_PERIOD		1			//set pwm period		

#endif //ADC_MOD

#endif /*_IOCTL_H_*/

