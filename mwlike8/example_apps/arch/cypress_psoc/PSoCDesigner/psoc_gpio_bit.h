/*
 * Copyright (c) 12-05-2004 cheng chung yan <chungyan5@hkem.com / yan@amonics.com> for 8-bit system
 *
 * header of psoc_gpio_bit.c
 *
 * PSoC hw configuration
 *	buzzer -- pull-down
 *	led -- pull-up
 *
 */

#include "m8c.h"

#define buz_hw_on		PRT2DR |= 0x08						// high active P2.3, pull-down
#define buz_hw_off		PRT2DR &= ~0x08

#define led_hw_off		PRT2DR |= 0x80						// low active P2.7, pull-up
#define led_hw_on		PRT2DR &= ~0x80

