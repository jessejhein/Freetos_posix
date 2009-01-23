/**
 * \file
 * led task employing posix thread and file API
 * \author Dennis Tsang <dennis@amonics.com>
 */

#ifndef LED_H_
#define LED_H_

/**
 * \brief toggle led at a specified rate
 * \param ptr [0]: led id
 * \n         [1]: 50% duty cycle (in sec)
 * \return never return
 */
extern void* tskFlashLED(void *ptr);


#endif /* LED_H_ */
