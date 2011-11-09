/**
 * \addtogroup posix POSIX
 * @{
 * 
 * Implementation of POSIX API using FreeRTOS API
 */

/**
 * \defgroup pthread POSIX Thread Call
 * @{
 * 
 * Implementation of POSIX Threads, interface between posix thread, mutex and freertos task, semaphore
 */

/**
 * \file
 * POSIX Thread
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

/*
 * 1) Starting a new FreeRTOS task
 *    Note: Do not use pthread_join() with FreeRTOS task                                                
 */
#include <pthread.h>
void* 
task (void* ptr)
{
  static unsigned char arg = 0;
  pthread_t thread_id;        
  pthread_create(&thread_id, NULL, foo, &arg);
}

/*
 * 2) Starting a new task using coroutine scheduler
 */
#include <pthread.h>
void* 
task (void* ptr)
{
  pthread_t thread_cr;
  pthread_attr_t attr;
  pthread_attr_init (&attr);
  pthread_attr_setscope (&attr, PTHREAD_SCOPE_SYSTEM);
  static unsigned char arg = 0;
  pthread_create (&thread_cr, &attr, ld_enable, &arg);
}

/*
 * 3)  Define a task
 */
#include <define.h>
/**
 * \brief the following section uses coroutine instead of task
 */
#define USE_COROUTINE                   1
#include <unistd.h>
/******************************************************************/
void* 
task (void* ptr)
{       
  start_process ();
    {
      init ();
      
      while (1)
        {
          foo ();
          sleep ();
        }
    }
  end_process ();
}

/*
 * 4) Using Mutex
 */
#include <pthread.h>
#include <define.h>
    
pthread_mutex_t myMutex;
unsigned int counter = 0;

void* 
tskMutex (void* ptr)
{
  unsigned int index = *((unsigned int*)ptr);
  unsigned char buf[20];
  char done = 0;
  int i = 0, j = 0;
        
  start_process ();
  
  while (1)
    {
      if (done == 0)
        {
          for (i = 0; i < 5000; i++)
            {
              //wait until myMutex is released
              while (pthread_mutex_lock (&myMutex) != 0);
              //increment counter
              counter++;
              //delay
              for (j = 0; j < index*5000; j++);
              //release after use
              pthread_mutex_unlock (&myMutex);
            }
          done = 1;
        }
    }
  end_process();
}
    
void 
vSetupHardware (void)
{
  //setup myMutex
  pthread_mutex_init (&myMutex, NULL);
}
    
void 
vUserMain (void)
{
  //Identify your threads here
  pthread_t thread_mutex1, thread_mutex2;
        
  static unsigned int arg_mutex1 = 1;  //Index, must be declared static or global
  static unsigned int arg_mutex2 = 2;  //Index, must be declared static or global
    
  //Create your threads here
  tsk1_created = pthread_create (&thread_mutex1, NULL, tskMutex, &arg_mutex1);
  tsk2_created = pthread_create (&thread_mutex2, NULL, tskMutex, &arg_mutex2);
}

/** @} */
/** @} */
