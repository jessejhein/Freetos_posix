/*
 * Copyright (c) 2004 cheng chung yan <chungyan5@hkem.com / yan@amonics.com>
 *
 */

#include "nano-X.h"

/*
 * Create new window with passed style, title and location.
 */
GR_WINDOW_ID
GrNewWindowEx(GR_WINDOW_ID parent,
	      GR_COORD x, GR_COORD y, GR_SIZE width, GR_SIZE height,
	      GR_COLOR background)
{
	GR_WINDOW_ID wid;
	GR_WM_PROPERTIES wmprops;

	/* create window with no borders */
	wid = GrNewWindow(parent, x, y, width, height, 0, background, BLACK);
	return wid;
}


