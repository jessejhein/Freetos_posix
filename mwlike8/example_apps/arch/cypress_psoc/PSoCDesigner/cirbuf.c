/*	Copyright (C) 2004 cheng chung yan <yan@amonics.com> for 8-bit system linlike8
 *	lib C in linlike8 8 bit system
 *
 *	cirbuf.c
 *
 *	Design Concept
 *		Make a circular buffer template.
 *		this is only a template, not for real code to work, please implement this code into your application area.
 *
 *			| wr ptr
 *		top	V			       bottom
 *		|<------------------------------------>| buffer		wr and rd ptr, they both must go upward, then circular
 *				^					wr ptr must go prior rd
 *				| rd ptr
 *
 *  Testing Procedures
 *	before you compile, you must copy whole directory of linlike8/ to your local home
 *	compile this file in linux gcc environment : " make cirbuf_to_run.o"
 *	testing by : "main_compile_dir/cirbuf_to_run.o"
 *
 */

unsigned char pre_wr_cir254buf(unsigned char wr, unsigned char rd, unsigned char max)
{
	unsigned char wr_backup = wr + 1;
	if (wr_backup==max) wr_backup = 0;
	if (wr_backup==rd) return 255;
	else return wr_backup;
}

unsigned char pre_rd_cir254buf(unsigned char wr, unsigned char rd, unsigned char max)
{
	unsigned char rd_backup;
	if (rd==wr) return 255;
	else {
		rd_backup = rd + 1;
		if (rd_backup==max) rd_backup = 0;
		return rd_backup;
	}
}

#ifdef	TEST
#include <stdio.h>									// in I386 linux platform
int main(void)
{
	#define	MAX_BUF	9		// MAX_BUF must under 255
	char vec[MAX_BUF];
	char ans;
	unsigned char wr = 0;
	unsigned char rd = 0;
	unsigned char i,j,k;
	
	for (j=0;j<3;j++) {
		for (k=0;k<7;k++) {
			if ((i=pre_wr_cir254buf(wr,rd,MAX_BUF))!=255) {
				vec[wr] = '0' + k + j;
				wr = i;
			}
		}
		while ((i=pre_rd_cir254buf(wr,rd,MAX_BUF))!=255) {
			ans = vec[rd];
			rd = i;
			printf("%c\n",ans);
		}
	}
	return 0;
}
#endif

