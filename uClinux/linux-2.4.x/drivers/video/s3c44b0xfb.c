/*
 *  linux/drivers/video/s3c44b0fb.c
 *
 *  tpu.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Framebuffer driver for the S3C44B0X processors.
 */
//#define	LCD_GRAY_16

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/delay.h>

#include <video/fbcon.h>
#ifdef	LCD_GRAY_16
#include <video/fbcon-cfb4.h>
#else
#include <video/fbcon-cfb8.h>
#endif

#include <asm/hardware.h>
#include <asm/mach-types.h>
#include <asm/uaccess.h>
#include <asm/linux_logo.h>

static struct s3c44b0fb_info {
	struct fb_info		fb;
	int			currcon;
} *cfb;

#define CMAP_SIZE	256

/*
 *    Set a single color register. Return != 0 for invalid regno.
 */
static int
s3c44b0fb_setcolreg(u_int regno, u_int red, u_int green, u_int blue,
		     u_int transp, struct fb_info *_info)
{
	//TODO
        /*struct s3c44b0xfb_info *fbi = (struct s3c44b0xfb_info *)info;
	//struct display *disp = get_con_display(info, fbi->currcon); 
	//if (disp->inverse) { 
	red = 0x100 - red; 
	green = 0x100 - green; 
	blue = 0x100 - blue; 
	//} 
	struct s3c44b0xfb_info *info = (struct s3c44b0xfb_info *) _info;

	unsigned int helpvalue;
	u32 blueval, redval, greenval;
	blueval = inw(S3C44B0X_BLUELUT);
	redval = inw(S3C44B0X_REDLUT);
	greenval = inw(S3C44B0X_GREENLUT);
	switch(8)//info->gen.info.var.bits_per_pixel)
	{
	case 1:
		return -EINVAL;
		break;
	case 2:
		if((regno > 4) || (red != green) || (red != blue)) // FIXME: maybe wrong 
			return 1;
		helpvalue = (unsigned int)(blue / 0x1111) & 0xF;
		blueval &= ~(0xF << (regno<<2));
		blueval |= (helpvalue << (regno<<2));
		outl(blueval, S3C44B0X_BLUELUT);
		break;
	case 4:
		return -EINVAL;
		break;
	case 8:
		if(regno > 256)
			return 1;
		helpvalue = (unsigned int)(blue / 0x1111) & 0xF;
		blueval &= ~(0xF << ((regno&0x03)<<2));
		blueval |= (helpvalue << ((regno&0x03)<<2));
		outl(blueval, S3C44B0X_BLUELUT);
		helpvalue = (unsigned int)(red / 0x1111) & 0xF;
		redval &= ~(0xF << (((regno>>5)&0x7)<<2));
		redval |= (helpvalue << (((regno>>5)&0x7)<<2));
		outl(redval, S3C44B0X_REDLUT);
		helpvalue = (unsigned int)(green / 0x1111) & 0xF;
		greenval &= ~(0xF << (((regno>>2)&0x7)<<2));
		greenval |= (helpvalue << (((regno>>2)&0x7)<<2));
		outl(greenval, S3C44B0X_GREENLUT);
		break;
	default:
		return 1;
	}*/
		
		
    return 0;
}
		    
/*
 * Set the colormap
 */
static int
s3c44b0fb_set_cmap(struct fb_cmap *cmap, int kspc, int con,
		    struct fb_info *info)
{
	struct s3c44b0fb_info *cfb = (struct s3c44b0fb_info *)info;
	struct fb_cmap *dcmap = &fb_display[con].cmap;
	int err = 0;

	/* no colormap allocated? */
	if (!dcmap->len)
		err = fb_alloc_cmap(dcmap, CMAP_SIZE, 0);

	if (!err && con == cfb->currcon) {
		err = fb_set_cmap(cmap, kspc, s3c44b0fb_setcolreg, &cfb->fb);
		dcmap = &cfb->fb.cmap;
	}

	if (!err)
		fb_copy_cmap(cmap, dcmap, kspc ? 0 : 1);

	return err;
}

/*
 *    Set the User Defined Part of the Display
 */
