/*
 *	include/linlike8/timer.h
 *
 *	03-05-2004	yan	create this following linux
 *
 *	must place after 
 *		app.h - due to TIMER_MOD
 *		sched.h - due to p_func
 *
 */

#if (TIMER_MOD==1)

extern unsigned char jiffies;
struct timer_list {
	unsigned char expires;								// 8-bit, since must follow jiffies
	union {
		unsigned int timer_data;						// usually, RAM data pointer as 16-bit in 8-bit system
		p_func timer_function;
	} data;
};

#if (NR_TIMER_OUT>0)
extern struct timer_list* timer_vector[];

#define del_timer(timer_num)		timer_vector[timer_num] = 0
// parameters
//	max. timer delay or sleep is 255*10mSec = 2.55 Sec
#define sleep(sec)			schedule_timeout(sec*100)
#define msleep(millisec)		schedule_timeout(millisec/10)
#define alarm(function,sec,timer)	malarm(function,sec*1000,timer)
#define malarm(function,msec,timer)	_malarm(function,msec/10,timer)
extern char _malarm(p_func function, unsigned char jiffies_cnt, struct timer_list* timer);
#endif
#endif

