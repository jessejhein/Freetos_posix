	.module setup.c
	.area text(rom, con, rel)
	.dbfile ./setup.c
	.dbfunc e setup_arch _setup_arch fV
_setup_arch::
	.dbline -1
	.dbline 22
; /*
;  *	arch/cypress_psoc/kernel/setup.c
;  *
;  *	09-04-2004	yan	create this following linux, init. this arch 
;  *
;  */
; 
; #include "app.h"
; 
; extern void gpio_open(void);
; #if (SERIAL_MOD==1)
; extern void serial_open(void);
; #endif
; #if (I2C_MOD==1)
; extern void i2c_open(void);
; #endif
; //#if (KB_MOD==1)
; //extern void kb_open(void);
; //#endif
; 
; void setup_arch(void)
; {
	.dbline 23
; 	gpio_open();
	xcall _gpio_open
	.dbline 28
; //#if (KB_MOD==1)
; //	kb_open();
; //#endif
; #if (SERIAL_MOD==1)
; 	serial_open();
	xcall _serial_open
	.dbline -2
	.dbline 34
; #endif
; #if (I2C_MOD==1)
; 	i2c_open();
; #endif
; 
; }
L1:
	.dbline 0 ; func end
	ret
	.dbend
