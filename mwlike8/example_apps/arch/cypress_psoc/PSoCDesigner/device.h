#ifndef _DEVICE_H
#define _DEVICE_H
/*
 * Copyright (c) 1999, 2000, 2001, 2002, 2003 Greg Haerr <greg@censoft.com>
 * Portions Copyright (c) 2002 Koninklijke Philips Electronics
 * Portions Copyright (c) 2002 Koninklijke Philips Electronics
 * Copyright (c) 05-05-2004 cheng chung yan <chungyan5@hkem.com / yan@amonics.com> for 8-bit system
 *
 * Engine-level Screen, Mouse and Keyboard device driver API's and types
 *
 * Contents of this file are not for general export
 *
 * must place after nano-X.h
 */
 
	// init. the lcd following manufacturor spec.
extern void scr_open(void);
extern void GrBitmapDrv(const unsigned char* pData);
extern void GrTextDrv(GR_COORD x, GR_COORD y, unsigned char font_code, GR_TEXTFLAGS flags);

//extern void kb_open(void);
//extern void kb_read(void);

#endif /*_DEVICE_H*/
