/*
 *	include/linlike8/time.h
 *
 *	27-04-2004	yan	create this following linux
 *
 *	must place after app.h
 *
 */

#if (TIMER_MOD==1)

// do not use this, since all lib calling is static, and programmer can check in compile time, so do not waste code to do verification
//---------------------------------------------------------------
//#define	MAX_JIFFY_OFFSET	((~0UL)-1)						// for 32 bit unsigned long type
//static unsigned long timespec_to_jiffies(unsigned char sec)
//{
//	if (sec >= (MAX_JIFFY_OFFSET / HZ)) return MAX_JIFFY_OFFSET;
//	return HZ * sec;
//}

#endif

