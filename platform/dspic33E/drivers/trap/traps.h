/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup trap fatal traps
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

#ifndef TRAPS_H_
#define TRAPS_H_

//Uncomment this to enable address trap debug coding
//#define DEBUG_ADDR_TRAP                         1


/**
 * \brief user routine to handle address trap
 * \remarks for example switch off laser etc.
 * \remarks implement externally
 */
extern void handle_address_trap (void);


#ifdef DEBUG_ADDR_TRAP
/**
 * \brief routine to create address trap
 * \remarks USE ONLY FOR DEBUG
 */
extern void create_address_trap (void);
#endif /* DEBUG_ADDR_TRAP */

#endif /* TRAPS_H_ */

/** @} */
/** @} */
