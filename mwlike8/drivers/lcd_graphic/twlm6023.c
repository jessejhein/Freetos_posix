/*
 * Copyright (c) 05-05-2004 cheng chung yan <yan@amonics.com> for 8-bit system
 *
 * In mono LCD, 128X64 configuration, topway(http://www.topwaydisplay.com/) lm6023
 * hardware setting under Laser Control circuit by PSoC
 *
 * Char mode
 * hardware setting 
 *	PSoC
                // chip's hardware setting
                //      * 24MHz cpu clk
                //      * reset and CS as pull high (for faster response on active low)
                //      * A0, D6 and D7 as strong (for faster response on any stage)
                   init. 
                        * from util/sys start init.
                                * chip select disable 
                                * reset 
                        * local init.
 *	LCD
                whole screen - 128X64
                font - 8x8
 *        
 * History
 * Date         author		descrption
 * 12-08-2004	yan		creation
 * 04-10-2004	Hei		enable the "=,-" by enable the ASCII-0x2A-0x2B
 * 06-10-2004	hei		the "+", "-" can be showed in the LCD.
 */

// including, get data linking from others ===============================================
//	this appl. layer
#include <pin_define.h>
//	gui mwlike config.
#include "config_mwlike8.h"

#include "app.h"									// linlike8 os

#include "nano-X.h"
#include "device.h"									// scr_open
#include <asm\system.h>									// cli()

//	data =================================================================================

