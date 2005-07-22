#ifndef	_NANO_X_H
#define	_NANO_X_H
/* 
 * Copyright (c) 05-05-2004 cheng chung yan <yan@amonics.com> for 8-bit system
 * Permission is granted to use, distribute, or modify this source,
 * provided that this copyright notice remains intact.
 *
 * Nano-X public definition header file:  user applications should
 * include only this header file.
 */

#include "config_mwlike8.h"
#include "mwtypes.h"			/* exported engine MW* types*/

/*
 * The following typedefs are inherited from the Microwindows
 * engine layer.
 */
typedef MWCOORD 	GR_COORD;	/* coordinate value */
typedef MWTEXTFLAGS	GR_TEXTFLAGS;	/* text encoding flags */
typedef MWKEY	 	GR_KEY;		/* keystroke value */
typedef MWTIMEOUT	GR_TIMEOUT;	/* timeout value */
//typedef MWIMAGEBITS 	GR_BITMAP;	/* bitmap unit */

/* Basic typedefs. */
typedef char		GR_EVENT_TYPE;	/* event types */
typedef char 		GR_COUNT;	/* number of items */
//typedef unsigned char	GR_BOOL;	/* boolean value */

/**
 * Event for a keystroke typed for the window with has focus.
 */
typedef struct {
//  GR_EVENT_TYPE type;		/**< event type */
  unsigned	type	:3;	/**< event type */
//  GR_KEY ch;			/**< 8-bit unicode key value, MWKEY_xxx */
  unsigned	ch	:2;	/**< keystroke events */
} GR_EVENT_KEYSTROKE;

/**
 * Union of all possible event structures.
 * This is the structure returned by GrGetNextEvent() and similar routines.
 */
typedef union {
//  GR_EVENT_TYPE type;			/**< event type */
  unsigned	type		:3;		/**< event type */
  GR_EVENT_KEYSTROKE keystroke;		/**< keystroke events */
} GR_EVENT;

// function pointer
typedef void const (*p_funct_nano)(void);

/* Public graphics routines. */
//void		GrGetNextEventTimeout(GR_EVENT *ep, GR_TIMEOUT timeout);

/* Event types.
 * Mouse motion is generated for every motion of the mouse, and is used to
 * track the entire history of the mouse (many events and lots of overhead).
 * Mouse position ignores the history of the motion, and only reports the
 * latest position of the mouse by only queuing the latest such event for
 * any single client (good for rubber-banding).
 * due to event_type in wr_event(char), it is char only, so range are -124 to 125
 */
//#define	GR_EVENT_TYPE_ERROR		(-1)
#define	GR_EVENT_TYPE_NONE		0
#define	GR_EVENT_TYPE_EXPOSURE		1
#define	GR_EVENT_TYPE_KEY_DOWN		2
#define	GR_EVENT_TYPE_KEY_UP		3
#define GR_EVENT_TYPE_TIMEOUT		4
#define GR_EVENT_TYPE_TIMEOUT_MANUAL 5

#include "device.h"									// GrBitmapDri()

/* Public graphics routines. */
extern void io_open(void);							// gui i/o
extern	void	GrMapWindow(void);
#if (LCD_MOD==1)
#define 	GrBitmap(pData)			GrBitmapDrv(pData)
#define 	GrText_buf(x, y, str, count, flags)	GrText(x, y, 0, str, count, flags)
#define 	GrText_const(x, y, str, count, flags)	GrText(x, y, str, 0, count, flags)
extern void	GrText(GR_COORD x, GR_COORD y, const unsigned char* str_ROM, unsigned char* str_RAM, GR_COUNT count, GR_TEXTFLAGS flags);
#endif
#if (SWITCH_CASE==0)
extern void	GrGetNextEventTimeout(GR_EVENT *ep, GR_TIMEOUT timeout);
#else
// return 
//	GR_EVENT_TYPE_NONE -- not completed
//	NOT GR_EVENT_TYPE_NONE -- valid event
extern GR_EVENT_TYPE GrGetNextEventTimeout(GR_EVENT *ep, GR_TIMEOUT timeout);
#endif
extern void insert_event_timeout_manual(void);

