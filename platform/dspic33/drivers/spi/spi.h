/**
 * \addtogroup drivers Drivers
 * @{
 *
 * Implementation of Drivers for SPI
 */

/**
 * \defgroup spi SPI Driver
 * @{
 */

/**
 * \file
 * SPI Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

#ifndef SPI_H_
#define SPI_H_


/**
 * \brief Initialise and enable the SPI module as Master
 * \remarks default clock speed is set to 416.67kHz
 */
extern void spi_initialize (void);


/**
 * \brief Set the SPI clock speed
 * \param pri_prescale Primary prescale (see datasheet for proper value)
 * \param sec_prescale Secondary prescale (see datasheet for proper value)
 */
extern void spi_set_clk_speed (int pri_prescale, int sec_prescale);


/**
 * \brief Send a single byte over the SPI port
 * \param input data byte to be sent
 */
extern void spi_send_byte (__u8 input);


/**
 * \brief Receive a byte
 * \return byte received
 * \remarks A byte is received when the module send an 0xFF (the bus idles high)
 */
extern __u8 spi_rcv_byte (void);


/**
 * \brief Assert the CS signal, active low (CS=0)
 */
extern void spi_cs_assert (void);


/**
 * \brief Deassert the CS signal, active low (CS=0)
 */
extern void spi_cs_deassert (void);


/**
 * \brief Disable the SPI module. This function assumes the module had already been initialised.
 */
extern void spi_disable (void);

/**
 * \brief Enable the SPI module. This function assumes the module had already been initialised.
 */
extern void spi_enable (void);


#endif /* SPI_H_ */
