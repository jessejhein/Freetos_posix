/*
 * header of semaphore library
 *
 * History
 * Date		author		description
 * 04-11-2004	yan		create
 */
 
extern void sem_init(char* sem_var, unsigned char init_val);

// return 
//	-ve -- semaphore busy
//	>=0 -- semaphore ready to use
extern char sem_wait(char* sem_var);

extern void sem_post(char* sem_var);

