/*
 * Copyright (c) 2004 cheng chung yan <chungyan5@hkem.com>
 *
 * Permission is granted to use, distribute, or modify this source,
 * provided that this copyright notice remains intact.
 *
 * Client routines to do graphics with windows and graphics contexts.
 *
 * Whenever you add a new API entry point, please comment it in the same way
 * as the rest of the functions in this file. Also add your functions to the
 * appropriate section(s) in doc/nano-X/nano-X-sections.txt and regenerate the
 * documentation by running make in the doc/nano-X/ directory.
 *
 */

/**
 * Recursively maps (makes visible) the specified window and all of the
 * child windows which have a sufficient map count. The border and background
 * of the window are painted, and an exposure event is generated for the
 * window and every child which becomes visible.
 *
 * @param wid  the ID of the window to map
 *
 * @ingroup nanox_window
 */
/*void 
GrMapWindow(GR_WINDOW_ID wid)
{
}*/




//#include "nano-X.h"

// following coding is temporary placing here, after understand about coding in server, put into another right place
// server coding ******************************************************************************************************
//#define	MAX_GC	1 
//struct graphic_context {
//	unsigned char id;								// id of this graphics context
//} gc_vec[MAX_GC];
// server coding ******************************************************************************************************

/**
 * Open a connection to the graphics server.
 * this subr be changed from return int to return char, since 8-bit system
 * Since no server concept in existing version, so ignore this for save code
 *
 * @return the fd of the connection to the server or -1 on failure
 *
 */
/*char 
GrOpen(void)
{
	return 0;
}*/

/**
 * Creates a new graphics context structure. The structure is initialised
 * with a set of default parameters.
 *
 * @return The ID of the newly created graphics context or 0 on error.
 *
 * @ingroup nanox_draw
 */
/*GR_GC_ID 
GrNewGC(void)
{
	return 0;
}*/

/**
 * Create a new window.
 *
 * @param parent      The ID of the parent window.
 * @param x           The X coordinate of the new window relative to the parent window.
 * @param y           The Y coordinate of the new window relative to the parent window.
 * @param width       The width of the new window.
 * @param height      The height of the new window.
 * @param bordersize  The width of the window border.
 * @param background  The color of the window background.
 * @param bordercolor The color of the window border.
 * @return            The ID of the newly created window.
 *
 * @ingroup nanox_window
 */
/*GR_WINDOW_ID
GrNewWindow(GR_WINDOW_ID parent, GR_COORD x, GR_COORD y, GR_SIZE width,
	GR_SIZE height, GR_SIZE bordersize, GR_COLOR background,
	GR_COLOR bordercolor)
{
	nxNewWindowReq *req;
	GR_WINDOW_ID 	wid;

	LOCK(&nxGlobalLock);
	req = AllocReq(NewWindow);
	req->parentid = parent;
	req->x = x;
	req->y = y;
	req->width = width;
	req->height = height;
	req->backgroundcolor = background;
	req->bordercolor = bordercolor;
	req->bordersize = bordersize;
	if(TypedReadBlock(&wid, sizeof(wid),GrNumNewWindow) == -1)
		wid = 0;
	UNLOCK(&nxGlobalLock);
	return wid;
}*/
