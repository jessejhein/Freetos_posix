/*
 * Copyright (c) 24-05-2004 cheng chung yan yan@amonics.com> for 8-bit system
 *
 */

// methods
//==============================================================

//	init
extern void eeprom_init(void);

// parameters
//	addr -- offset value inside eeprom
//	buf -- pointer of RAM buf. which copy to their after read fr. eeprom
//	n -- counter
// return 
//	0 -- completed
//	+ve -- busy
//	-ve -- error
// Remarks : no err. handle, so please make sure all parameter is valid data
extern unsigned char eeprom_write(void *buf, unsigned char addr, unsigned char n);
extern unsigned char eeprom_read(void *buf, unsigned char addr, unsigned char n);