static int
s3c44b0fb_set_var(struct fb_var_screeninfo *var, int con,
		   struct fb_info *info)
{
	struct display *display;
	unsigned int lcdcon, syscon;
	int chgvar = 0;

	if (var->activate & FB_ACTIVATE_TEST)
		return 0;

	if ((var->activate & FB_ACTIVATE_MASK) != FB_ACTIVATE_NOW)
		return -EINVAL;

	if (cfb->fb.var.xres != var->xres)
		chgvar = 1;
	if (cfb->fb.var.yres != var->yres)
		chgvar = 1;
	if (cfb->fb.var.xres_virtual != var->xres_virtual)
		chgvar = 1;
	if (cfb->fb.var.yres_virtual != var->yres_virtual)
		chgvar = 1;
	if (cfb->fb.var.bits_per_pixel != var->bits_per_pixel)
		chgvar = 1;

	if (con < 0) {
		display = cfb->fb.disp;
		chgvar = 0;
	} else {
		display = fb_display + con;
	}

	var->transp.msb_right	= 0;
	var->transp.offset	= 0;
	var->transp.length	= 0;
	var->red.msb_right	= 0;
	var->red.offset		= 5;
	var->red.length		= 3;
	var->green.msb_right	= 0;
	var->green.offset	= 2;
	var->green.length	= 3;
	var->blue.msb_right	= 0;
	var->blue.offset	= 0;
	var->blue.length	= 2;

	switch (var->bits_per_pixel) {
#ifdef FBCON_HAS_MFB
	case 1:
		cfb->fb.fix.visual	= FB_VISUAL_MONO01;
		display->dispsw		= &fbcon_mfb;
		display->dispsw_data	= NULL;
		break;
#endif
#ifdef FBCON_HAS_CFB2
	case 2:
		cfb->fb.fix.visual	= FB_VISUAL_PSEUDOCOLOR;
		display->dispsw		= &fbcon_cfb2;
		display->dispsw_data	= NULL;
		break;
#endif
#ifdef FBCON_HAS_CFB4
	case 4:
		cfb->fb.fix.visual	= FB_VISUAL_PSEUDOCOLOR;
		display->dispsw		= &fbcon_cfb4;
		display->dispsw_data	= NULL;
		break;
#endif
#ifdef FBCON_HAS_CFB8
	case 8:
		cfb->fb.fix.visual	= FB_VISUAL_PSEUDOCOLOR;//FB_VISUAL_TRUECOLOR;
		display->dispsw		= &fbcon_cfb8;
		display->dispsw_data	= NULL;
		break;
#endif
	default:
		return -EINVAL;
	}

	display->next_line	= var->xres_virtual * var->bits_per_pixel / 8;

	cfb->fb.fix.line_length = display->next_line;

	display->screen_base	= cfb->fb.screen_base;
	display->line_length	= cfb->fb.fix.line_length;
	display->visual		= cfb->fb.fix.visual;
	display->type		= cfb->fb.fix.type;
	display->type_aux	= cfb->fb.fix.type_aux;
	display->ypanstep	= cfb->fb.fix.ypanstep;
	display->ywrapstep	= cfb->fb.fix.ywrapstep;
	display->can_soft_blank = 1;
	display->inverse	= 0;

	cfb->fb.var		= *var;
	cfb->fb.var.activate	&= ~FB_ACTIVATE_ALL;

	/*
	 * Update the old var.  The fbcon drivers still use this.
	 * Once they are using cfb->fb.var, this can be dropped.
	 *                                      --rmk
	 */
	display->var		= cfb->fb.var;

	/*
	 * If we are setting all the virtual consoles, also set the
	 * defaults used to create new consoles.
	 */
	if (var->activate & FB_ACTIVATE_ALL)
		cfb->fb.disp->var = cfb->fb.var;

	if (chgvar && info && cfb->fb.changevar)
		cfb->fb.changevar(con);


	fb_set_cmap(&cfb->fb.cmap, 1, s3c44b0fb_setcolreg, &cfb->fb);

	return 0;
}

/*
 * Get the currently displayed virtual consoles colormap.
 */