//		Continuous ASCII Code so use 2D Table
const unsigned char Font_Table[][8]=
{
	{ 0x00, 0x00, 0x14, 0x0E, 0x14, 0x00, 0x00, 0x00 },		// ASCII=0x2A
	{ 0x00, 0x10, 0x10, 0x7C, 0x10, 0x10, 0x00, 0x00 },		// ASCII=0x2B
	{ 0x00, 0x00, 0x00, 0x00, 0x80, 0x60, 0x00, 0x00 },		// ASCII=0x2C
	{ 0x00, 0x00, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00 },		// ASCII=0x2D
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00 },		// ASCII=0x2E
	{ 0x00, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x02, 0x00 },		// ASCII=0x2F
	{ 0x00, 0x7C, 0x42, 0x42, 0x3E, 0x00, 0x00, 0x00 },		// ASCII=0x30
	{ 0x00, 0x00, 0x04, 0x7E, 0x00, 0x00, 0x00, 0x00 },		// ASCII=0x31
	{ 0x00, 0x00, 0x44, 0x62, 0x52, 0x4C, 0x00, 0x00 },		// ASCII=0x32
	{ 0x00, 0x00, 0x24, 0x42, 0x4A, 0x34, 0x00, 0x00 },		// ASCII=0x33
	{ 0x00, 0x30, 0x28, 0x24, 0x7E, 0x20, 0x00, 0x00 },		// ASCII=0x34
	{ 0x00, 0x4E, 0x4A, 0x4A, 0x32, 0x00, 0x00, 0x00 },		// ASCII=0x35
	{ 0x00, 0x3C, 0x4A, 0x4A, 0x32, 0x00, 0x00, 0x00 },		// ASCII=0x36
	{ 0x00, 0x02, 0x62, 0x1A, 0x06, 0x00, 0x00, 0x00 },		// ASCII=0x37
	{ 0x00, 0x34, 0x4A, 0x4A, 0x34, 0x00, 0x00, 0x00 },		// ASCII=0x38
	{ 0x00, 0x1E, 0x12, 0x12, 0x7E, 0x00, 0x00, 0x00 },		// ASCII=0x39
	{ 0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00 },		// ASCII=0x3A
	{ 0x00, 0x00, 0x00, 0x40, 0x34, 0x00, 0x00, 0x00 },		// ASCII=0x3B
	{ 0x00, 0x10, 0x28, 0x28, 0x28, 0x44, 0x00, 0x00 },		// ASCII=0x3C
	{ 0x00, 0x28, 0x28, 0x28, 0x28, 0x28, 0x00, 0x00 },		// ASCII=0x3D
	{ 0x00, 0x44, 0x28, 0x28, 0x28, 0x10, 0x00, 0x00 },		// ASCII=0x3E
	{ 0x00, 0x04, 0x02, 0xB2, 0x12, 0x0C, 0x00, 0x00 },		// ASCII=0x3F
	{ 0x00, 0x7C, 0xC6, 0xFA, 0x8E, 0xE6, 0x3C, 0x00 },		// ASCII=0x40
	{ 0x00, 0x60, 0x1C, 0x12, 0x1C, 0x60, 0x00, 0x00 },		// ASCII=0x41
	{ 0x00, 0x7E, 0x4A, 0x4A, 0x3C, 0x00, 0x00, 0x00 },		// ASCII=0x42
	{ 0x00, 0x7C, 0x82, 0x82, 0x82, 0x44, 0x00, 0x00 },		// ASCII=0x43
	{ 0x00, 0x7E, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00 },		// ASCII=0x44
	{ 0x00, 0x7E, 0x4A, 0x4A, 0x4A, 0x42, 0x00, 0x00 },		// ASCII=0x45
	{ 0x00, 0x7E, 0x12, 0x12, 0x02, 0x00, 0x00, 0x00 },		// ASCII=0x46
	{ 0x00, 0x3C, 0x42, 0x42, 0x52, 0x34, 0x00, 0x00 },		// ASCII=0x47
	{ 0x00, 0x7E, 0x08, 0x08, 0x08, 0x7E, 0x00, 0x00 },		// ASCII=0x48
	{ 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00 },		// ASCII=0x49
	{ 0x00, 0x60, 0x40, 0x40, 0x3E, 0x00, 0x00, 0x00 },		// ASCII=0x4A
	{ 0x00, 0x7E, 0x08, 0x14, 0x22, 0x40, 0x00, 0x00 },		// ASCII=0x4B
	{ 0x00, 0x7E, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00 },		// ASCII=0x4C
	{ 0x00, 0x7E, 0x04, 0x18, 0x10, 0x04, 0x7E, 0x00 },		// ASCII=0x4D
	{ 0x00, 0x7E, 0x04, 0x08, 0x30, 0x7E, 0x00, 0x00 },		// ASCII=0x4E
	{ 0x00, 0x3C, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00 },		// ASCII=0x4F
	{ 0x00, 0x7E, 0x0A, 0x0A, 0x0E, 0x00, 0x00, 0x00 },		// ASCII=0x50
	{ 0x00, 0x3C, 0x42, 0x42, 0x62, 0x5C, 0x00, 0x00 },		// ASCII=0x51
	{ 0x00, 0x7E, 0x12, 0x12, 0x32, 0x4C, 0x00, 0x00 },		// ASCII=0x52
	{ 0x00, 0x00, 0x2C, 0x4A, 0x52, 0x34, 0x00, 0x00 },		// ASCII=0x53
	{ 0x00, 0x02, 0x02, 0x7E, 0x02, 0x02, 0x00, 0x00 },		// ASCII=0x54
	{ 0x00, 0x3E, 0x40, 0x40, 0x40, 0x3E, 0x00, 0x00 },		// ASCII=0x55
	{ 0x00, 0x0E, 0x30, 0x40, 0x30, 0x0E, 0x00, 0x00 },		// ASCII=0x56
	{ 0x00, 0x1E, 0x60, 0x1C, 0x38, 0x60, 0x1E, 0x00 },		// ASCII=0x57
	{ 0x00, 0x42, 0x34, 0x08, 0x34, 0x42, 0x00, 0x00 },		// ASCII=0x58
	{ 0x00, 0x02, 0x0C, 0x70, 0x0C, 0x02, 0x00, 0x00 },		// ASCII=0x59
	{ 0x00, 0x62, 0x52, 0x4A, 0x4A, 0x46, 0x42, 0x00 },		// ASCII=0x5A
	{ 0x00, 0x00, 0x00, 0x00, 0xFF, 0x81, 0x00, 0x00 },		// ASCII=0x5B
	{ 0x00, 0x00, 0x00, 0x06, 0x38, 0xC0, 0x00, 0x00 },		// ASCII=0x5C
	{ 0x00, 0x00, 0x81, 0xFF, 0x00, 0x00, 0x00, 0x00 },		// ASCII=0x5D
	{ 0x00, 0x00, 0x00, 0x0C, 0x02, 0x0C, 0x00, 0x00 },		// ASCII=0x5E
	{ 0x00, 0x00, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00 },		// ASCII=0x5F
	{ 0x00, 0x00, 0x00, 0x00, 0x06, 0x08, 0x00, 0x00 },		// ASCII=0x60
	{ 0x00, 0x00, 0x64, 0x54, 0x54, 0x7C, 0x00, 0x00 },		// ASCII=0x61
	{ 0x00, 0x00, 0x7F, 0x44, 0x44, 0x38, 0x00, 0x00 },		// ASCII=0x62
	{ 0x00, 0x00, 0x38, 0x44, 0x44, 0x28, 0x00, 0x00 },		// ASCII=0x63
	{ 0x00, 0x00, 0x38, 0x44, 0x44, 0x7F, 0x00, 0x00 },		// ASCII=0x64
	{ 0x00, 0x00, 0x38, 0x54, 0x54, 0x58, 0x00, 0x00 },		// ASCII=0x65
	{ 0x00, 0x00, 0x04, 0x7E, 0x05, 0x00, 0x00, 0x00 },		// ASCII=0x66
	{ 0x00, 0x00, 0x2E, 0x51, 0x51, 0x3F, 0x00, 0x00 },		// ASCII=0x67
	{ 0x00, 0x00, 0x7F, 0x04, 0x04, 0x78, 0x00, 0x00 },		// ASCII=0x68
	{ 0x00, 0x00, 0x00, 0x00, 0x7D, 0x00, 0x00, 0x00 },		// ASCII=0x69
	{ 0x00, 0x00, 0x40, 0x80, 0x7D, 0x00, 0x00, 0x00 },		// ASCII=0x6A
	{ 0x00, 0x00, 0x7F, 0x10, 0x38, 0x44, 0x00, 0x00 },		// ASCII=0x6B
	{ 0x00, 0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00 },		// ASCII=0x6C
	{ 0x00, 0x7C, 0x04, 0x04, 0x7C, 0x04, 0x04, 0x78 },		// ASCII=0x6D
	{ 0x00, 0x00, 0x7C, 0x04, 0x04, 0x78, 0x00, 0x00 },		// ASCII=0x6E
	{ 0x00, 0x00, 0x38, 0x44, 0x44, 0x38, 0x00, 0x00 },		// ASCII=0x6F
	{ 0x00, 0x00, 0xFE, 0x22, 0x22, 0x1C, 0x00, 0x00 },		// ASCII=0x70
	{ 0x00, 0x00, 0x1C, 0x22, 0x22, 0xFE, 0x00, 0x00 },		// ASCII=0x71
	{ 0x00, 0x00, 0x00, 0x7C, 0x04, 0x04, 0x00, 0x00 },		// ASCII=0x72
	{ 0x00, 0x00, 0x48, 0x54, 0x54, 0x24, 0x00, 0x00 },		// ASCII=0x73
	{ 0x00, 0x00, 0x00, 0x04, 0x7E, 0x44, 0x00, 0x00 },		// ASCII=0x74
	{ 0x00, 0x00, 0x3C, 0x40, 0x40, 0x7C, 0x00, 0x00 },		// ASCII=0x75
	{ 0x00, 0x00, 0x04, 0x38, 0x40, 0x38, 0x04, 0x00 },		// ASCII=0x76
	{ 0x00, 0x00, 0x3C, 0x40, 0x3C, 0x40, 0x3C, 0x00 },		// ASCII=0x77
	{ 0x00, 0x00, 0x44, 0x38, 0x38, 0x44, 0x00, 0x00 },		// ASCII=0x78
	{ 0x00, 0x00, 0x02, 0x9C, 0x60, 0x1C, 0x02, 0x00 },		// ASCII=0x79
	{ 0x00, 0x00, 0x00, 0x64, 0x54, 0x4C, 0x00, 0x00 },		// ASCII=0x7A

};

