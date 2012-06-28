/**
 * \addtogroup drivers Drivers
 * @{
 *
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup lpt LPT
 * @{
 */

/**
 * \file
 * Parallel Port Driver
 * \author Hui Ning, Sam <sam_hui@amonics.com>
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

#ifndef LPT_H_
#define LPT_H_

#ifdef LPT_MOD

/**
 * \brief Initialise Parallel Ports
 * \param flags accessing mode
 * \retval 0 LPT opened
 */
extern int lpt_open (int flags);


/**
 * \brief copy count bytes of the data from buffer and transmit
 * \param buf pointer to buffer for writing
 * \param count number of bytes to write
 * \retval >=0 the number of bytes written
 * \retval -1 error
 * \n errno = EBADF: LPT is not opened for writing
 */
extern int lpt_write (__u8 *buf, __u16 count);

/**
 * \brief copy the received data to buffer
 * \param buf pointer to buffer for reading
 * \param count max. number of bytes to read
 * \retval number of bytes read
 * \retval -1 error, LPT is not opened for read operation (errno = EBADF)
 */
extern int lpt_read (__u8 *buf, __u16 count);


/**
 * \brief close Parallel Port
 * \retval 0 success
 * \retval -1 error
 */
extern int lpt_close (void);


/**
 * \brief To configure the INT pin for LPT
 * \param set_int INT pin state to set
 * \n     1 - Set the INT pin as output pin and trigger an interrupt
 * \n     0 - Reset the INT pin as input pin and wait for interrupt
 */
extern void lpt_set_interrupt_pin (int set_int);

#endif /* LPT_MOD */

#endif /* LPT_H_ */

/** @} */
/** @} */
