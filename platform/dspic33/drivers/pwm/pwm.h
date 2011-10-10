/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup pwm PWM
 * @{
 * 
 * Control PWM channels output
 * \li The PWM module consists of 8 channels using the output compare module of dsPic.
 * \li The driver has a POSIX-compliance interface with open(), write(), and ioctl().
 * \li The range of PWM frequencies obtainable is 2Hz to 5MHz. Suggested range of operation
 *     is 2Hz to 1MHz. 
 * \li All PWM channels share the same timer (Timer 2)
 * \li When changing the PWM Period using ioctl(), all channels will be affected.
 * \li At steady state, all channels will be triggered simultaneously (all rise to HIGH at
 *     the same time)
 * \li Accuracy on PWM period and duty cycle timings is not guaranteed. Instability may
 *     arise due to rise/fall times and resolution of timer, which becomes critical for 
 *     high frequency PWMs.
 * \li There is a time delay between write() and actual change of PWM pulses. Change will
 *     take effect on the next timer cycle.
 * \li A higher PWM frequency means a coarser resolution of duty cycle (See Table Below)
 * \li When incorporating with FreeRTOS, the kernel clock may impose limitation to the 
 *     stability of high frequency PWM (since the kernel disable all interrupt at each kernel
 *     clock tick). Please evaluate the trade-off between PWM frequency range and kernel clock
 *     configuration.
 * \verbatim
   Rough estimate of relationship between pwm frequency and resolution of duty cycle
   +------------------------------+-----+----+------+------+------+
   | Frequency (Hz)               | 5M  | 1M | 500k | 200k | <20k |
   +------------------------------+-----+----+------+------+------+
   | Resolution of Duty Cycle (%) | 25  |  6 |   3  |  0.8 | >0.1 |
   +------------------------------+-----+----+------+------+------+
   \endverbatim
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

#ifndef PWM_H_
#define PWM_H_

#ifdef PWM_MOD

/**
 * \brief Initialise PWM Ports
 * \param flags accessing mode
 * \retval 0 PWM opened
 */
extern int pwm_open (int flags);


/**
 * \brief change the duty cycle (in ns)
 * \param buf pointer of data to write to PWM
 * \n         note that if *buf = 0, the entire period will be ON state, i.e. duty cycle = 100%
 * \retval >=0 number of bytes written
 * \retval -1 error
 * \n         errno = EIO: channel has note been enabled
 * \n         errno = EBADF: PWM not opened for writing
 * \n         errno = ENXIO: no such channel exist
 */
extern int pwm_write (__u32* buf);


/**
 * \brief set the local parameters of PWM
 * \param request Request code - defined in ioctl.h
 * \param argp pointer for control configuration
 * \n          PWM_SET_PERIOD: set the period for PWM frequency (in ns)
 * \n          PWM_SELECT_CH: select the channel to be used
 * \retval -1 failure
 * \retval 0 success
 */ 
extern int pwm_ioctl (int request, void* argp);

#endif /* PWM_MOD */

#endif /* PWM_H_ */

/** @} */
/** @} */
