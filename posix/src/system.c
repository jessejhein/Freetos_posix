/**
 * \file
 * Implementation of system thread
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
struct crElement_t crlist[MAX_CRTHREAD];
pthread_t crthread_id_counter = (pthread_t)1;
#endif /* CRTHREAD_SCHED */


/**
 * \brief System Thread (currently coroutine scheduler)
 * \param ptr (not used)
 * \remarks main program should create this thread if coroutine scheduler is needed 
 */
void* 
sys_thread (void* ptr)
{
  while (1)
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
      for (index = 0; index < MAX_CRTHREAD; index++)
        {
          if (crlist[index].crthread > (((crthread_t) 0 ) + MAX_CRTHREAD))
            {
              if ((*(crlist[index].crthread))(crlist[index].arg) == 0)
                {
                  unsigned char j, index_new;
                  unsigned char found = 0;
                  //activate next thread (if any)
                  for (j = 0; j < MAX_CRTHREAD; j++)
                    {
                      if (crlist[j].crthread == (((crthread_t) 0 ) + index))
                        {
                          if (found == 0)
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
      usleep (0);
    }
}