static int
gen_get_cmap(struct fb_cmap *cmap, int kspc, int con, struct fb_info *info)
{
	fb_copy_cmap(&info->cmap, cmap, kspc ? 0 : 2);
	return 0;
}

/*
 * Get the currently displayed virtual consoles fixed part of the display.
 */
static int
gen_get_fix(struct fb_fix_screeninfo *fix, int con, struct fb_info *info)
{
	*fix = info->fix;
	return 0;
}

/*
 * Get the current user defined part of the display.
 */
static int
gen_get_var(struct fb_var_screeninfo *var, int con, struct fb_info *info)
{
	*var = info->var;
	return 0;
}

static struct fb_ops s3c44b0fb_ops = {
	owner:		THIS_MODULE,
	fb_set_var:	s3c44b0fb_set_var,
	fb_set_cmap:	s3c44b0fb_set_cmap,
	fb_get_fix:	gen_get_fix,
	fb_get_var:	gen_get_var,
	fb_get_cmap:	gen_get_cmap,
};

static int s3c44b0fb_updatevar(int con, struct fb_info *info)
{
	return -EINVAL;
}

static void s3c44b0fb_blank(int blank, struct fb_info *info)
{
	//TODO
    	if (blank) {
	} else {
	}
}

/********************************************************************************/
#define	U8	__u8
#define	U16	__u16	 
#define	U32	__u32
#define	MAX749_CTRL	0x80
#define	MAX749_ADJ	0x40
#define	Max749AdjHi()	outl(inl(S3C44B0X_PDATC)|MAX749_ADJ, S3C44B0X_PDATC)
#define	Max749AdjLo()	outl(inl(S3C44B0X_PDATC)&~MAX749_ADJ, S3C44B0X_PDATC)
#define	Max749CtrlHi()	outl(inl(S3C44B0X_PDATC)|MAX749_CTRL, S3C44B0X_PDATC)
#define	Max749CtrlLo()	outl(inl(S3C44B0X_PDATC)&~MAX749_CTRL, S3C44B0X_PDATC)

static void Max749Rst(void)
{
	unsigned char i;
	Max749AdjHi();
	Max749CtrlLo();
	i = 100;
	while(i--);	
	Max749CtrlHi(); 
}

static void Max749Up(U16 cnt)
{	
	unsigned char i;
	
	Max749Rst();
	for(; cnt; cnt--)
	{
		Max749AdjLo();
		i = 10;
		while(i--);
		Max749AdjHi();
		i = 10;
		while(i--);
	}
}

#define	SCR_XSIZE	320
#define	SCR_YSIZE	240
#define	LCD_XSIZE	320
#define	LCD_YSIZE	240

#ifdef	LCD_GRAY_16
#define	LCD_DEPTH	4
#define	LCD_BUF_SIZE	((SCR_XSIZE*SCR_YSIZE)>>1)
#else
#define	LCD_DEPTH	8
#define	LCD_BUF_SIZE	((SCR_XSIZE*SCR_YSIZE))
#endif

/////////////////////	LCDCON1
#define	CLKVAL		(4<<12)		//VCLK = MCLK/(2*CLKVAL)
#define	WLH		(3<<10)		//16 clock
#define	WDLY		(3<<8)		//16 clock
#define	MMODE		(0<<7)		//VM = ecah frame, not use MVAL
#define	DISMODE		(2<<5)	//8 bit single scan
#define	INVCLK		(0<<4)		//falling edge fetch data
#define	INVLINE		(0<<3)		//normal, non inverse
#define	INVFRAME	(0<<2)		//normal
#define	INVVD		(0<<1)		//normal
#define	ENVID		1
#define	DSVID		0

