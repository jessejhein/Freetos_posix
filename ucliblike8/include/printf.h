/*  Copyright (C) 2004		cheng chung yan	<chungyan5@hkem.com / 
yan@amonics.com> for 8-bit system
*  lib C in 8 bit system
*
*  Header of printf
*
*  this header file is be copied to application and applicaotin 
specification
*
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

