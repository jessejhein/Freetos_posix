/*
 * Copyright (c) 24-05-2004 cheng chung yan yan@amonics.com> for 8-bit system
 *
 */

// including, get data linking from others
//==============================================================
//	avr sys
#include <avr/eeprom.h> 

// data
//==============================================================
struct {
	unsigned write : 2;
	unsigned read : 2;
	} eeprom_status;

// methods
//==============================================================

void eeprom_init(void)
{
	eeprom_status.write = 0;
	eeprom_status.read = 0;
}

// return 
//	0 -- completed
//	non- 0, busy
unsigned char eeprom_write(void *buf, void *addr, unsigned char n)
{
	switch (eeprom_status.write) {
		case 0 : 
			eeprom_status.write = 1;
			break;
		case 1 : 
			if (eeprom_is_ready()) {
				eeprom_write_block(buf, addr, n);
				eeprom_status.write = 0;
			}
			break;
	}
	return eeprom_status.write;
}

unsigned char eeprom_read(void *buf, void *addr, unsigned char n)
{
	switch (eeprom_status.read) {
		case 0 : 
			eeprom_status.read = 1;
			break;
		case 1 : 
			if (eeprom_is_ready()) eeprom_status.read = 2;
			break;
		case 2 : 
			eeprom_read_block(buf, addr, n);
			eeprom_status.read = 0;
			break;
	}
	return eeprom_status.read;
}
