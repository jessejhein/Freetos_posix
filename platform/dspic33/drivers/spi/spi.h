/**
 * \addtogroup drivers Drivers
 * @{
 *
 * Implementation of Drivers for SPI
 */

/**
 * \defgroup spi SPI Driver
 * @{
 *
 * The SPI driver provide an interface for SPI devices (e.g. MMC card)
 */

/**
 * \file
 * SPI Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

/*
 * Copyright (C) 2012  Dennis Tsang <dennis@amonics.com>
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

#ifndef SPI_H_
#define SPI_H_


//Primary Prescale
#define PPRE_1_1        3
#define PPRE_4_1        2
#define PPRE_16_1       1
#define PPRE_64_1       0


//Secondary Prescale
#define SPRE_1_1        7
#define SPRE_2_1        6
#define SPRE_3_1        5
#define SPRE_4_1        4
#define SPRE_5_1        3
#define SPRE_6_1        2
#define SPRE_7_1        1
#define SPRE_8_1        0


/**
 * \brief Initialise and enable the SPI module as Master
 * \remarks default clock speed is set to about 400kHz
 */
extern void spi_open (void);


/**
 * \brief Disable the SPI module
 */
extern void spi_close (void);


/**
 * \brief Set SPI clock speed
 * \param speed speed in Hz
 * \return actual speed in Hz
 */
extern __u32 spi_set_clk_speed (__u32 speed);


/**
 * \brief Exchange a byte over the SPI port
 * \param data data to be sent
 * \return data received from SPI slave
 * \remarks for MMC card operation. MMC card will return a byte whenever a byte is sent
 */
extern __u8 spi_xchg (__u8 data);


/**
 * \brief Send multiple bytes over the SPI port
 * \param buf pointer to buffer for writing
 * \param count number of bytes to write
 * \return the number of bytes written
 */
extern int spi_write (const __u8 *buf, __u16 count);


/**
 * \brief Read multiple bytes over the SPI port
 * \param buf pointer to buffer for reading
 * \param count number of bytes to read
 * \return the number of bytes read
 */
extern int spi_read (__u8 *buf, __u16 count);


/**
 * \brief Assert the CS signal
 */
extern void spi_cs_assert (void);


/**
 * \brief Deassert the CS signal
 */
extern void spi_cs_deassert (void);



#endif /* SPI_H_ */

/** @} */
/** @} */
