/**
 * \file
 * Implementation of system thread
 * \author Dennis Tsang <dennis@amonics.com>
 */

#include<define.h>
#include<unistd.h>
#include <pthread.h>

/*
 * Coroutine Thread
 */
#ifdef CRTHREAD_SCHED
struct crElement_t crlist[MAX_CRTHREAD];
pthread_t crthread_id_counter = (pthread_t)1;
#endif /* CRTHREAD_SCHED */


/**
 * \brief System Thread (currently corountine scheduler)
 * \param ptr (not used)
 * \remarks main program should create this thread if coroutine scheduler is needed 
 */
void* 
sys_thread(void* ptr)
{
  while(1)
  {
#ifdef CRTHREAD_SCHED
    /*
     * Principle of CRTHREAD Scheduler
     *   for each active thread, execute it
     *   if the active thread is completed, 
     *      make the next inactive thread active
     *      renumber the rest of inactive threads
     * 
     *   e.g. enable() is finished
     *        before execution
     *                  crlist[0].crthread = enable   -> execute
     *                  crlist[0].crthread = adj      -> execute
     *                  crlist[0].crthread = disable  -> execute
     *                  crlist[0].crthread = 0        -> not execute
     *                  crlist[0].crthread = 1        -> not execute
     *        after execution
     *                  crlist[0].crthread = 3
     *                  crlist[0].crthread = adj
     *                  crlist[0].crthread = disable
     *                  crlist[0].crthread = enable
     *                  crlist[0].crthread = 1
     */
    unsigned char index;
    for(index=0; index<MAX_CRTHREAD; index++)
      {
        if(crlist[index].crthread > (((crthread_t) 0 ) + MAX_CRTHREAD) )
          {
            if( (*(crlist[index].crthread))(crlist[index].arg) == 0 )
              {
                unsigned char j, index_new;
                unsigned char found = 0;
                //activate next thread (if any)
                for(j=0; j<MAX_CRTHREAD; j++)
                  {
                    if( crlist[j].crthread == (((crthread_t) 0 ) + index) )
                      {
                        if(found == 0)
                          {
                            found = 1;
                            crlist[j].crthread = crlist[index].crthread;
                            index_new = j;
                          }
                        else
                          {
                            crlist[j].crthread = ((crthread_t) 0 ) + index_new;
                          }
                      }
                  }
                //free the thread
                crlist[index].id = (pthread_t) 0;
                crlist[index].crthread = CRTHREAD_EMPTY;
                crlist[index].arg = NULL;
              }
          }
      }
#endif /* CRTHREAD_SCHED */
    
    //TODO: add other system operations here
    usleep(0);
  }
}