#define	LCD_MODE_DS	(CLKVAL|WLH|WDLY|MMODE|DISMODE|INVCLK|INVLINE|INVFRAME|INVVD|DSVID)
#define	LCD_MODE_EN	(CLKVAL|WLH|WDLY|MMODE|DISMODE|INVCLK|INVLINE|INVFRAME|INVVD|ENVID)
/////////////////////	LCDCON2
#define	LINEBLINK	       10
#define   HOZVAL_COLOR	(LCD_XSIZE*3/8-1)
#define   LINEVAL			(LCD_YSIZE-1)
/////////////////////	LCDCON3
#define	SELFREF_EN	1
#define	SELFREF_DS	0
/////////////////////	LCDSADDR1
#define	MONO_MODE	0
#define	G4_MODE		1
#define	G16_MODE	2
#define	C8_MODE		3
#define	BankOfDisMem(addr)	(((U32)(addr)>>22)<<21)
#define	BaseOfDisMem(addr)	(((U32)(addr)>>1)&0x1fffff)
/////////////////////	LCDSADDR2
#define	BSWP_EN		1
#define	BSWP_DS		0
#define	MVAL		13
/////////////////////	LCDSADDR3
#define	MONO_VS_SIZE	(LCD_XSIZE/16)|(((SCR_XSIZE-LCD_XSIZE)/16)<<9)
#define	G4_VS_SIZE		(LCD_XSIZE/8)|(((SCR_XSIZE-LCD_XSIZE)/8)<<9)
#define	G16_VS_SIZE		(LCD_XSIZE/4)|(((SCR_XSIZE-LCD_XSIZE)/4)<<9)
#define	C8_VS_SIZE		(LCD_XSIZE/2)|(((SCR_XSIZE-LCD_XSIZE)/2)<<9)

