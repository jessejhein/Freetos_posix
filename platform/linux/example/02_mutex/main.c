/**
 * \file
 * This program creates 2 threads (tasks).
 * 1) Thread 1 increment $counter 5000 times, then prints "T1: counter = x"
 * 2) Thread 2 increment $counter 5000 times, then prints "T2: counter = x" 
 * EXPECTED RESULTS:
 * If MUTEX_ENABLE is set to 1 in mutex.c, then one of the thread should equals 10000; 
 * otherwise, both threads are likely to yield values less then 10000.
 * \author Dennis Tsang <dennis@amonics.com>
 */

#include <define.h>
#include <pthread.h>
#include <mutex.h>


/** mutex for threads */
pthread_mutex_t myMutex;
/** global counter, shared by threads */
unsigned int counter = 0;


/**
 * \brief set up hardware
 */
void 
vSetupHardware (void)
{
  pthread_mutex_init (&myMutex, NULL);
}


/**
 * \brief user main
 */
void 
vUserMain (void)
{
  //Identify your threads here
  pthread_t thread_mutex1, thread_mutex2;
  
  static unsigned int arg_mutex1 = 0;
  static unsigned int arg_mutex2 = 1;

  //Create your threads here
  pthread_create (&thread_mutex1, NULL, tskMutex, &arg_mutex1);
  pthread_create (&thread_mutex2, NULL, tskMutex, &arg_mutex2);
}
