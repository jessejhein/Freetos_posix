/*
 *	linlike8/arch/cypress_psoc/boot/boot.c
 *
 *	13-04-2004	yan	create this following linux
 *
 *	hardware driver in cpu dependent
 */

void main()
{
		// cpu hardware init
		// goto init kernel
	asm("ljmp _start_kernel");//start_kernel();					
	//for (;;);									// main should never return here, but just in case, we know what happens.
}

