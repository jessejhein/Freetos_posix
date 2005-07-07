/*
 * Copyright (c) 24-05-2004 cheng chung yan yan@amonics.com> for 8-bit system
 *
 */

// methods
//==============================================================

//	init
extern void eeprom_init(void);

// return 
//	0 -- completed
//	non- 0, busy
extern unsigned char eeprom_write(void *buf, void *addr, unsigned char n);

extern unsigned char eeprom_read(void *buf, void *addr, unsigned char n);
