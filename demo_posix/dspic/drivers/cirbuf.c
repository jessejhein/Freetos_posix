/************************************************************************************************
 * File: 			cirbuf.c
 * Description:		implementation of circular buffer
 ***********************************************************************************************/

/***********************************************************************************************
 * 	Copyright (C) 2004 cheng chung yan <yan@amonics.com> for 8-bit system linlike8
 *	lib C in linlike8 8 bit system
 *
 *	Design Concept
 *		Make a circular buffer template.
 *		this is only a template, not for real code to work, please implement this code into 
 * 		your application area.
 *
 *			| wr ptr
 *		top	V			       bottom
 *		|<------------------------------------>| buffer			wr and rd ptr, they both 
 *				^					wr ptr must go prior rd		must go upward, then circular
 *				| rd ptr
 *
 *  Testing Procedures
 *	before you compile, you must copy whole directory of linlike8/ to your local home
 *	compile this file in linux gcc environment : " make cirbuf_to_run.o"
 *	testing by : "main_compile_dir/cirbuf_to_run.o"
 **********************************************************************************************/

/**********************************************************************************************
 * Determine the next valid position to write to the buffer
 * Return: 
 *   +-- 0 - 254 : 	next position of buffer
 *   +-- 255:		buffer is full
 **********************************************************************************************/
unsigned char pre_wr_cir254buf(unsigned char wr, unsigned char rd, unsigned char max)
{
	unsigned char wr_backup = wr + 1;		//normal position of next char to write
	if (wr_backup==max) wr_backup = 0;		//if reach the end of buf, loop back
	if (wr_backup==rd) return 255;			//Buffer is full
	else return wr_backup;
}

/**********************************************************************************************
 * Determine the next valid position to read from the buffer
 * Return: 
 *   +-- 0 - 254 : 	next position of buffer
 *   +-- 255:		no data
 **********************************************************************************************/
unsigned char pre_rd_cir254buf(unsigned char wr, unsigned char rd, unsigned char max)
{
	unsigned char rd_backup;
	if (rd==wr) return 255;						//all data read
	else 
    {
		rd_backup = rd + 1;						//normal position of next char to read
		if (rd_backup==max) rd_backup = 0;		//if reach the end of buf, loop back
		return rd_backup;						
	}
}

