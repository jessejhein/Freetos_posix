/**
 * \file
 * Interface between POSIX thread, MUTEX and FreeRTOS task, semaphore
 * \author Dennis Tsang <dennis@amonics.com>
 */

#include <pthread.h>

int 
pthread_create (pthread_t* thread, pthread_attr_t* attr, void* (*start_routine)(void*), void* arg)
{
#ifdef CRTHREAD_SCHED
  /*
   * Principle of CRTHREAD Scheduler
   *    search if there is a previous call to same thread
   *      if found, insert the index number (inactive) to the the first CRTHREAD_EMPTY position
   *      if not found, insert the function pointer (active) to the first CRTHREAD_EMPTY position
   *    skip the thread when list is full
   *
   * e.g. adding enable to here:
   *                  crlist[0].crthread = adj
   *                  crlist[1].crthread = CRTHREAD_EMPTY          <-- (void*)3
   *                  crlist[2].crthread = CRTHREAD_EMPTY
   *                  crlist[3].crthread = enable
   *                  crlist[4].crthread = CRTHREAD_EMPTY
   */
  if ((attr != NULL) && (*attr == PTHREAD_SCOPE_SYSTEM))
    {
      unsigned char i;
      unsigned char indexCr, indexEmpty;
      unsigned char foundCr = 0;
      unsigned char foundEmpty = 0;        
      for (i = 0; i < MAX_CRTHREAD; i++)
        {
          //Find CRTHREAD_EMPTY
          if (foundEmpty == 0)
            {
              if (crlist[i].crthread == CRTHREAD_EMPTY)
                {
                  foundEmpty = 1;
                  indexEmpty = i;
                }
            }
          //Find start_rountine
          if (foundCr == 0)
            {
              if (crlist[i].crthread == (crthread_t) start_routine)
                {
                  foundCr = 1;
                  indexCr = i;
                }
            }
          if ((foundEmpty == 1) && (foundCr == 1))
            {
              break;
            }
        }
      if (foundEmpty == 1)
        {
          //save id
          crlist[indexEmpty].id = next_crthread_id();
          *thread = crlist[indexEmpty].id;
          //save function
          if (foundCr == 0)
            {
              crlist[indexEmpty].crthread = (crthread_t) start_routine;
            }
          else
            {
              crlist[indexEmpty].crthread = (((crthread_t) 0) + indexCr);
            }
          //save arg
          crlist[indexEmpty].arg = arg;
        }
      else
        {
          return -1;
        }
    }
  else
#endif /* CRTHREAD_SCHED */
    {
      xTaskCreate ((pdTASK_CODE) start_routine, NULL, configMINIMAL_STACK_SIZE, arg, tskIDLE_PRIORITY, thread);
    }
  return 0;
}


int 
pthread_mutex_init (pthread_mutex_t *mutex, pthread_mutexattr_t *attr)
{
  vSemaphoreCreateBinary (*mutex);
  return (*mutex != NULL)? 0 : 1;
}


int 
pthread_mutex_lock (pthread_mutex_t *mutex)
{
  return (pdTRUE == xSemaphoreTake (*mutex, (portTickType)0)) ? 0 : 1;
}


int 
pthread_mutex_unlock (pthread_mutex_t *mutex)
{
  return (pdTRUE == xSemaphoreGive(*mutex)) ? 0 : 1;	
}
