

#include <asm/hardware.h>



static int sput(int c)
{
	/* wait for room in the transmit FIFO */
	while(!((UTRSTAT1)&0x02));

	(UTXH1) = (unsigned char)c;

	return 0;
}

static void sputs(char *s)
{
	int len, i;

        len = strlen(s);
        for(i=0; i<len; i++){
            sput(s[i]);
            if(s[i] == '\n')
                sput('\r');
        }
}

static void shexb(int h)
{
	int dt;

	dt = (h>>4)&0x0f;
	if(dt>9)
		dt += 7;
	dt += 0x30;
	sput(dt);

	dt = (h)&0x0f;
	if(dt>9)
		dt += 7;
	dt += 0x30;
	sput(dt);
}

static void shexw(int w)
{
	shexb(w>>8);
	shexb(w);
}

static void shex(int d)
{
	shexw(d>>16);
	shexw(d);
}
