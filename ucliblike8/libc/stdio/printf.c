/*  Copyright (C) 2004		cheng chung yan	<yan@amonics.com> for 8-bit system
*  lib C in 8 bit system
*
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Library General Public
*  License as published by the Free Software Foundation; either
*  version 2 of the License, or (at your option) any later version.
*
*  This library is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Library General Public License for more details.
*
*  You should have received a copy of the GNU Library General Public
*  License along with this library; if not, write to the Free
*  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*  Design Concept
*	transfer info. to string and place into the buffer which pointed by a char pointer
*	support reentrant
*
*  Testing Procedures
*	before you compile, you must copy whole directory of ucliblike8/ to your local home
*	compile this file in linux gcc environment : "make printf.o" in ucliblike8/
*	testing by : "main_compile_dir/printf.o" in ucliblike8/
*
*  Features
*	printf format specific
*		- as left justify
*		0 as pad zero / empty
*		number as width for display
*	print integer & floating point with
*		left justify
*		pad zero / empty
*		width
*		negative num
*		base 10 only
*	floating point
*		range: -99999.0 to 99999.0
*		Maximum decimal digit range: 5 decimal digit
*		Maximum width range=-9 to 9
*		must entry the width no. of decimal point & the width no.of integer
*
*  Examples
*	print_integer
*		display negative number with zero for empty position and 4 digital width
*			unsigned char vec_buf[6];
*			print_integer(vec_buf, "04", -45);
*			printf("%s\n",vec_buf);
*		display number with left justify and 5 digital width
*			unsigned char vec_buf[6];
*			print_integer(vec_buf, "-5", 23);
*			printf("%s\n",vec_buf);
*	print_float
*		display negative number with zero for empty position and 5 digital width, 3 decimal points
*			unsigned char vec_buf[13];
*			print_float(vec_buf, "05.3", -13.5);
*			printf("%s\n",vec_buf);
*		display number with left justify and 5 digital width, 3 decimal points
*			unsigned char vec_buf[13];
*			print_float(vec_buf, "-5.2", 23.3);
*			printf("%s\n",vec_buf);
*
*  Pay Attention
*	MUST input a valid format flag, no chk in coding
*	width data must great than the real width of number
*	MUST provide enough buffer (char* out) to hold the char. data, eg. 3456, must provide 5 bytes including '\0'
*
*/

#include "printf.h"

struct format_flag {
	unsigned	pad_left :1 ;							// 0 - right justify, 1 - left justify
	unsigned	pad_zero :1 ;							// 0 - blank space for empty digit, 1 - '0' for empty digit
	unsigned	neg :1 ;							// 0 - +ve number, 1 - '0' - -ve number
	unsigned	width :5 ;							// max. 32 digit placement
#if (FLOAT==1)
//	signed		width :5;
	unsigned	dp :1 ;								// 0 - no decimal, 1 - have decimal

	unsigned	decimal :5 ;							// max. 32 digit placement

	unsigned	fulldec :1 ;							// 0 - no full decimal, 1 - full decimal
	unsigned	add_zero :3 ;							// max 8 digit placement
#endif
};

struct format_flag format_manipulate(const char* flag_ptr)
{
	struct format_flag flag;

	flag.pad_left = 0;
	flag.pad_zero = 0;
	flag.neg = 0;
	flag.width = 0;
#if (FLOAT==1)
	flag.dp = 0;
	flag.decimal = 0;
	flag.fulldec = 0;
	flag.add_zero = 0;
#endif

	for (;*flag_ptr;flag_ptr++) {
		switch (*flag_ptr) {
			case '-' : 							// left justify
				 flag.pad_left = 1;
				break;
			case '0' : 							// pad zero / empty
				 flag.pad_zero = 1;
				break;
#if (FLOAT==1)
			case '.' :
				 flag.dp= 1;
				 flag_ptr++;
				 flag.decimal = *flag_ptr - '0';
				break;
#endif
			default :							// number
				 flag.width = *flag_ptr - '0';
				 // ATTENTION ATTENTION ATTENTION
				 //	this input is programmer job to input a valid number
				 //	we just want to reduce code size and execution speed, so ignore the checking for programming fault, we assume this must be a number
		}
	}


	return flag;
}

