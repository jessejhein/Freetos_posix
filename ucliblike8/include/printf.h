/*  Copyright (C) 2004		cheng chung yan	<yan@amonics.com> for 8-bit system
*  lib C in 8 bit system
*
*  Header of printf
*
*  this header file is be copied to application and application specification
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
*/

	// Parameters for this module

		// modules, if do not any module, set it to zero, then reduce code size
#define	INTEGER			1
#define FLOAT			0
	// declarations

#if (INTEGER==1)
extern void print_integer(char* out, const char* flag_ptr, int i);
#endif



#if (FLOAT==1)
extern void print_float(char* out, const char* flag_ptr, float f);
#endif

