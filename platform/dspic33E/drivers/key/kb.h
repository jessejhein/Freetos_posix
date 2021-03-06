/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup kb Keyboard
 * @{
 */

/**
 * \file
 * Keyboard Driver
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

#ifndef KB_H_
#define KB_H_

#ifdef KB_MOD

/**
 * \brief get keyboard port ready
 * \param flags accessing mode
 * \retval -1 error write mode is selected (errno = EROFS)
 * \retval 0 OK
 */
extern int kb_open (int flags);


/**
 * \brief copy the received data to buffer
 * \param buf pointer to buffer for reading
 * \retval -1 error not opened for read operation (errno = EBADF)
 * \retval 0 indicating no data is available
 * \retval 1 indicating 1 byte has been read
 */
extern int kb_read (__u8* buf);


#ifdef KB_PUSH_KEY
/**
 * \brief detect push key
 * \remarks used in idle task
 */
extern void kb_check_push_key (void);
#endif /* KB_PUSH_KEY */


#ifdef KB_FN_KEY
/**
 * \brief detect push key
 * \remarks used in idle task
 */
extern void kb_check_fn_key (void);
#endif /* KB_FN_KEY */


/**
 * \brief configure pin for all keys
 * \remarks implement externally
 */
extern void kb_config (void);


/**
 * \brief determine if a key has been pressed (push key)
 * \param id key id
 * \retval 1 key is being pressed
 * \retval 0 key is released
 * \remarks implement externally
 */
extern int kb_key_press (unsigned char id);


#ifdef KB_ROTATE_KEY
/**
 * \brief Check the state of rotary pin
 * \param id key id
 * \retval 1 key is at high state
 * \retval 0 key is at low state
 * \remarks implement externally
 */
extern int kb_rkey_state (unsigned char id);
#endif /* KB_ROTATE_KEY */
#endif /* KB_MOD */

#endif /* KB_H_ */

/** @} */
/** @} */
