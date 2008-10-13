/**
 * \addtogroup posix POSIX
 * @{
 * 
 * Implementation of POSIX API using FreeRTOS API
 */

/**
 * \defgroup ioctrl IO Control for System Call
 * @{
 * 
 * Definition of $request codes for System Calls
 */

/**
 * \file
 * IO Control
 * \author Dennis Tsang <dennis@amonics.com>
 */

#ifndef _IOCTL_H_
#define _IOCTL_H_

/*
 * I2C Module
 */
#ifdef I2C_MOD
/** set status bits */
#define I2C_SET_STATUS      0
/** send start bit */
#define I2C_START           0x01
/** send restart bit */
#define I2C_RESTART         0x02
/** send stop bit */
#define I2C_STOP            0x04
/** send not acknowledgment */
#define I2C_NACK            0x08

/** Select channel (A, B, C, or D) */
#define DAC_SET_CTL         0
#endif /* I2C_MOD */


/*
 * ADC Module
 */
#ifdef ADC_MOD
/** add a channel to current selection */
#define ADC_ADD_CH          0
#endif /* ADC_MOD */


/*
 * PWM Module
 */
#ifdef PWM_MOD
/** select channel to use */
#define PWM_SELECT_CH       0
/** set pwm period */
#define PWM_SET_PERIOD      1	
#endif /* PWM_MOD */


/*
 * LED Module
 */
#ifdef LED_MOD
/** select channel to use */
#define LED_SELECT_CH       0
#ifdef BUZZER
#define BUZZER_CH           LED_NUM                   
#endif /* BUZZER */
#endif /* LED_MOD */


#endif /*_IOCTL_H_*/

/** @} */
/** @} */