#if (INTEGER==1)
void print_i(char* out, struct format_flag flag, int i)
{
	#define	base	10
	char padchar;

#if (FLOAT==1)
	if (flag.fulldec) goto next;
#endif

	*(out + flag.width--) = '\0';							// place a NULL at the end of string

next:	if (i<0) {									// -ve number
		i = -i;
		flag.neg = 1;
	}
	if (i==0) *(out + flag.width--) = '0';						//if i=0

	while (i) {									// convert the integer number to string
		*(out + flag.width--) = (i % base) + '0';
		i /= base;
	}
	if (flag.neg) *(out + flag.width--) = '-';

#if (FLOAT==1)
	for(;flag.add_zero;flag.add_zero--){						// add zero for 1.005,1.05,1.012 etc
		*(out + flag.width--) = '0';
	}

	if (flag.dp) {
	 	*(out + flag.width--) = '.';
		goto complete;
	}
#endif

	if (++flag.width) {								// if not fit to width, place something in the space
											//	since the above ++flag.width, so i must adjust back to right position
//	printf("flagwidth=%d\n",flag.width);						//check point only


		if (flag.pad_left) {							// left justify and width value > real width
			unsigned char j = 0;						//	adding one to flag.width 1st, because
			char tmp;							//	if fit of digial placement, ++flag.width == 0
			do {
				tmp = *(out + flag.width + j);
				*(out + j) = tmp;
				j++;
			} while (tmp);							//	chk '\0'
		} else {
			if ((!flag.neg)||(!flag.pad_zero)) flag.width--;
			if (flag.pad_zero) padchar = '0';				// place zero for empty space and width value > real width
			else padchar = ' ';
			do {
				*(out + flag.width) = padchar;
				if (flag.width) flag.width--;
				else break;
			} while (1);
			if ((flag.neg)&&(flag.pad_zero)) *out = '-';			//	should be *(out + flag.width), but flag.width must be zere
		}
	}
#if (FLOAT==1)
complete:		flag.dp=0;
#endif
}


void print_integer(char* out, const char* flag_ptr, int i)
{
	print_i(out,format_manipulate(flag_ptr),i);
}
#endif

#if (FLOAT==1)
void print_float(char* out, const char* flag_ptr, float f)
{

	struct format_flag flag = format_manipulate(flag_ptr);				// get format flag
	unsigned char d, int_width, fract_width, neg_width, min_width, dp_width;
	float c,y,z,fractpart;
	int intpart;

	c=1.0;
	neg_width=0;
	dp_width=1;

	y=f-(int)f;
	d=flag.decimal;

	if(y==0.0){									//operate case 34,00,15.00000 etc,,.
	 while(d>1){
	  flag.add_zero++;
	  d--;
	 }
	  c=0.0;
	  d=flag.decimal;
	}

	if(f<0) c*=-1;									//for negative calculation

	for(;d>0;d--){
	 c*=0.1;
	}

	z=f+(c/2);

	fractpart=z-(int)z;

	y=fractpart;

	if(y<0) y*=-1;

	while(y<0.1){									// count down the no.of zero
	 if(y==0.0) break;								// case for eg,1.005, 13.05, 34.0001
	 y*=10;
	 flag.add_zero++;
	}

	if((flag.add_zero>=flag.decimal)&&(flag.add_zero>0))  flag.add_zero--;		// operate "0.0, 0.1 , 0.2"

	intpart=(int)(z-fractpart);

	for(d=0;d<flag.decimal;d++){
	 fractpart*=10;
	}

	if(fractpart<0) fractpart*=-1;							// disable negative value of fractpart

	if(intpart>=10000) int_width=5;							// calculate int_width
	else if(intpart>=1000) int_width=4;
	else if(intpart>=100) int_width=3;
	else if(intpart>=10) int_width=2;
	else if(intpart>-10) int_width=1;
	else if(intpart>-100) int_width=2;
	else if(intpart>-1000) int_width=3;
	else if(intpart>-10000) int_width=4;
	else int_width=5;

	if(fractpart>=10000) fract_width=5;						// calculate fract_width
	else if(fractpart>=1000) fract_width=4;
	else if(fractpart>=100) fract_width=3;
	else if(fractpart>=10) fract_width=2;
	else if(fractpart>=1) fract_width=1;
	else fract_width=flag.decimal;

	if(fract_width<flag.decimal) fract_width=flag.decimal;				//change fraction width

	if(((f>-1.0)&&(f<0.0))||(intpart<0)) neg_width=1;				//if flag.neg=1, neg_width=1

	if(fract_width==0){
		dp_width=0;
	 	flag.dp=0;
	}


	min_width=int_width+fract_width+dp_width+neg_width;				//calculate min_width


	if((min_width>flag.width)||(flag.pad_left)) flag.width=min_width;		//if min_width>flag.width or shift left

	print_i(out,flag,fractpart);

	flag.width=flag.width-fract_width-1-dp_width;					//use for position shift "\0" & "."
	flag.fulldec=1;
	flag.dp=0;
	flag.add_zero=0;

	if((f>-1.0)&&(f<0.0)) flag.neg=1;

	print_i(out,flag,intpart);							//print integer part number

}

