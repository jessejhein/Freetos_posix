/*
 *	linlike8/include/linlike8/current.h
 *
 *	must palce after sched.h, since task[] declaration from sched.h 1st
 *
 *	09-04-2004	yan	create this following linux
 *
 */

#if (CONTEXT_SW==1)
extern unsigned char current_ptr;
#define	current	(&task[current_ptr])
#else
#define	current	(&task[0])
#endif

