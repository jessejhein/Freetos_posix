/*
 * semaphore library
 *
 * History
 * Date		author		description
 * 04-11-2004	yan		create
 */

#define	DEBUG	0

#include "sem.h"

#if (DEBUG==1)
#include <stdio.h>

int main()
{
	char sem;
	
	sem_init(&sem, 1);
	
	if (sem_wait(&sem)>=0) printf("ok 1\n");
	else printf("bad 1\n");
	
	if (sem_wait(&sem)>=0) printf("ok 2\n");
	else printf("bad 2\n");
	
	sem_post(&sem);
	
	if (sem_wait(&sem)>=0) printf("ok 3\n");
	else printf("bad 3\n");
	
	sem_post(&sem);
	
	return 0;
}
#endif

void sem_init(char* sem_var, unsigned char init_val)
{
	*sem_var = init_val;
}

// return 
//	-ve -- semaphore busy
//	>=0 -- semaphore ready to use
char sem_wait(char* sem_var)
{
	if ((*sem_var-1)>=0) return --(*sem_var);
	else return -1;
}

void sem_post(char* sem_var)
{
	++(*sem_var);
}

