/**
 * \file
 * mutex task employing posix thread and file API
 * \author Dennis Tsang <dennis@amonics.com>
 */

#include <define.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

/** Set this to 0 to disable mutex, then the result should add up less to 10000 */
#define MUTEX                   1

/**
 * \brief Increment $counter 5000 times, then prints "Ty: counter = x"
 * \param ptr [0]: thread index
 * \return never return
 */
void* 
tskMutex(void* ptr)
{
  unsigned int index = *((unsigned int*)ptr);
  char done = 0;
  
  start_process();

  while(1)
    {
      if(done == 0)
        {
          int i=0;
          for(i=0; i<5000; i++)
            {
#if MUTEX
              while(pthread_mutex_lock(&myMutex) != 0);
#endif /* MUTEX */
              counter++;
              int j;
              for(j=0; j<5000; j++);
#if MUTEX
              pthread_mutex_unlock(&myMutex);
#endif /* MUTEX */
            }
          printf("T%d: counter = %d\n", index, counter);
        }
      done = 1;
    }     

  end_process();
}
