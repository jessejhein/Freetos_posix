/*
 * Copyright (c) 2004 yan <yan@amonics.com>
 *
 * Device-independent keyboard routines
 */
 
#include "kb.h"

extern unsigned char raw_key_code;

void kb_softirq(void)
{
	raw_key_code
	
	switch (raw_key_code) {
		case ENTER_KEY_CODE :
			 if (ENTER_KEY_DETECT) wr_event(GR_EVENT_TYPE_KEY_DOWN);
			 else wr_event(GR_EVENT_TYPE_KEY_UP);
			break;
		case UPPER_KEY_DETECT : 
			 if (UPPER_KEY_DETECT) wr_event(GR_EVENT_TYPE_KEY_DOWN);
			 else wr_event(GR_EVENT_TYPE_KEY_UP);
			break;
	}
	
}