int __init s3c44b0xfb_init(void)
{
	int err = -ENOMEM;
	char *fbuf;
        //------绘图测试-------
        unsigned char *src,*dst;
        unsigned char x1, y1;
        //---------------------

	cfb = kmalloc(sizeof(*cfb) + sizeof(struct display), GFP_KERNEL);
	if (!cfb)
		goto out;
	memset(cfb, 0, sizeof(*cfb) + sizeof(struct display));
	
	fbuf = kmalloc(LCD_BUF_SIZE, GFP_KERNEL);
	if(!fbuf){
		kfree(cfb);
		goto out;
	}
	memset(fbuf, 0, LCD_BUF_SIZE);

	cfb->currcon		= -1;

	strcpy(cfb->fb.fix.id, "s3c44b0");
	cfb->fb.screen_base	= fbuf;
	cfb->fb.fix.smem_start	= fbuf;
	cfb->fb.fix.smem_len	= LCD_BUF_SIZE;
	cfb->fb.fix.type	= FB_TYPE_PACKED_PIXELS;

	cfb->fb.var.xres	   = LCD_XSIZE;
	cfb->fb.var.xres_virtual   = SCR_XSIZE;
	cfb->fb.var.yres	   = LCD_YSIZE;
	cfb->fb.var.yres_virtual   = SCR_YSIZE;
	cfb->fb.var.bits_per_pixel = LCD_DEPTH;
	cfb->fb.var.grayscale   = 1;
	cfb->fb.var.activate	= FB_ACTIVATE_NOW;
	cfb->fb.var.height	= -1;
	cfb->fb.var.width	= -1;

	cfb->fb.fbops		= &s3c44b0fb_ops;
	cfb->fb.changevar	= NULL;
//	cfb->fb.switch_con	= s3c44b0fb_switch;
	cfb->fb.updatevar	= s3c44b0fb_updatevar;
	cfb->fb.blank		= s3c44b0fb_blank;
	cfb->fb.flags		= FBINFO_FLAG_DEFAULT;
	cfb->fb.disp		= (struct display *)(cfb + 1);

	fb_alloc_cmap(&cfb->fb.cmap, CMAP_SIZE, 0);

	/*
	 * Power up the LCD
	 */
#ifdef	LCD_GRAY_16
	Max749Up(23);
	outl(0xaaaa, S3C44B0X_PCOND);		//VFRAME,VM,VLINE,VCLK,VD3-VD0 enable
	outl(LCD_MODE_DS, S3C44B0X_LCDCON1);
	outl((LINEBLINK<<21)|(((LCD_XSIZE>>2)-1)<<10)|(LCD_YSIZE-1), S3C44B0X_LCDCON2);
	outl((G16_MODE<<27)|BankOfDisMem(fbuf)|BaseOfDisMem(fbuf), S3C44B0X_LCDSADDR1);
	outl((BSWP_EN<<29)|(MVAL<<21)|BaseOfDisMem((U32)fbuf+SCR_XSIZE*LCD_YSIZE/2), S3C44B0X_LCDSADDR2);
	outl(G16_VS_SIZE, S3C44B0X_LCDSADDR3);
	
	outl(inl(S3C44B0X_PDATC)&~0x100, S3C44B0X_PDATC);	// set GPC8 low
#else
/*	Max749Up(23);
	PCOND     = 0xaaaa;	//VFRAME,VM,VLINE,VCLK,VD3-VD0 enable
	LCDCON1   = LCD_MODE_DS;		
	LCDCON2   = (LINEBLINK<<21)|((LCD_XSIZE/4-1)<<10)|(LCD_YSIZE-1);
	LCDSADDR1 = (G16_MODE<<27)|BankOfDisMem(fbuf)|BaseOfDisMem(fbuf);		
	LCDSADDR2 = (BSWP_EN<<29)|(MVAL<<21)|BaseOfDisMem((U32)fbuf+SCR_XSIZE*LCD_YSIZE/2);
	LCDSADDR3 = G16_VS_SIZE;*/

//	outl(inl(S3C44B0X_PCONC)&~0xff00, S3C44B0X_PCONC);
//	outl(inl(S3C44B0X_PCONC)|0xff00, S3C44B0X_PCONC);	//GPC4-GPC7 as VD7-VD4
	outl(0xaaaa, S3C44B0X_PCOND);				//VFRAME,VM,VLINE,VCLK,VD3-VD0
//	outl(inl(S3C44B0X_PDATC)&~0x100, S3C44B0X_PDATC);	// set GPC8 low
	
	outl(LCD_MODE_DS, S3C44B0X_LCDCON1);
       outl((LINEVAL)|(HOZVAL_COLOR<<10)|(LINEBLINK<<21),S3C44B0X_LCDCON2);
//	outl((LINEBLINK<<21)|(((LCD_XSIZE>>1)-1)<<10)|(LCD_YSIZE-1), S3C44B0X_LCDCON2);
	outl((C8_MODE<<27)|((int)fbuf>>1), S3C44B0X_LCDSADDR1);
	outl(((((int)fbuf+LCD_XSIZE*LCD_YSIZE)>>1)&0x1fffff)|(MVAL<<21)|(BSWP_EN<<29), S3C44B0X_LCDSADDR2);
	outl(C8_VS_SIZE, S3C44B0X_LCDSADDR3);
	outl(0xfdb96420, S3C44B0X_REDLUT);
	outl(0xfdb96420, S3C44B0X_GREENLUT);
	outl(0xfb40, S3C44B0X_BLUELUT);
#endif
	outl(inl(S3C44B0X_LCDCON1)|ENVID, S3C44B0X_LCDCON1);
        outl(inl(S3C44B0X_PDATE)&0xfe,S3C44B0X_PDATE);
	printk("LCD buffer : %p\n", fbuf);

	s3c44b0fb_set_var(&cfb->fb.var, -1, &cfb->fb);
	err = register_framebuffer(&cfb->fb);
//	printk("err=%d\n", err);
        //-----------------绘图测试(绘制一个小企鹅)----------------------
        
	src = linux_logo;
	for( y1 = 0; y1 < 80; y1++ ) {
		dst = fbuf + y1*320 + 120;
		for( x1 = 0; x1 < 80; x1++ )
		{
                    fb_writeb (((((*src)&0x7)<<5)+(((*src)&0x38)>>1)+(((*src)&0xc0)>>6)), dst++);
                    src++;
                }
	}
        mdelay(3000);
        //--------------------------------------------------------------

out:	return err;
}

static void __exit s3c44b0xfb_exit(void)
{
	unregister_framebuffer(&cfb->fb);
	kfree(cfb);

}

int __init s3c44b0xfb_setup(char *options)
{
	return 0;
}

#ifdef MODULE
module_init(s3c44b0xfb_init);
module_exit(s3c44b0xfb_exit);
#endif