const unsigned char Font_SPACE[]={ 00, 00, 00, 00, 00, 00, 00, 00 };                            // ASCII=0x20
const unsigned char Font_DEGREE[]={ 0x00, 0x0C, 0x12, 0x12, 0x0C, 0x00, 0x00, 0x00 };           // ASCII=0xA7
const unsigned char Font_EXC[]={ 00, 00, 00, 0x5e, 00, 00, 00, 00 };                            // ASCII=0x21
const unsigned char ClearPaper[]={0};

void wrTxLm6023(unsigned char wrAddr, unsigned char wrData);

// methods ===============================================================================

void scr_open(void)
{
        
        disResetP;
        
		// send out hardware reset pulse
        enResetP;									// delay >1uSec ~ 1.4uSec, under 24MHz cpu clk
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        disResetP;                                      				// some transient ~ 0.4uSec
        asm("nop");                                     				// delay >1uSec ~ 1.2uSec
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        
        	// set LCD bias, default as 1/8 by hw reset pin, show skip
        
		// set power supply control, default close voltage follower, voltage adjustment and voltage higher by hw reset pin, show skip
        wrTxLm6023(0x00, 0x2f);                              				// turn ON all voltage control
        
        	// set register control, i.e contrast
        wrTxLm6023(0x00, 0x94);                              				// set contrast in bit 4 to bit 0 => 0x0f as middle
        
        	// ADC setting, i.e. normal or invert display, default as normal; So skip
        //wrTxLm6023(0x00, 0xa0);
        
        	// turn ON LCD
        wrTxLm6023(0x00, 0xaf);                                				// bit 0 as 1 to ON, 0 as OFF LCD
        
        //wrTxLm6023(0x00, 0xa7);
        //wrTxLm6023(0x00, 0xa5);                              				// ON all pixel
        
}

        //----------------------------------------------------------------------------
        // Description : write data/command into LCD module
        // parameters : 
        //      wrPara : bit 7 ( , , , , , , , A0) bit 0
        //      wrData : Tx data
        //----------------------------------------------------------------------------
