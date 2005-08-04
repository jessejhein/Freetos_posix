/*
 * header of semaphore library
 *
 * History
 * Date		author		description
 * 04-11-2004	yan		create
 */

// put the init. value into semaphore variable
extern void sem_init(signed char* sem_var, unsigned char init_val);

// return 
//	-ve -- semaphore busy
//	>=0 -- semaphore ready to use
extern signed char sem_trywait(signed char* sem_var);

extern void sem_post(signed char* sem_var);

