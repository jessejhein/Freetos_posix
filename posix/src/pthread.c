/**
 * \file
 * Interface between POSIX thread, MUTEX and FreeRTOS task, semaphore
 * \author Dennis Tsang <dennis@amonics.com>
 */

/*
 * Copyright (C) 2007-2011  Dennis Tsang <dennis@amonics.com>
 *
 * This file is part of freertos_posix.
 *
 * freertos_posix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * freertos_posix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with freertos_posix.  If not, see <http://www.gnu.org/licenses/>
 */

#include <define.h>
#include <unistd.h>
#include <pthread.h>


/*
 * Coroutine Thread
 */
#ifdef CRTHREAD_SCHED
/**
 * \remarks must be initialised by pthread_coroutine_init () before used
 */
struct crElement_t crlist[MAX_CRTHREAD];


//---------------------------------------------------------------------------------------------------------------------
/**
 * \brief a running index from 1 to size (void*)
 * \remarks 0 is reserved to indicate EMPTY
 */
static pthread_t crthread_id_counter = (pthread_t)1;


/**
 * \brief return the next valid crthread ID
 * \return a unique and valid crthread ID > 0
 */
#define next_crthread_id()                              (crthread_id_counter++); \
                                                        if (crthread_id_counter == (pthread_t)0) crthread_id_counter++



//---------------------------------------------------------------------------------------------------------------------
/**
 * \brief this constant indicate that no crthread has been scheduled
 * \remarks set to the maximum size of the coroutine ID
 */
#define CRTHREAD_EMPTY                                  (((crthread_t) 0) + MAX_CRTHREAD)


/**
 * \remarks set function to CRTHREAD_EMPTY (MAX_CRTHREAD)
 * \verbatim
                             ID, FUNCTION,       ARG
   crlist[0]              = {0,  CRTHREAD_EMPTY, 0}
   crlist[1]              = {0,  CRTHREAD_EMPTY, 0}
   crlist[2]              = {0,  CRTHREAD_EMPTY, 0}
   crlist[3]              = {0,  CRTHREAD_EMPTY, 0}
   ...
   crlist[MAX_CRTHREAD-1] = {0,  CRTHREAD_EMPTY, 0}
   \endverbatim
 */
static void
pthread_coroutine_init (void)
{
  __u8 index;
  for (index = 0; index < MAX_CRTHREAD; index++)
    {
      crlist[index].id = 0;
      crlist[index].crthread = CRTHREAD_EMPTY;
    }
}


/**
 * \remarks
 * Principle of Operation
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
 *                  crlist[0].crthread = 0        -> not execute (enable waiting to be executed)
 *                  crlist[0].crthread = 1        -> not execute (adj waiting to be executed)
 *        after execution
 *                  crlist[0].crthread = CRTHREAD_EMPTY
 *                  crlist[0].crthread = adj
 *                  crlist[0].crthread = disable
 *                  crlist[0].crthread = enable
 *                  crlist[0].crthread = 1
 */
void*
pthread_coroutine (void* ptr)
{
  pthread_coroutine_init ();

  while (1)
    {
      __u8 index;
      for (index = 0; index < MAX_CRTHREAD; index++)
        {
          //the first MAX_CRTHREAD are reserved (i.e. not a real function)
          if (crlist[index].crthread > (((crthread_t) 0) + MAX_CRTHREAD))
            {
              //crthread is active, execute
              //if return 0, crthread is completed
              if ((*(crlist[index].crthread))(crlist[index].arg) == 0)
                {
                  int j, index_new;
                  int found = 0;
                  //activate next thread (if any)
                  for (j = 0; j < MAX_CRTHREAD; j++)
                    {
                      //search the crthread in list referencing the current crthread
                      if (crlist[j].crthread == (((crthread_t) 0) + index))
                        {
                          //first found in list - replace crthread with the function pointer
                          if (found == 0)
                            {
                              found = 1;
                              crlist[j].crthread = crlist[index].crthread;
                              index_new = j;
                            }
                          //update the remaining references in the list
                          else
                            {
                              crlist[j].crthread = ((crthread_t) 0) + index_new;
                            }
                        }
                    }
                  //free the current crthread
                  crlist[index].id = (pthread_t) 0;
                  crlist[index].crthread = CRTHREAD_EMPTY;
                  crlist[index].arg = NULL;
                }
            }
        }
      usleep (0);
    }
}
#endif /* CRTHREAD_SCHED */


/**
 * \remarks
 * Principle of CRTHREAD Scheduler
 *    search if there is a previous call to the same thread
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
int 
pthread_create (pthread_t* thread, pthread_attr_t* attr, void* (*start_routine)(void*), void* arg)
{
#ifdef CRTHREAD_SCHED
  if ((attr != NULL) && (*attr == PTHREAD_SCOPE_SYSTEM))
    {
      //found:
      //bit 0 : empty found
      //bit 1 : coroutine found
      __u8 found = 0;
      __u8 i, indexCr, indexEmpty;
      for (i = 0; i < MAX_CRTHREAD; i++)
        {
          //Find CRTHREAD_EMPTY
          if ((found & 0x01) == 0)
            {
              if (crlist[i].crthread == CRTHREAD_EMPTY)
                {
                  found |= 0x01;
                  indexEmpty = i;
                }
            }
          //Find start_rountine
          if ((found & 0x02) == 0)
            {
              if (crlist[i].crthread == (crthread_t) start_routine)
                {
                  found |= 0x02;
                  indexCr = i;
                }
            }
          //when CRTHREAD_EMPTY & start_rountine are found
          if (found == 0x03) break;
        }

      //found CRTHREAD_EMPTY
      if (found & 0x01)
        {
          //save ID: for use with join()
          crlist[indexEmpty].id = next_crthread_id ();
          *thread = crlist[indexEmpty].id;
          //save function
          if (found & 0x02) crlist[indexEmpty].crthread = (((crthread_t) 0) + indexCr);
          else crlist[indexEmpty].crthread = (crthread_t) start_routine;
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


//-------------------------------------------------------------------------------------------------------
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
