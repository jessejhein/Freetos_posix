/*
 * Copyright (c) 12-05-2004 cheng chung yan yan@amonics.com> / <chungyan5@hkem.com for 8-bit system
 *
 * this module is driver specification for keyboard. So each hardware configuration has different code here
 * hardware setting under Laser Control branchtop circuit by PSoC
 * os under linlike8, and provide service to mwlike8
 *
 * header of kb.h
 *
 * using cypress psoc gpio
 * rotary pin A : falling edge
 * for keyboard scanning and ...
 *
 */

#include "m8c.h"

	// psoc configuration, pull-up, change from last for interrupt
#define ENTER_KEY_SET_HIGH			PRT3DR |= 0x02          // high P3.1
#define ENTER_KEY_DET				(PRT3DR & 0x02)
//#define ENTER_KEY_INTR_EN			do {		\
//		PRT3IE |= 0x02;					\
//		PRT3IC0 |= 0x02;				\
//		PRT3IC1 &= ~0x02;				\
//	} while (0)
//#define ENTER_KEY_INTR_DIS			do {		\
//		PRT3IE &= ~0x02;				\
//		PRT3IC0 &= ~0x02;				\
//		PRT3IC1 &= ~0x02;				\
//	} while (0)
#define UPPER_KEY_SET_HIGH			PRT5DR |= 0x08          // high P5.3
#define UPPER_KEY_DET				(PRT5DR & 0x08)
#define UPPER_KEY_INTR_EN			PRT5IE |= 0x08;
#define UPPER_KEY_INTR_FALLING			do {		\
		PRT5IC0 |= 0x08;				\
		PRT5IC1 &= ~0x08;				\
	} while (0)
#define UPPER_KEY_INTR_RISING			do {		\
		PRT5IC0 &= ~0x08;				\
		PRT5IC1 |= 0x08;				\
	} while (0)
#define UPPER_KEY_INTR_DIS			do {		\
		PRT5IE &= ~0x08;				\
		PRT5IC0 &= ~0x08;				\
		PRT5IC1 &= ~0x08;				\
	} while (0)
#define UPPER_KEY_INTR_DET			(PRT5IE & 0x08)
#define DOWN_KEY_SET_HIGH			PRT3DR |= 0x08          // high P3.3
#define DOWN_KEY_DET				(PRT3DR & 0x08)
#define DOWN_KEY_INTR_DET			(PRT3IE & 0x08)
#define DOWN_KEY_INTR_FALLING			do {		\
		PRT3IC0 |= 0x08;				\
		PRT3IC1 &= ~0x08;				\
	} while (0)
#define DOWN_KEY_INTR_RISING			do {		\
		PRT3IC0 &= ~0x08;				\
		PRT3IC1 |= 0x08;				\
	} while (0)
#define DOWN_KEY_INTR_EN			PRT3IE |= 0x08
#define DOWN_KEY_INTR_DIS			do {		\
		PRT3IE &= ~0x08;				\
		PRT3IC0 &= ~0x08;				\
		PRT3IC1 &= ~0x08;				\
	} while (0)

