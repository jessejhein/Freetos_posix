/**
 * \file
 * emulate control LED for module board (non-POSIX compliant)
 * \author Dennis Tsang <dennis@amonics.com>
 */

#ifndef PIN_DEFINE_H_
#define PIN_DEFINE_H_

/**
 * \brief turn on led
 * \param led number
 */
extern void led_on (unsigned short led);


/**
 * \brief turn off led
 * \param led number
 */
extern void led_off (unsigned short led);


/**
 * \brief read status of LED
 * \param led number
 * \retval -1 error
 */
extern int led_status (unsigned short led);


/**
 * \brief initialise led
 */
extern void led_init (void);


#endif /* PIN_DEFINE_H_ */