#endif

#ifdef	TEST
#include <stdio.h>									// in I386 linux platform
int main(void)
{
	unsigned char vec_buf[13];							//vec_buf[13] for floating point
											//vec_buf[6] for integer

#ifdef	TEST_PRINT_INTEGER
	print_integer(vec_buf, "1", 0);
	printf("%s\n",vec_buf);

	print_integer(vec_buf, "2", 45);						// test fitted +ve
	printf("%s\n",vec_buf);

	print_integer(vec_buf, "4", -101);						// test fitted -ve
	printf("%s\n",vec_buf);

	print_integer(vec_buf, "-5", 23);						// left +ve
	printf("%s\n",vec_buf);

	print_integer(vec_buf, "-5", -78);						// left -ve
	printf("%s\n",vec_buf);

	print_integer(vec_buf, "05", 26);						// place zero for space +ve
	printf("%s\n",vec_buf);

	print_integer(vec_buf, "05", -26);						// place zero for space -ve
	printf("%s\n",vec_buf);

	print_integer(vec_buf, "5", -26);						// place ' ' for space -ve
	printf("%s\n",vec_buf);

	print_integer(vec_buf, "5", 0);							// place ' ' for space +ve
	printf("%s\n",vec_buf);

	printf("******************************\n");

	print_integer(vec_buf, "1", 0);
	printf("%s\n",vec_buf);

	print_integer(vec_buf, "2", 0);
	printf("%s\n",vec_buf);

	print_integer(vec_buf, "3", 0);
	printf("%s\n",vec_buf);

	print_integer(vec_buf, "4", 0);
	printf("%s\n",vec_buf);

	print_integer(vec_buf, "5", 0);
	printf("%s\n",vec_buf);

	print_integer(vec_buf, "6", 0);
	printf("%s\n",vec_buf);

	print_integer(vec_buf, "7", 0);
	printf("%s\n",vec_buf);

	print_integer(vec_buf, "8", 0);
	printf("%s\n",vec_buf);

#endif

#if (FLOAT==1)
	printf("******************************\n");

	print_float(vec_buf, "4.1", 55.45);						// test fitted +ve
	printf("0.1f=%s\n",vec_buf);

	print_float(vec_buf, "5.2", -0.909);						// test fitted -ve
	printf("5.2f=%s\n",vec_buf);

	print_float(vec_buf, "-1.5", 0.00001);						// left fitted +ve
	printf("-1.5f=%s\n",vec_buf);

	print_float(vec_buf, "-6.2", -100.0004);					// left fitted -ve
	printf("-6.2f=%s\n",vec_buf);

	print_float(vec_buf, "07.2", 120.0);						// place zero for space +ve
	printf("7.2f=%s\n",vec_buf);

	print_float(vec_buf, "08.3", -1.505);						// place zero for space -ve
	printf("08.3f=%s\n",vec_buf);

	print_float(vec_buf, "9.2", 100.03);						// place " " for space +ve
	printf("9.2f=%s\n",vec_buf);

	print_float(vec_buf, "9.2", -33.038);						// place " " for space -ve
	printf("9.2f=%s\n",vec_buf);

	print_float(vec_buf, "0.3", 0.0);						// test zero
	printf("0.3f=%s\n",vec_buf);

	print_float(vec_buf, "4.5", -99999.0);						// test the lowest no. -ve
	printf("4.5f=%s\n",vec_buf);

	print_float(vec_buf, "2.3", 99999.0);						// test the upper no.+ve
	printf("2.3f=%s\n",vec_buf);
#endif

	return 0;

}

#endif


