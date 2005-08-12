/*
 * Copyright (c) 24-05-2004 cheng chung yan yan@amonics.com> for 8-bit system
 *
 */

// including, get data linking from others
//==============================================================
//	avr sys
#include <m8c.h> 
#include "PSoCAPI.h"									// PSoC API definitions for all User Modules

// data
//==============================================================

// methods
//==============================================================

void eeprom_init(void)
{
	E2PROM_1_Start();
}

unsigned char eeprom_write(void *buf, unsigned char addr, unsigned char n)
{
	return E2PROM_1_bE2Write(addr, buf, n, 25);
}

unsigned char eeprom_read(void *buf, unsigned char addr, unsigned char n)
{
	E2PROM_1_E2Read(addr, buf, n);
	return 0;
}
