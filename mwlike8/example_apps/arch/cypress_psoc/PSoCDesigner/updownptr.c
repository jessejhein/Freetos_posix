/*	Copyright (C) 2004 cheng chung yan <yan@amonics.com> for 8-bit system linlike8
 *	lib C in linlike8 8 bit system
 *
 *	updownptr.c
 *
 *	Design Concept
 *		Make a circular buffer template.
 *		using adjusting(related) value from original to destination, since 
 *			one lib, supporting circular and non-circular
 *			we can handle some processing in moving, such as step by step ...
 *
 *			| ptr						ptr can go up/downward
 *		top	V			       bottom		parameter to set circular or not
 *		|<------------------------------------>| buffer		
 *
 *		advantage of related value
 *			* do not need to keep all range of distinguish data
 */

// parameters
//	circular_flag -- 1 as circular, 0 as non-circular
unsigned char up254buf(unsigned char adj_val, unsigned char cur_pos, unsigned char circular_flag, unsigned char max, unsigned char min)
{
	if (adj_val>(max-cur_pos)) {
		if (circular_flag) cur_pos = adj_val - (max-cur_pos) + min - 1;
		else cur_pos = max;
	} else {
		cur_pos += adj_val;
	}
	return cur_pos;
}

unsigned char down254buf(unsigned char adj_val, unsigned char cur_pos, unsigned char circular_flag, unsigned char max, unsigned char min)
{
	if (adj_val>(cur_pos-min)) {
		if (circular_flag) cur_pos = max - (adj_val - (cur_pos-min)) + 1;
		else cur_pos = min;
	} else {
		cur_pos -= adj_val;
	}
	return cur_pos;
}