void wrTxLm6023(unsigned char wrAddr, unsigned char wrData) {
        
        unsigned char i;
        
	cli();										// disable interrupt, make sure the clock time
	
        // prepare data port for pull-high
        
        // chip select                                                  all timing is cal. under 5V supply
        enCsP;                                                  // <-| Min. 30 ns from LCD; 24MHz cpu Min. as 4.6us
                                                                //   |
        // A0                                                        |
        if (wrAddr&0x01) highA0P;                               //   |  <-| Min. 50 ns from LCD; 24MHz cpu Min. as 3.6us
        else lowA0P;                                            //   |    |
                                                                //   |    |
        // serial tx data for 8 cycle                                |    |
        for (i=0;i<8;i++) {                                     //   |    |
                                                                //   |    |
                // low serial data clock                             |    |
                lowSClk;                                        //   |    |     <-| Min. 250 ns from LCD; 24MHz cpu Min. as 3.2us       <-| Min. 70 ns from LCD; 24MHz cpu Min. as 1.8us        
                                                                //   |    |       |                                                       |
                // prepare data                                      |    |       |                                                       |
                if (wrData&0x80) highSD;                        //   |    |       ...                                                     |                                                     <-| Min. 100 ns from LCD; 24MHz cpu Min. as 0.8us
                else lowSD;                                     //   |    |       ...                                                     |                                                       | 
                wrData <<= 1;                                   //   |    |       |                                                       |                                                       | 
                                                                //   |    |     <-| next cycle sclk changing,                             |                                                       |  
                // high serial data clock                            |    |                                                               |                                                       | 
                highSClk;                                       // <-|  <-|  <-| Min. 100ns from LCD; 24MHz cpu Min. as 1.2us           <-|   <-| Min. 500ns from LCD; 24MHz cpu Min. as 5us    <-|   <-| Min. 100ns from LCD; 24MHz cpu Min. as 1.4us  <-| Min. 100ns from LCD; 24MHz cpu Min. as 2.4us
                                                                //             |                                                                |                                                       |                                                 |
        }                                                       //             |                                                                |                                                       |                                                 |
                                                                //             |                                                                ...                                                     ...                                               ...
        disCsP;                                                 //           <-|                                                                ...                                                     ...                                               ...
                                                                //                                                                              |                                                       |                                                 |
                                                                //                                                                            <-| next cycle A0 changing                              <-| next cycle SCLK changing                      <-| next cycle SCLK changing
        
	sti();
}

