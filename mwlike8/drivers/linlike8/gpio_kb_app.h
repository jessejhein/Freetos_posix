/*
 *	linlike8/test_apps/gpio_kb_app.h
 *
 *	14-06-2004	yan	gpio module for keyboard application part in gui mwlike8
 *
 *	Remarks
 *		must place after
 *			app.h
 */

//#include "app.h"									// linlike8 os

#if (KB_MOD==1)
#include "nano-X.h"									// GR_EVENT
#include "cirbuf.h"									// pre_wr_cir254buf()

#define	APP_UP_KEY_DOWN_CASE	do {							\
		unsigned char i;							\
		if ((i=pre_wr_cir254buf(ptr_events_vect.wr,ptr_events_vect.rd,NR_EVENT))!=255) {	\
			events_vect[ptr_events_vect.wr].type = GR_EVENT_TYPE_KEY_DOWN;			\
			events_vect[ptr_events_vect.wr].keystroke.ch = GR_SPECIAL_KEY_ARROW_UP;		\
			ptr_events_vect.wr = i;								\
		}											\
	} while (0)
#define	APP_DOWN_KEY_DOWN_CASE	do {							\
		unsigned char i;							\
		if ((i=pre_wr_cir254buf(ptr_events_vect.wr,ptr_events_vect.rd,NR_EVENT))!=255) {	\
			events_vect[ptr_events_vect.wr].type = GR_EVENT_TYPE_KEY_DOWN;			\
			events_vect[ptr_events_vect.wr].keystroke.ch = GR_SPECIAL_KEY_ARROW_DOWN;	\
			ptr_events_vect.wr = i;								\
		}											\
	} while (0)
#define	APP_ENTER_KEY_DOWN_CASE	do {							\
		unsigned char i;							\
		if ((i=pre_wr_cir254buf(ptr_events_vect.wr,ptr_events_vect.rd,NR_EVENT))!=255) {\
			events_vect[ptr_events_vect.wr].type = GR_EVENT_TYPE_KEY_DOWN;	\
			events_vect[ptr_events_vect.wr].keystroke.ch = GR_KEY_ENTER;	\
			ptr_events_vect.wr = i;						\
		}									\
	} while (0)
#define	APP_UP_KEY_UP_CASE	do {							\
		unsigned char i;							\
		if ((i=pre_wr_cir254buf(ptr_events_vect.wr,ptr_events_vect.rd,NR_EVENT))!=255) {\
			events_vect[ptr_events_vect.wr].type = GR_EVENT_TYPE_KEY_UP;	\
			events_vect[ptr_events_vect.wr].keystroke.ch = GR_SPECIAL_KEY_ARROW_UP;\
			ptr_events_vect.wr = i;						\
		}									\
	} while (0)
#define	APP_DOWN_KEY_UP_CASE	do {							\
		unsigned char i;							\
		if ((i=pre_wr_cir254buf(ptr_events_vect.wr,ptr_events_vect.rd,NR_EVENT))!=255) {	\
			events_vect[ptr_events_vect.wr].type = GR_EVENT_TYPE_KEY_UP;			\
			events_vect[ptr_events_vect.wr].keystroke.ch = GR_SPECIAL_KEY_ARROW_DOWN;	\
			ptr_events_vect.wr = i;								\
		}											\
	} while (0)
#define	APP_ENTER_KEY_UP_CASE	do {							\
		unsigned char i;							\
		if ((i=pre_wr_cir254buf(ptr_events_vect.wr,ptr_events_vect.rd,NR_EVENT))!=255) {\
			events_vect[ptr_events_vect.wr].type = GR_EVENT_TYPE_KEY_UP;	\
			events_vect[ptr_events_vect.wr].keystroke.ch = GR_KEY_ENTER;	\
			ptr_events_vect.wr = i;						\
		}									\
	} while (0)
#endif

