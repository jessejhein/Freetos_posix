/**
 * \addtogroup posix POSIX
 * @{
 * 
 * Implementation of POSIX API using FreeRTOS API
 */

/**
 * \defgroup types Data Types
 * @{
 * 
 * POSIX Data Types
 */

/**
 * \file
 * POSIX Data Types
 * \author Dennis Tsang <dennis@amonics.com>
 */

#ifndef TYPES_H_
#define TYPES_H_    1

#include <FreeRTOS.h>
#include <dsp.h>

// ---------------------------------------------------------------------------------------
/** unsigned 8-bit data */
typedef unsigned char           __u8;
/** 8-bit data */
typedef char                    __s8;
/** unsigned 16-bit data */
typedef unsigned int            __u16;
/** 16-bit data */
typedef int                     __s16;
/** unsigned 32-bit data */
typedef unsigned long           __u32;
/** 32-bit data */
typedef long                    __s32;
/** unsigned 64-bit data */
typedef unsigned long long      __u64;
/** 64-bit data */
typedef long long               __s64;


// ---------------------------------------------------------------------------------------
/** Data type for time for <time.h> */
typedef unsigned long           time_t;
/** Data type for clock ticks for <time.h> */
#if configUSE_16_BIT_TICKS
typedef unsigned int            clock_t;
#else  /* NOT configUSE_16_BIT_TICKS */
typedef unsigned long           clock_t;
#endif /* NOT configUSE_16_BIT_TICKS */


// ---------------------------------------------------------------------------------------
//Data type for complex number for <fft.h> : not POSIX
/** complex numbers */
typedef fractcomplex            complex_t;
/** real numbers */
typedef fractional              real_t;
/** convert real data to floating point data */
#define REAL2FLOAT(N)           Fract2Float(N)
/** convert floating point data to real data */
#define FLOAT2REAL(N)           Float2Fract(N)


// ---------------------------------------------------------------------------------------
/** Interrupt Request */
#define _IRQ                    __attribute__((__interrupt__))

/** TRAP IRQ for saving program counter: declare __u16 StkAddrLo, StkAddrHi in trap.c (order matters) */
#define _TRAP_IRQ               __attribute__((__interrupt__(__preprologue__( \
                                  "mov #_StkAddrHi,w1\n\tpop [w1--]\n\tpop [w1++]\n\tpush [w1--]\n\tpush [w1++]"))))

/** IO Stub Functions are placed in .libc section so that the standard libraries can access these functions using short jumps. */
#define _LIBC                   __attribute__((section(".libc")))

/** FAST RAM */
#define _DMA                    __attribute__((space(dma), aligned(256)))

/** Aligned Data in Program Memory */
#define _PROG(N)                __attribute__ ((space(auto_psv), aligned(N)))

/** Aligned Data in Y Memory */
#define _YMEM(N)                __attribute__ ((section("data, ymemory"), aligned(N)))
#endif /*TYPES_H_*/

/** @} */
/** @} */
