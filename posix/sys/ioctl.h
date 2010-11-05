/**
 * \addtogroup posix POSIX
 * @{
 * 
 * Implementation of POSIX API using FreeRTOS API
 */

/**
 * \defgroup ioctl IO Control for System Call
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
 * UART Module
 */
#ifdef UART_MOD
#define UART_SET_BAUDRATE                      0x00
#define UART_GET_BAUDRATE                      0x01
#endif /* UART_MOD */


/*
 * I2C Module
 */
#ifdef I2C_MOD
/** set status bits */
#define I2C_SET_STATUS                          0
/** send start bit */
#define I2C_START                               0x01
/** send restart bit */
#define I2C_RESTART                             0x02
/** send stop bit */
#define I2C_STOP                                0x04
/** send not acknowledgement */
#define I2C_NACK                                0x08
#endif /* I2C_MOD */


/*
 * I2C DAC Module
 */
#ifdef I2C_DAC_MOD
/** Select channel (A, B, C, or D) */
#define DAC_SET_CTL                             0
#endif /* I2C_DAC_MOD */


/*
 * I2C DSPIC Module
 */
#ifdef I2C_MOD_DSPIC_MOD
/** Select channel (A, B, C, or D) */
#define MOD_DSPIC_SET_CTL                       0
#endif /* I2C_DAC_MOD */


/*
 * I2C LED DRIVER Module
 */
#ifdef I2C_LED_DRIVER_MOD
/** select the channel to dim */
#define I2C_LED_DRIVER_CH_DIM                   0
/** select the channel to on/off */
#define I2C_LED_DRIVER_CH_ON                    1
#endif /* I2C_LED_DRIVER_MOD */


/*
 * I2C GPIO Module
 */
#ifdef I2C_GPIO_MOD
/** select the channel */
#define I2C_GPIO_SET_CH                         0
/** select the mode */
#define I2C_GPIO_SET_MODE                       1
#endif /* I2C_GPIO_MOD */


/*
 * ADC Module
 */
#ifdef ADC_MOD
/** add a channel to current selection */
#define ADC_ADD_CH                              0
#endif /* ADC_MOD */


/*
 * PWM Module
 */
#ifdef PWM_MOD
/** select channel to use */
#define PWM_SELECT_CH                           0
/** set PWM period */
#define PWM_SET_PERIOD                          1
#endif /* PWM_MOD */


/*
 * LED Module
 */
#ifdef LED_MOD
/** select channel to use */
#define LED_SELECT_CH                           0
#ifdef BUZZER
#define BUZZER_CH                               LED_NUM
#endif /* BUZZER */
/** select channel to block */
#define LED_BLOCK_CH                            1
/** set pulse period */
#define LED_SET_PULSE_PERIOD                    2
#endif /* LED_MOD */


/*
 * GPDI Module
 */
#ifdef GPDI_MOD
#define GPDI_SELECT_CH                          0
#endif /* GPDI_MOD */


/*
 * LCD Module
 */
#ifdef LCD_MOD
/** change contrast level */
#define LCD_SET_CONTRAST                        0
/** change brightness level */
#define LCD_SET_BRIGHTNESS                      1
/** refresh LCD screen */
#define LCD_REFRESH                             2
#endif /* LCD_MOD */


#endif /*_IOCTL_H_*/

/** @} */
/** @} */
