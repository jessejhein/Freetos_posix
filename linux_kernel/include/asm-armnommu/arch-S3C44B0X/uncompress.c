/*
 * linux/include/asm/arch-samsung/uncompress.c
 * 2001 Mac Wang <mac@os.nctu.edu.tw> 
 */

#include <asm/hardware.h>

#define	outb(d, p)	*(volatile unsigned char *)(p) = (d)
#define	inb(p)		*(volatile unsigned char *)(p)

static int s3c44b0x_decomp_setup()
{
}

static int s3c44b0x_putc(char c)
{
	outb(c, S3C44B0X_UTXH0);
	while(!(inb(S3C44B0X_UTRSTAT0) & 2));

	if(c == '\n')
		s3c44b0x_putc('\r');
}

static void s3c44b0x_puts(const char *s)
{
	while(*s != '\0')
		s3c44b0x_putc(*s++);
}
