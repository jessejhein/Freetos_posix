	.module printf.c
	.area text(rom, con, rel)
	.dbfile ./printf.c
	.dbfunc e format_manipulate _format_manipulate fX
;           flag -> X+0
;       flag_ptr -> X-7
_format_manipulate::
	.dbline -1
	push X
	mov X,SP
	add SP,3
	.dbline 90
; /*  Copyright (C) 2004		cheng chung yan	<yan@amonics.com> for 8-bit system
; *  lib C in 8 bit system
; *
; *  This library is free software; you can redistribute it and/or
; *  modify it under the terms of the GNU Library General Public
; *  License as published by the Free Software Foundation; either
; *  version 2 of the License, or (at your option) any later version.
; *
; *  This library is distributed in the hope that it will be useful,
; *  but WITHOUT ANY WARRANTY; without even the implied warranty of
; *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
; *  Library General Public License for more details.
; *
; *  You should have received a copy of the GNU Library General Public
; *  License along with this library; if not, write to the Free
; *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
; *
; *  Design Concept
; *	transfer info. to string and place into the buffer which pointed by a char pointer
; *	support reentrant
; *
; *  Testing Procedures
; *	before you compile, you must copy whole directory of ucliblike8/ to your local home
; *	compile this file in linux gcc environment : "make printf.o" in ucliblike8/
; *	testing by : "main_compile_dir/printf.o" in ucliblike8/
; *
; *  Features
; *	printf format specific
; *		- as left justify
; *		0 as pad zero / empty
; *		number as width for display
; *	print integer & floating point with
; *		left justify
; *		pad zero / empty
; *		width
; *		negative num
; *		base 10 only
; *	floating point
; *		range: -99999.0 to 99999.0
; *		Maximum decimal digit range: 5 decimal digit
; *		Maximum width range=-9 to 9
; *		must entry the width no. of decimal point & the width no.of integer
; *
; *  Examples
; *	print_integer
; *		display negative number with zero for empty position and 4 digital width
; *			unsigned char vec_buf[6];
; *			print_integer(vec_buf, "04", -45);
; *			printf("%s\n",vec_buf);
; *		display number with left justify and 5 digital width
; *			unsigned char vec_buf[6];
; *			print_integer(vec_buf, "-5", 23);
; *			printf("%s\n",vec_buf);
; *	print_float
; *		display negative number with zero for empty position and 5 digital width, 3 decimal points
; *			unsigned char vec_buf[13];
; *			print_float(vec_buf, "05.3", -13.5);
; *			printf("%s\n",vec_buf);
; *		display number with left justify and 5 digital width, 3 decimal points
; *			unsigned char vec_buf[13];
; *			print_float(vec_buf, "-5.2", 23.3);
; *			printf("%s\n",vec_buf);
; *
; *  Pay Attention
; *	MUST input a valid format flag, no chk in coding
; *	width data must great than the real width of number
; *	MUST provide enough buffer (char* out) to hold the char. data, eg. 3456, must provide 5 bytes including '\0'
; *
; */
; 
; #include "printf.h"
; 
; struct format_flag {
; 	unsigned	pad_left :1 ;							// 0 - right justify, 1 - left justify
; 	unsigned	pad_zero :1 ;							// 0 - blank space for empty digit, 1 - '0' for empty digit
; 	unsigned	neg :1 ;							// 0 - +ve number, 1 - '0' - -ve number
; 	unsigned	width :5 ;							// max. 32 digit placement
; #if (FLOAT==1)
; //	signed		width :5;
; 	unsigned	dp :1 ;								// 0 - no decimal, 1 - have decimal
; 
; 	unsigned	decimal :5 ;							// max. 32 digit placement
; 
; 	unsigned	fulldec :1 ;							// 0 - no full decimal, 1 - full decimal
; 	unsigned	add_zero :3 ;							// max 8 digit placement
; #endif
; };
; 
; struct format_flag format_manipulate(const char* flag_ptr)
; {
	.dbline 93
; 	struct format_flag flag;
; 
; 	flag.pad_left = 0;
	mov A,[X+0]
	and A,-2
	mov [X+0],A
	.dbline 94
; 	flag.pad_zero = 0;
	mov A,[X+0]
	and A,-3
	mov [X+0],A
	.dbline 95
; 	flag.neg = 0;
	mov A,[X+0]
	and A,-5
	mov [X+0],A
	.dbline 96
; 	flag.width = 0;
	mov A,[X+0]
	and A,7
	mov [X+0],A
	.dbline 104
	xjmp L6
L3:
	.dbline 104
; #if (FLOAT==1)
; 	flag.dp = 0;
; 	flag.decimal = 0;
; 	flag.fulldec = 0;
; 	flag.add_zero = 0;
; #endif
; 
; 	for (;*flag_ptr;flag_ptr++) {
	.dbline 105
	mov A,[X-6]
	mov [__r1],A
	mov A,[X-7]
	push X
	mov X,[__r1]
	romx
	pop X
	mov [X+2],A
	mov [X+1],0
	cmp [X+1],0
	jnz X1
	cmp [X+2],45
	jz L10
X1:
	cmp [X+1],0
	jnz X2
	cmp [X+2],48
	jz L11
X2:
	xjmp L7
X0:
	.dbline 105
; 		switch (*flag_ptr) {
L10:
	.dbline 107
; 			case '-' : 							// left justify
; 				 flag.pad_left = 1;
	mov A,[X+0]
	or A,1
	mov [X+0],A
	.dbline 108
; 				break;
	xjmp L8
L11:
	.dbline 110
; 			case '0' : 							// pad zero / empty
; 				 flag.pad_zero = 1;
	mov A,[X+0]
	or A,2
	mov [X+0],A
	.dbline 111
; 				break;
	xjmp L8
L7:
	.dbline 120
; #if (FLOAT==1)
; 			case '.' :
; 				 flag.dp= 1;
; 				 flag_ptr++;
; 				 flag.decimal = *flag_ptr - '0';
; 				break;
; #endif
; 			default :							// number
; 				 flag.width = *flag_ptr - '0';
	mov A,[X+0]
	and A,7
	mov [__r0],A
	mov A,[X-6]
	mov [__r3],A
	mov A,[X-7]
	push X
	mov X,[__r3]
	romx
	pop X
	mov [__r2],A
	sub [__r2],48
	and [__r2],31
	mov A,[__r2]
	asl A
	asl A
	asl A
	and A,-8
	or [__r0],A
	mov A,[__r0]
	mov [X+0],A
	.dbline 124
; 				 // ATTENTION ATTENTION ATTENTION
; 				 //	this input is programmer job to input a valid number
; 				 //	we just want to reduce code size and execution speed, so ignore the checking for programming fault, we assume this must be a number
; 		}
L8:
	.dbline 125
L4:
	.dbline 104
	inc [X-6]
	adc [X-7],0
L6:
	.dbline 104
	mov A,[X-6]
	mov [__r1],A
	mov A,[X-7]
	push X
	mov X,[__r1]
	romx
	pop X
	cmp A,0
	jnz L3
	.dbline 128
; 	}
; 
; 
; 	return flag;
	mov A,[X-4]
	mov [__r1],A
	mov [__r3],X
	mvi A,[__r3]
	mvi [__r1],A
	.dbline -2
L1:
	add SP,-3
	pop X
	.dbline 0 ; func end
	ret
	.dbsym l flag 0 X
	.dbsym l flag_ptr -7 pc
	.dbend
	.dbfunc e print_i _print_i fV
;            tmp -> X+2
;              j -> X+1
;        padchar -> X+0
;              i -> X-9
;           flag -> X-7
;            out -> X-5
_print_i::
	.dbline -1
	push X
	mov X,SP
	add SP,3
	.dbline 133
; }
; 
; #if (INTEGER==1)
; void print_i(char* out, struct format_flag flag, int i)
; {
	.dbline 141
; 	#define	base	10
; 	char padchar;
; 
; #if (FLOAT==1)
; 	if (flag.fulldec) goto next;
; #endif
; 
; 	*(out + flag.width--) = '\0';							// place a NULL at the end of string
	mov A,[X-6]
	mov [__r1],A
	mvi A,[__r1]
	mov [__r1],A
	mov [__r0],0
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	and [__r1],31
	and [__r0],0
	mov A,[X-6]
	mov [__r3],A
	mov A,[X-6]
	mov [__r5],A
	mvi A,[__r5]
	mov [__r4],A
	and [__r4],7
	mov A,[__r1]
	sub A,1
	mov [__r7],A
	mov A,[__r0]
	sbb A,0
	mov [__r6],[__r7]
	and [__r6],31
	mov A,[__r6]
	asl A
	asl A
	asl A
	and A,-8
	or [__r4],A
	mov A,[__r4]
	mvi [__r3],A
	mov A,[X-4]
	add [__r1],A
	mov A,[X-5]
	adc [__r0],A
	mov A,0
	mvi [__r1],A
L13:
	.dbline 143
; 
; next:	if (i<0) {									// -ve number
	mov A,[X-9]
	xor A,-128
	mov [__rY],A
	mov A,0
	xor A,-128
	mov [__rZ],A
	mov [__rX],0
	mov A,[X-8]
	sub A,0
	or [__rX],A
	mov A,[__rY]
	sbb A,[__rZ]
	or [__rX],A
	jnc L14
	mov A,[__rX]
	jz L14
X3:
	.dbline 143
	.dbline 144
; 		i = -i;
	mov A,[X-8]
	cpl A
	sub A,-1
	mov [X-8],A
	mov A,[X-9]
	cpl A
	sbb A,-1
	mov [X-9],A
	.dbline 145
; 		flag.neg = 1;
	mov A,[X-6]
	mov [__r1],A
	mov A,[X-6]
	mov [__r3],A
	mvi A,[__r3]
	or A,4
	mvi [__r1],A
	.dbline 146
; 	}
L14:
	.dbline 147
; 	if (i==0) *(out + flag.width--) = '0';						//if i=0
	cmp [X-9],0
	jnz L19
	cmp [X-8],0
	jnz L19
X4:
	.dbline 147
	mov A,[X-6]
	mov [__r1],A
	mvi A,[__r1]
	mov [__r1],A
	mov [__r0],0
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	and [__r1],31
	and [__r0],0
	mov A,[X-6]
	mov [__r3],A
	mov A,[X-6]
	mov [__r5],A
	mvi A,[__r5]
	mov [__r4],A
	and [__r4],7
	mov A,[__r1]
	sub A,1
	mov [__r7],A
	mov A,[__r0]
	sbb A,0
	mov [__r6],[__r7]
	and [__r6],31
	mov A,[__r6]
	asl A
	asl A
	asl A
	and A,-8
	or [__r4],A
	mov A,[__r4]
	mvi [__r3],A
	mov A,[X-4]
	add [__r1],A
	mov A,[X-5]
	adc [__r0],A
	mov A,48
	mvi [__r1],A
	xjmp L19
L18:
	.dbline 149
	.dbline 150
	mov A,[X-6]
	mov [__r1],A
	mvi A,[__r1]
	mov [__r1],A
	mov [__r0],0
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	and [__r1],31
	and [__r0],0
	mov A,[X-6]
	mov [__r3],A
	mov A,[X-6]
	mov [__r5],A
	mvi A,[__r5]
	mov [__r4],A
	and [__r4],7
	mov A,[__r1]
	sub A,1
	mov [__r7],A
	mov A,[__r0]
	sbb A,0
	mov [__r6],[__r7]
	and [__r6],31
	mov A,[__r6]
	asl A
	asl A
	asl A
	and A,-8
	or [__r4],A
	mov A,[__r4]
	mvi [__r3],A
	mov A,[X-4]
	add [__r1],A
	mov A,[X-5]
	adc [__r0],A
	mov A,0
	push A
	mov A,10
	push A
	mov A,[X-9]
	push A
	mov A,[X-8]
	push A
	xcall __divmod_16X16_16
	add SP,-2
	pop A
	mov [__r3],A
	pop A
	mov [__r2],A
	add [__r3],48
	adc [__r2],0
	mov A,[__r3]
	mvi [__r1],A
	.dbline 151
	mov A,0
	push A
	mov A,10
	push A
	mov A,[X-9]
	push A
	mov A,[X-8]
	push A
	xcall __divmod_16X16_16
	pop A
	mov [X-8],A
	pop A
	mov [X-9],A
	add SP,-2
	.dbline 152
L19:
	.dbline 149
; 
; 	while (i) {									// convert the integer number to string
	cmp [X-9],0
	jnz L18
	cmp [X-8],0
	jnz L18
X5:
	.dbline 153
; 		*(out + flag.width--) = (i % base) + '0';
; 		i /= base;
; 	}
; 	if (flag.neg) *(out + flag.width--) = '-';
	mov A,[X-6]
	mov [__r1],A
	mvi A,[__r1]
	mov [__r0],A
	asr [__r0]
	asr [__r0]
	and [__r0],1
	jz L21
	.dbline 153
	mov A,[X-6]
	mov [__r1],A
	mvi A,[__r1]
	mov [__r1],A
	mov [__r0],0
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	and [__r1],31
	and [__r0],0
	mov A,[X-6]
	mov [__r3],A
	mov A,[X-6]
	mov [__r5],A
	mvi A,[__r5]
	mov [__r4],A
	and [__r4],7
	mov A,[__r1]
	sub A,1
	mov [__r7],A
	mov A,[__r0]
	sbb A,0
	mov [__r6],[__r7]
	and [__r6],31
	mov A,[__r6]
	asl A
	asl A
	asl A
	and A,-8
	or [__r4],A
	mov A,[__r4]
	mvi [__r3],A
	mov A,[X-4]
	add [__r1],A
	mov A,[X-5]
	adc [__r0],A
	mov A,45
	mvi [__r1],A
L21:
	.dbline 166
; 
; #if (FLOAT==1)
; 	for(;flag.add_zero;flag.add_zero--){						// add zero for 1.005,1.05,1.012 etc
; 		*(out + flag.width--) = '0';
; 	}
; 
; 	if (flag.dp) {
; 	 	*(out + flag.width--) = '.';
; 		goto complete;
; 	}
; #endif
; 
; 	if (++flag.width) {								// if not fit to width, place something in the space
	mov A,[X-6]
	mov [__r1],A
	mvi A,[__r1]
	mov [__r1],A
	mov [__r0],0
	mov [__r3],A
	mov [__r2],0
	asr [__r2]
	rrc [__r3]
	asr [__r2]
	rrc [__r3]
	asr [__r2]
	rrc [__r3]
	and [__r3],31
	and [__r2],0
	add [__r3],1
	adc [__r2],0
	mov [__r2],0
	and [__r3],31
	and [__r2],0
	mov A,[X-6]
	mov [__r5],A
	and [__r1],7
	and [__r0],-1
	mov [__r7],[__r3]
	mov [__r6],[__r2]
	asl [__r7]
	rlc [__r6]
	asl [__r7]
	rlc [__r6]
	asl [__r7]
	rlc [__r6]
	and [__r7],-8
	and [__r6],0
	mov A,[__r7]
	or [__r1],A
	mov A,[__r6]
	or [__r0],A
	mov A,[__r1]
	mvi [__r5],A
	cmp [__r2],0
	jnz X6
	cmp [__r3],0
	jz L23
X6:
	.dbline 166
	.dbline 171
; 											//	since the above ++flag.width, so i must adjust back to right position
; //	printf("flagwidth=%d\n",flag.width);						//check point only
; 
; 
; 		if (flag.pad_left) {							// left justify and width value > real width
	mov A,[X-6]
	mov [__r1],A
	mvi A,[__r1]
	mov [__r0],A
	and [__r0],1
	jz L25
	.dbline 171
	.dbline 172
; 			unsigned char j = 0;						//	adding one to flag.width 1st, because
	mov [X+1],0
L27:
	.dbline 174
; 			char tmp;							//	if fit of digial placement, ++flag.width == 0
; 			do {
	.dbline 175
; 				tmp = *(out + flag.width + j);
	mov A,[X+1]
	mov [__r1],A
	mov [__r0],0
	mov A,[X-6]
	mov [__r3],A
	mvi A,[__r3]
	mov [__r3],A
	mov [__r2],0
	asr [__r2]
	rrc [__r3]
	asr [__r2]
	rrc [__r3]
	asr [__r2]
	rrc [__r3]
	and [__r3],31
	and [__r2],0
	mov A,[X-4]
	add [__r3],A
	mov A,[X-5]
	adc [__r2],A
	mov A,[__r3]
	add [__r1],A
	mov A,[__r2]
	adc [__r0],A
	mvi A,[__r1]
	mov [X+2],A
	.dbline 176
; 				*(out + j) = tmp;
	mov A,[X+1]
	mov [__r1],A
	mov [__r0],0
	mov A,[X-4]
	add [__r1],A
	mov A,[X-5]
	adc [__r0],A
	mov A,[X+2]
	mvi [__r1],A
	.dbline 177
; 				j++;
	inc [X+1]
	.dbline 178
L28:
	.dbline 178
; 			} while (tmp);							//	chk '\0'
	cmp [X+2],0
	jnz L27
	.dbline 179
	xjmp L26
L25:
	.dbline 179
; 		} else {
	.dbline 180
	mov A,[X-6]
	mov [__r1],A
	mvi A,[__r1]
	mov [__r1],A
	mov [__r0],0
	mov [__r3],A
	mov [__r2],0
	asr [__r2]
	rrc [__r3]
	asr [__r2]
	rrc [__r3]
	and [__r3],1
	and [__r2],0
	jnz X7
	cmp [__r3],0
	jz L32
X7:
	asr [__r0]
	rrc [__r1]
	and [__r1],1
	and [__r0],0
	jnz L30
	cmp [__r1],0
	jnz L30
X8:
L32:
	.dbline 180
; 			if ((!flag.neg)||(!flag.pad_zero)) flag.width--;
	mov A,[X-6]
	mov [__r1],A
	mvi A,[__r1]
	mov [__r1],A
	mov [__r0],0
	mov A,[X-6]
	mov [__r3],A
	mov A,[__r1]
	and A,7
	mov [__r5],A
	mov A,0
	and A,-1
	mov [__r4],A
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	and [__r1],31
	and [__r0],0
	sub [__r1],1
	sbb [__r0],0
	mov [__r0],0
	and [__r1],31
	and [__r0],0
	asl [__r1]
	rlc [__r0]
	asl [__r1]
	rlc [__r0]
	asl [__r1]
	rlc [__r0]
	and [__r1],-8
	and [__r0],0
	mov A,[__r5]
	or A,[__r1]
	mov [__r1],A
	mov A,[__r4]
	or A,[__r0]
	mov A,[__r1]
	mvi [__r3],A
L30:
	.dbline 181
; 			if (flag.pad_zero) padchar = '0';				// place zero for empty space and width value > real width
	mov A,[X-6]
	mov [__r1],A
	mvi A,[__r1]
	mov [__r0],A
	and F,-5
	rrc [__r0]
	and [__r0],1
	jz L33
	.dbline 181
	mov [X+0],48
	xjmp L34
L33:
	.dbline 182
; 			else padchar = ' ';
	mov [X+0],32
L34:
L35:
	.dbline 183
; 			do {
	.dbline 184
; 				*(out + flag.width) = padchar;
	mov A,[X-6]
	mov [__r1],A
	mvi A,[__r1]
	mov [__r1],A
	mov [__r0],0
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	and [__r1],31
	and [__r0],0
	mov A,[X-4]
	add [__r1],A
	mov A,[X-5]
	adc [__r0],A
	mov A,[X+0]
	mvi [__r1],A
	.dbline 185
; 				if (flag.width) flag.width--;
	mov A,[X-6]
	mov [__r1],A
	mvi A,[__r1]
	asr A
	asr A
	asr A
	and A,31
	mov [__r0],A
	and [__r0],31
	jz L37
	.dbline 185
	mov A,[X-6]
	mov [__r1],A
	mvi A,[__r1]
	mov [__r1],A
	mov [__r0],0
	mov A,[X-6]
	mov [__r3],A
	mov A,[__r1]
	and A,7
	mov [__r5],A
	mov A,0
	and A,-1
	mov [__r4],A
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	asr [__r0]
	rrc [__r1]
	and [__r1],31
	and [__r0],0
	sub [__r1],1
	sbb [__r0],0
	mov [__r0],0
	and [__r1],31
	and [__r0],0
	asl [__r1]
	rlc [__r0]
	asl [__r1]
	rlc [__r0]
	asl [__r1]
	rlc [__r0]
	and [__r1],-8
	and [__r0],0
	mov A,[__r5]
	or A,[__r1]
	mov [__r1],A
	mov A,[__r4]
	or A,[__r0]
	mov A,[__r1]
	mvi [__r3],A
	.dbline 186
; 				else break;
L39:
	.dbline 187
L36:
	.dbline 187
; 			} while (1);
	xjmp L35
L37:
	.dbline 188
; 			if ((flag.neg)&&(flag.pad_zero)) *out = '-';			//	should be *(out + flag.width), but flag.width must be zere
	mov A,[X-6]
	mov [__r1],A
	mvi A,[__r1]
	mov [__r1],A
	mov [__r0],0
	mov [__r3],A
	mov [__r2],0
	asr [__r2]
	rrc [__r3]
	asr [__r2]
	rrc [__r3]
	and [__r3],1
	and [__r2],0
	jnz X9
	cmp [__r3],0
	jz L40
X9:
	asr [__r0]
	rrc [__r1]
	and [__r1],1
	and [__r0],0
	jnz X10
	cmp [__r1],0
	jz L40
X10:
	.dbline 188
	mov A,[X-4]
	mov [__r1],A
	mov A,45
	mvi [__r1],A
L40:
	.dbline 189
; 		}
L26:
	.dbline 190
L23:
	.dbline -2
	.dbline 194
; 	}
; #if (FLOAT==1)
; complete:		flag.dp=0;
; #endif
; }
L12:
	add SP,-3
	pop X
	.dbline 0 ; func end
	ret
	.dbsym l tmp 2 c
	.dbsym l j 1 c
	.dbsym l padchar 0 c
	.dbsym l i -9 I
	.dbsym l flag -7 pX
	.dbsym l out -5 pc
	.dbend
	.dbfunc e print_integer _print_integer fV
;              i -> X-9
;       flag_ptr -> X-7
;            out -> X-5
_print_integer::
	.dbline -1
	push X
	mov X,SP
	add SP,1
	.dbline 198
; 
; 
; void print_integer(char* out, const char* flag_ptr, int i)
; {
	.dbline 199
; 	print_i(out,format_manipulate(flag_ptr),i);
	mov A,[X-7]
	push A
	mov A,[X-6]
	push A
	mov A,[__r0]
	push A
	push X
	xcall _format_manipulate
	mov A,[X-9]
	push A
	mov A,[X-8]
	push A
	mov A,[__r0]
	push A
	push X
	mov A,[X-5]
	push A
	mov A,[X-4]
	push A
	xcall _print_i
	add SP,-10
	.dbline -2
	.dbline 200
; }
L42:
	add SP,-1
	pop X
	.dbline 0 ; func end
	ret
	.dbsym l i -9 I
	.dbsym l flag_ptr -7 pc
	.dbsym l out -5 pc
	.dbend
