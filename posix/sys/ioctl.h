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
#define GPIO_SET_TRISA                          0x00
#define GPIO_SET_TRISB                          0x01
#define GPIO_SET_ODA                            0x02
#define GPIO_SET_ODB                            0x03
#define GPIO_SET_LATA                           0x04
#define GPIO_SET_LATB                           0x05
#define GPIO_SET_PORTA                          0x06
#define GPIO_SET_PORTB                          0x07
#define GPIO_SET_GPPUA                          0x08
#define GPIO_SET_GPPUB                          0x09
//
#define GPIO_GET_TRISA                          0x80
#define GPIO_GET_TRISB                          0x81
#define GPIO_GET_ODA                            0x82
#define GPIO_GET_ODB                            0x83
#define GPIO_GET_LATA                           0x84
#define GPIO_GET_LATB                           0x85
#define GPIO_GET_PORTA                          0x86
#define GPIO_GET_PORTB                          0x87
#define GPIO_GET_GPPUA                          0x88
#define GPIO_GET_GPPUB                          0x89
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
 * LCD Module
 */
#ifdef LCD_MOD
/** change contrast level */
#define LCD_SET_CONTRAST                        0
/** change brightness level */
#define LCD_SET_BRIGHTNESS                      1
/** refresh LCD screen */
#define LCD_REFRESH                             2
/** enable cursor */
#define LCD_CURSOR                              3
/** enable blinking cursor */
#define LCD_BLINK_CURSOR                        4
#endif /* LCD_MOD */


#endif /*_IOCTL_H_*/

/** @} */
/** @} */
