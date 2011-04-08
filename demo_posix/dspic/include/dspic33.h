/**
 * \addtogroup drivers Drivers
 * @{
 *
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup dspic33 Microchip DsPIC33
 * @{
 */

/**
 * \file
 * DSPIC33 Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

#ifndef DSPIC33_H_
#define DSPIC33_H_

/* Header files for supported dsPic33 devices */
#ifdef FJ128GP306
#include <p33FJ128GP306.h>
#endif /* FJ128GP306 */

#ifdef FJ256GP506
#include <p33FJ256GP506.h>
#endif /* FJ256GP506 */

//-----------------------------------------------------------------
#include <asm/types.h>
#include <asm/delay.h>
#include <asm/system.h>

#endif /* DSPIC33_H_ */

/** @} */
/** @} */
