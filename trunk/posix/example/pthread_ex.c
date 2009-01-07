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
 * 1) Starting a new FreeRTOS task
 *    Note: Do not use pthread_join() with FreeRTOS task                                                
 */
#include <pthread.h>
void* 
task(void* ptr)
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
task(void* ptr)
{
  pthread_t thread_cr;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
  static unsigned char arg = 0;
  pthread_create(&thread_cr, &attr, enable, &arg);
}

/*
 * 3)  Define a task
 */
#include<define.h>
/*This part is needed if the thread is run by the coroutine scheduler
    //---------------------------------------------------------
    // Include this section of code to map 
    //  start_process() -> scrBegin 
    //  end_process()   -> scrFinish(0)
    //  sleep()         -> scrReturn(-1)
    //  usleep()        -> scrReturn(-1)
    #ifdef FREERTOS_SCHED 
    #   undef FREERTOS_SCHED
    #   undef start_process
    #   undef end_process
    #   include <coroutine_st.h>
    #   define start_process()          scrBegin
    #   define end_process()            scrFinish((void*)0)
    #endif
    #include <unistd.h>
    //---------------------------------------------------------
*/
#include<unistd.h>
void* 
task(void* ptr)
{       
  start_process();
    {
      init();
      
      while(1)
        {
          foo();
          sleep();
        }
    }
  end_process();    
}

/*
 * 4) Using Mutex
 */
#include <pthread.h>
#include <define.h>
    
pthread_mutex_t myMutex;
unsigned int counter = 0;

void* 
tskMutex(void* ptr)
{
  unsigned int index = *((unsigned int*)ptr);
  unsigned char buf[20];
  char done = 0;
  int i=0, j=0;
        
  start_process();
  
  while(1)
    {
      if(done == 0)
        {
          for(i=0;i<5000;i++)
            {
              //wait until myMutex is released
              while(pthread_mutex_lock(&myMutex) != 0);
              //increment counter
              counter++;
              //delay
              for(j=0; j<index*5000; j++);
              //release after use
              pthread_mutex_unlock(&myMutex);
            }
          done = 1;
        }
    }
  end_process();
}
    
void 
vSetupHardware( void )
{
  //setup myMutex
  pthread_mutex_init(&myMutex, NULL);
}
    
void 
vUserMain()
{
  //Identify your threads here
  pthread_t thread_mutex1, thread_mutex2;
        
  static unsigned int arg_mutex1 = 1;  //Index, must be declared static or global
  static unsigned int arg_mutex2 = 2;  //Index, must be declared static or global
    
  //Create your threads here
  tsk1_created = pthread_create(&thread_mutex1, NULL, tskMutex, &arg_mutex1);
  tsk2_created = pthread_create(&thread_mutex2, NULL, tskMutex, &arg_mutex2);
}

/** @} */
/** @} */