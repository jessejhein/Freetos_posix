/*
 * linlike8/arch/arv/boot/boot.c
 * 24th June 2005	yan	create this following linux
 *
 * When Power ON, and system init. from compiler, then come here
 *
 */

extern void start_kernel(void);

int main(void);

void delay_loop(void)
{
	unsigned int i;
	for (i=0;i<60000;i++);
}

int main()
{
// cpu hardware init
	delay_loop();								// wait power and whole cpu to stable
	
// goto init kernel
//	need to improve to reduce stack usage, replaced by ljmp
	start_kernel();					
	return 0;
}
