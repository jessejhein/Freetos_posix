	.module main.c
	.area text(rom, con, rel)
	.dbfile ./main.c
	.dbfunc e main _main fV
_main::
	.dbline -1
	.dbline 10
; /*
;  *	linlike8/arch/cypress_psoc/boot/boot.c
;  *
;  *	13-04-2004	yan	create this following linux
;  *
;  *	hardware driver in cpu dependent
;  */
; 
; void main()
; {
	.dbline 13
; 		// cpu hardware init
; 		// goto init kernel
; 	asm("ljmp _start_kernel");//start_kernel();					
		ljmp _start_kernel

	.dbline -2
	.dbline 15
; 	//for (;;);									// main should never return here, but just in case, we know what happens.
; }
L1:
	.dbline 0 ; func end
	ret
	.dbend
