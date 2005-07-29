/*
 * Copyright (c) 05-05-2004 cheng chung yan <yan@amonics.com> for 8-bit system
 *
 * In char LCD, TINSHARP(http://www.tinsharp.com/) tc1602e
 *
 * Char mode
 * hardware setting 
 *	PSoC
                // chip's hardware setting
                //      * 24MHz cpu clk
 *	Hei	10-03-2005	modified for the cur position
 */

// including, get data linking from others ===============================================
//	this appl. layer
#include <pin_define.h>
//	gui mwlike config.
#include "config_mwlike8.h"

#include "app.h"									// linlike8 os

#include "nano-X.h"
#include "system.h"									// cli()
#include "ts_tc1602e.h"									// hw bet. lcd and mcu

#define	CLEAR_PAPER	0x01
const unsigned char ClearPaper[]={CLEAR_PAPER};
void wr_ts_tc1602e(unsigned char inst, unsigned char data);

void scr_open(void)
{
	unsigned int i;
	
	CS_low;											// not selected
	
	// init flow provided from lcd manufacturer
												// Power ON
	// msleep(31);
	for (i=0;i<1000;i++);									// when os ready, change to os delay
	wr_ts_tc1602e(1, 0x3c);
	// usleep(40);
	for (i=0;i<500;i++);
	wr_ts_tc1602e(1, 0x0f);					//modified for the cur position
	// usleep(40);
	for (i=0;i<500;i++);
	wr_ts_tc1602e(1, 0x01);
	// msleep(2);
	for (i=0;i<1000;i++);
	wr_ts_tc1602e(1, 0x06);
}

void wr_ts_tc1602e(unsigned char inst, unsigned char data)
{
	unsigned char i;
	
	cli();										// disable interrupt, make sure the clock time
	
	if (inst) a0_low;								// instruction by A0 as low
	else a0_high;
	
	CS_high;
	
	i = 0x01;
	if (data&i) d0_high;
	else d0_low;
	i <<= 1;
	if (data&i) d1_high;
	else d1_low;
	i <<= 1;
	if (data&i) d2_high;
	else d2_low;
	i <<= 1;
	if (data&i) d3_high;
	else d3_low;
	i <<= 1;
	if (data&i) d4_high;
	else d4_low;
	i <<= 1;
	if (data&i) d5_high;
	else d5_low;
	i <<= 1;
	if (data&i) d6_high;
	else d6_low;
	i <<= 1;
	if (data&i) d7_high;
	else d7_low;
	
	CS_low;
	
	sti();
	
	for (i=0;i<175;i++);// usleep(40);, 175 is min. value
	
}

/*
 * Draw a text in the specified drawable using the
 * specified graphics context.
 *
 * flags
 *	bit 0 - '0' as high light off, '1' as high light on
 */
void
GrTextDrv(GR_COORD x, GR_COORD y, unsigned char font_code,
	GR_TEXTFLAGS flags)
{
	unsigned char i;
	
	switch (font_code) {
		case CLEAR_PAPER :
			 wr_ts_tc1602e(1, CLEAR_PAPER);
			 for (i=0;i<250;i++);// usleep(40);
			 for (i=0;i<250;i++);
			 for (i=0;i<250;i++);
			 for (i=0;i<250;i++);
			 for (i=0;i<250;i++);
			break;
		default :
			 wr_ts_tc1602e(1, 0x80 + x + y*0x40);
			 wr_ts_tc1602e(0, font_code);
			 wr_ts_tc1602e(1, 0x80 + x + (y)*0x40);
	}
	
}
