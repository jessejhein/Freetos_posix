/*
 * linlike8/arch/arv/boot/boot.c
 * 24th June 2005	yan	create this following linux
 *
 * When Power ON, and system init. from compiler, then come here
 *
 */

extern void start_kernel(void);

void main(void);

void main()
{
// cpu hardware init

// goto init kernel
//	need to improve to reduce stack usage, replaced by ljmp
	start_kernel();					
}