// static events list
extern GR_EVENT events_vect[];
struct EVENT_PTR {
	unsigned	wr :4;
	unsigned	rd :4;
};
extern struct EVENT_PTR ptr_events_vect;
extern char wr_event(char event_type);

// keyboard
#define 	GR_KEY_ENTER			((unsigned) 0)
#define 	GR_SPECIAL_KEY_ARROW_UP		((unsigned) 1)
#define 	GR_SPECIAL_KEY_ARROW_DOWN	((unsigned) 2)

// IO bit control
//	Remarks : all I/O have not checked the busy flag for save code size, so programmer must pay attention in I/O conflict
struct io_bit_struct {
	//unsigned cnt:5;									// counter, rep. flashing counter
	unsigned led_stat:2;
	unsigned buzzer_stat:1;								// state, machine state
	#if (SWITCH_CASE==1)
	unsigned GrGetNextEventTimeout_status:2;
	#endif
};
extern struct io_bit_struct io_bit_ctrl;
extern unsigned char led_status[];
extern p_funct_nano leds_on_drv[];
extern p_funct_nano leds_off_drv[];
extern p_funct_nano funct_ptr;
#define	BUZZER_OFF		0
#define	BUZZER_POS_PULSE	BUZZER_OFF + 1
#define	BUZZER_POS_PULSE_1	BUZZER_POS_PULSE + 1
//#define	BUZZER_REP_PULSE	BUZZER_POS_PULSE + 1
//#define	BUZZER_REP_PULSE_1	BUZZER_REP_PULSE + 1
//extern struct io_bit_struct buzzer_ctrl;
#define	buz_off()	io_bit_ctrl.buzzer_stat = BUZZER_OFF
#define	buz_one_shot()	io_bit_ctrl.buzzer_stat = BUZZER_POS_PULSE
/*#define	buz_rep_pul(freq) do {							\
				buzzer_ctrl.stat = BUZZER_REP_PULSE;		\
				buzzer_ctrl.cnt = (1/freq/0.05);		\
			} while (0)*/
			// limitation as repeat freq. for 20Hz, 10Hz, 6Hz, 5Hz, 4Hz, 3Hz, 2Hz, 1Hz ...
#define	LED_OFF		0
#define	LED_POS_PULSE	LED_OFF + 1
#define	LED_ON		LED_POS_PULSE + 1
#define	LED_NEG_PULSE	LED_ON + 1
//#define	LED_REP_PULSE	LED_POS_PULSE + 1
//#define	LED_REP_PULSE_1	LED_REP_PULSE + 1
//extern struct io_bit_struct led_ctrl;
#define	led_off(which)			led_status[which] = LED_OFF
#define	led_off_rt(which)	do {								\
								led_off(which);					\
								funct_ptr = leds_off_drv[which];\
								funct_ptr();					\
							} while (0)
#define	led_one_pos_shot(which)	led_status[which] = LED_POS_PULSE
#define	led_one_pos_shot_rt(which)	do {						\
								led_one_pos_shot(which);		\
								funct_ptr = leds_on_drv[which];	\
								funct_ptr();					\
							} while (0)
#define	led_on(which)			led_status[which] = LED_ON
#define	led_on_rt(which)	do {								\
								led_on(which);					\
								funct_ptr = leds_on_drv[which];	\
								funct_ptr();					\
							} while (0)
#define	led_one_neg_shot(which)	led_status[which] = LED_NEG_PULSE
#define	led_one_neg_shot_rt(which)	do {						\
								led_one_neg_shot(which);		\
								funct_ptr = leds_off_drv[which];\
								funct_ptr();					\
							} while (0)
/*#define	led_rep_pul(freq) do {							\
				led_ctrl.stat = BUZZER_REP_PULSE;		\
				led_ctrl.cnt = (1/freq/0.05);			\
			} while (0)*/
#endif /* _NANO_X_H*/
