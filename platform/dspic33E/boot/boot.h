/**
 * \addtogroup drivers Drivers
 * @{
 *
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup io_bus IO_BUS
 * @{
 *
 * IO BUS driver
 */

/**
 * \file
 * IO BUS Driver
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

#ifndef BOOT_H_
#define BOOT_H_


//  =========================ADDRESS BUS=================================
/**
 * \brief Initialise address bus
 */
extern void bus_addr_init (void);


/**
 * \brief set address bus
 * \param address address to set
 */
extern void bus_set_addr (__u16 address);



// ============================IO BUS===================================
/**
 * \brief initialise IO bus
 */
extern void bus_io_init (void);


/**
 * \brief set IO bus
 * \param write write/read mode
 */
extern void bus_set_io_write (int write);



//  ===========================DATA BUS===================================
/**
 * \brief configure data port pins to output or input
 * \param input 0: output 1: input
 */
extern void bus_data_config (int input);


/**
 * \brief write a byte to data port
 * \param data 8-bit data
 */
extern void bus_data_write (__u8 data);


/**
 * \brief read a byte from data port
 * \return 8-bit data
 */
extern __u8 bus_data_read (void);


#endif /* BOOT_H_ */

/** @} */
/** @} */