/*
 * Draw a rectangular area in the specified drawable using the specified
 * graphics, as determined by the specified bit map.  This differs from
 * rectangle drawing in that the rectangle is drawn using the foreground
 * color and possibly the background color as determined by the bit map.
 * Each row of bits is aligned to the next bitmap word boundary (so there
 * is padding at the end of the row).  The background bit values are only
 * written if the usebackground flag is set in the GC.
 *
 *	for whole screen only temporary
 */
void
GrBitmapDrv(const unsigned char* pData)
{
        unsigned char pageNo;
        unsigned char tmpByte;
        unsigned int tmpInt;
        unsigned int dataCounter;
        
        // init.
        dataCounter = 0;
        
        // each page
        for (pageNo=0;pageNo<8;pageNo++) {
        
                tmpByte = 0xb0 | pageNo;                                // set page address
                wrTxLm6023(0x00, tmpByte);
                
                wrTxLm6023(0x00, 0x10);                                      // set coloumn address (higher) at 0 position
                wrTxLm6023(0x00, 0x00);                                      // set coloumn address (lower)
                
                // each segment in one page (adjust the code for more efficiency later)
                tmpInt = (pageNo+1)*128;
                for (;dataCounter<tmpInt;dataCounter++) {
                        if (pData!=ClearPaper) wrTxLm6023(0x01, *(pData+dataCounter));              // set data, coloumn address will auto. inc. inside LCD
                        else wrTxLm6023(0x01, 0);                                    // clear screen
                }
                        
        }
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
        const unsigned char* p;
	
		wrTxLm6023(0x00, 0xb0 | y);                                  // set page address
		
		x *= 8;                                                 // set coloumn address 
		wrTxLm6023(0x00, 0x0f & x );                                         // lower
		wrTxLm6023(0x00, ((x>>4) & 0x1f) | 0x10 );                           // higher
		
		switch (font_code) {
			case 0x20 : 
				 p = Font_SPACE;
				break;
			case 0xa7 : 
				 p = Font_DEGREE;
				break;
			case 0x21 : 
				 p = Font_EXC;
				break;
			default : 
				 p = Font_Table[font_code-0x2A];
				 //p = Font_Table[font_code-0x2E];
				break;
		}
		
		for (i=0;i<8;i++)                                       // wr. 8 bytes data for one fonts (8x8 in size), set data, coloumn address will auto. inc. inside LCD
			//if (flags&0x01) {
			if (flags) {
				wrTxLm6023(0x01, ~(*(p+i)) );
			} else {
				wrTxLm6023(0x01, *(p+i));
			}
}
