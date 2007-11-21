/************************************************************************************************
 * File: 			pthread.h
 * Description:		interface between posix thread, mutex and freertos task, semaphore
 ***********************************************************************************************/

/*******************************************************************************************
 * This header file contains the mixed operation for FreeRTOS Task and CRTHREAD, the selection
 * of which operation is determined by #define directives
 *******************************************************************************************
 * Principle of FreeRTOS Scheduler
 * ===============================
 * 
 * When a task is created, the required dynamic memory (minimal stack size) will be acquired
 * from heap.
 * 
 * The created task will be executed in a round-robin fashion, i.e Task A -> Task B -> Idle Task
 * -> Task A -> Task B -> ... 
 * 
 * During the context switch between differnt tasks (except Idle Task), all the contents in 
 * the register stack will be saved, and the task will resume correctly when the saved data
 * are loaded back to the stack.
 * 
 *******************************************************************************************
 * Principle of CRTHREAD Scheduler
 * ===============================
 * 
 * When the application task creates a CRTHREAD, the coroutine function pointer is added to 
 * the coroutine queue if the queue is not full, otherwise, the CRTHEAD is skipped.
 * 
 * The created CTHREADs will be executed during the execution of the Idle task.
 * 
 * If another instance of the same corountine function is running (active), the added newly 
 * created CRTHREAD will become inactive until the all the previous instances have been completed.  
 * 
 * NOTE:
 * A problem may occur when many CRTHREADs are created so that the the queue is full at most time.
 * In such case, most of the CRTHREADs will be skipped. Whenever one of the CRTHREAD is completed
 * and a space is available in the queue, only one new CRTHREAD (most probably, the one after usleep())
 * can be added. 
 * 
 * To prevent this to occur, the application task should check the return value of
 * pthread_create(). If it is -1, it indicate the queue is full and the application should handle 
 * the issue. Alternatively, you should increase the size of queue in define.h 
 * 
 ******************************************************************************************/ 

#ifndef PTHREAD_H_
#define PTHREAD_H_  1

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>



/*************************************************************************
 * FreeRTOS Task
 *************************************************************************/

/************************************************************************
 * ==Interfaces==
 ************************************************************************/

/************************************************************************
 * ===define.h===
 * 
 * Description:     a configuration file which contains the compile time configuration
 * 
 * Location:        which place in system directory
 * 
 * Content:         see below "Compile Time Configurations"
 *  
 ************************************************************************/

/************************************************************************
 * ==Compile Time Configurations==
 ***********************************************************************/

/************************************************************************
 * ===inside define.h===                                                
 * 
 * ====Enable FreeRTOS Scheduler====
 * 1) A Task corresponds to an infinite loop
 */
#if 0

    /* ====Enable FreeRTOS Scheduler====
     */
    #ifdef FREERTOS_SCHED 
    #   define start_process()          while(1){
    #   define end_process()            }
    #else
    #   include <coroutine_st.h>
    #   define start_process()          scrBegin
    #   define end_process()            scrFinish((void*)0)
    #endif

#endif
 /***********************************************************************/

/************************************************************************
 * ==APIs==
 ************************************************************************/

/************************************************************************
 * ===API Usage Examples===
 * 
 * 1) Starting a new task
 *    Note: Do not use pthread_join() with FreeRTOS task                                                
 */
#if 0

    #include <pthread.h>
    void* task(void* ptr){
        static unsigned char arg = 0;
        pthread_t thread_id;        
        pthread_create(&thread_id, NULL, foo, &arg);
    }

#endif
/*
 * 2)  Creating a task
 */
#if 0

    #include<define.h>
    #include<unistd.h>
    void* task(void* ptr){       
        start_process();
        {
            init();
            
            while(1){
                foo();
                sleep();
            }
        }
        end_process();    
    }
    
#endif
/*
 * 3) Using Mutex
 */
#if 0

    #include <pthread.h>
    #include <define.h>
    
    pthread_mutex_t myMutex;
    unsigned int counter = 0;

    void* tskMutex(void* ptr)
    {
        unsigned int index = *((unsigned int*)ptr);
        unsigned char buf[20];
        char done = 0;
        int i=0, j=0;
        
        start_process();
            if(done == 0){
                for(i=0;i<5000;i++){
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
        end_process();
    }
    
    void vSetupHardware( void ){
        //setup myMutex
        pthread_mutex_init(&myMutex, NULL);
    }
    
    void vUserMain(){
        //Identify your threads here
        pthread_t thread_mutex1, thread_mutex2;
        
        static unsigned int arg_mutex1 = 1;  //Index, must be declared static or global
        static unsigned int arg_mutex2 = 2;  //Index, must be declared static or global
    
        //Create your threads here
        tsk1_created = pthread_create(&thread_mutex1, NULL, tskMutex, &arg_mutex1);
        tsk2_created = pthread_create(&thread_mutex2, NULL, tskMutex, &arg_mutex2);
        
        //Main program thread should waits here while user threads are running  
        pthread_join(thread_mutex1, NULL);
        pthread_join(thread_mutex2, NULL);
    }
    
#endif 
 /***********************************************************************/
 
//-------------------------------------------------------------------------





/*******************************************************************************************
 * Name:        int pthread_attr_init(pthread_attr_t *attr)
 * 
 * Function:    The pthread_attr_init() function shall initialize a thread attributes object 
 *              attr with the default value for all of the individual attributes used by a 
 *              given implementation.
 * 
 * Input:       pointer to attr variable                                                   */
                 //pthread_attr_t -> unsigned char (expandable if more attributes are needed)
               #define pthread_attr_t      unsigned char  
/* 
 * Output:      Upon successful completion, pthread_attr_init() shall return a value of 0; 
 *              otherwise, an error number shall be returned to indicate the error. (not implemented)
 *  
 *******************************************************************************************/
#define pthread_attr_init(attr_ptr)        while(0)

/*******************************************************************************************
 * Name:        int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope)
 * 
 * Function:    pthread_attr_setscope() functions, respectively, shall set the contentionscope 
 *              attribute in the attr object.
 * 
 * Input:       pointer to attr variable
 *              attr_id (e.g. PTHREAD_SCOPE_SYSTEM, PTHREAD_SCOPE_PROCESS)                 */
                enum
                {
                  PTHREAD_SCOPE_SYSTEM,
                #define PTHREAD_SCOPE_SYSTEM    PTHREAD_SCOPE_SYSTEM
                  PTHREAD_SCOPE_PROCESS
                #define PTHREAD_SCOPE_PROCESS   PTHREAD_SCOPE_PROCESS
                };
/*
 *  Output:      If successful, the pthread_attr_setscope() functions shall return zero; 
 *              otherwise, an error number shall be returned to indicate the error. (not implemented)
 *  
 *******************************************************************************************/
#define pthread_attr_setscope(attr_ptr, attr_id)    *attr_ptr = attr_id

/*******************************************************************************************
 * Name:        int pthread_create( 
 *                      pthread_t *restrict thread, 
 *                      const pthread_attr_t *restrict attr, 
 *                      void* (*start_routine)(void*), 
 *                      void *restrict arg)
 * 
 * Function:    Create a new thread, with attributes specified by attr. 
 *              Upon successful completion, pthread_create() shall store the ID of 
 *              the created thread in the location referenced by thread.
 *              The thread is created executing start_routine with arg as its sole argument.
 * 
 * Input:       thread: handler for task
 *                      if caller function is a coroutine, content of thread should be declared
 *                      static or global                                                    */
                //pthread_t -> xTaskHandle (void *, and tskTCB, refer to task.h, and task.c)
               #define pthread_t           xTaskHandle
/*              attr: attribute pointer related to the thread (dynamic selection of scheduling policy)
 *                    NULL use FreeRTOS Task
 *                    PTHREAD_SCOPE_SYSTEM use crthread
 *              start_routine: function pointer for the task
 *              arg: arguments to passed to task
 *                   for crthread, content of arg should be declared static or global
 * 
 * Output:      If successful, the pthread_create() function shall return zero; 
 *              otherwise, an error number (-1) shall be returned to indicate the error.
 *******************************************************************************************
 * The task is created with minimal stack size and idle priority 
 *******************************************************************************************/
extern int pthread_create(pthread_t* thread, pthread_attr_t* attr, void* (*start_routine)(void*), void* arg);

/*******************************************************************************************
 * Function:    int pthread_join(pthread_t thread, void **value_ptr);
 * 
 * Description: wait for thread termination 
 * 
 * Input:       thread:     handler for target thread
 *              *value_ptr: resources pointed by the pointer will be released (not implemented yet)
 * 
 * Output:      (not implemented)
 *              Upon sucess, return 0
 *              Upon failure, -1, errno raised
 *                  ESRCH:   no such thread
 *                  EDEADLK: detected deadlock
 *                  EINVAL:  target thread is not joinable
 * 
 * Limitations: do not work for (Caller, Target) = (FreeRTOS Task, FreeRTOS Task)
 *              work for (Caller, Target) = (FreeRTOS Task, coroutine_st)
 *              work for (Caller, Target) = (coroutine_st, coroutine_st)
 *******************************************************************************************
 * Rationale for pthread_join:
 * ---------------------------
 * 
 * CRTHREAD QUEUE
 * ==============
 *  crlist[0] = {88,  disable,        arg0}
 *  crlist[1] = {0,   CRTHREAD_EMPTY, arg1}
 *  crlist[2] = {124, 0,              arg3}
 *  crlist[3] = {125, 0,              arg4}
 *      ...
 *  crlist[MAX_CRTHREAD-1] = {0, CRTHREAD_EMPTY, argn}
 * 
 * Implementation at pthread_create()
 * ==================================
 *  Thread ID = an integer greater than 0;
 * 
 * Implementation at pthread_join()
 * ==================================
 *  Finish condition occurs when the thread id does not exist in the list as a result of
 *  removal by coroutine scheduler  
 *******************************************************************************************
 * Problem arised if using address of crthread as id:
 * --------------------------------------------------
 *
 *               disable finishes              
 *            within 1 round and set 
 *       crthread[5] = CRTHREAD_EMPTY        Task B will
 *              Task B should wake        continue to sleep
 *                              |               |
 *                          [2] |           [4] |
 *                             \|/             \|/
 *        +--------+--------+------+--------+--------+------+
 *        | Task A | Task B | Idle | Task A | Task B | Idle |
 *        +--------+--------+------+--------+--------+------+
 *                     |      /|\       |               /|\
 *                 [1] |       |    [3] |                |
 *                     +-------+        +----------------+
 *  Task B call pthread_create()     Task A call pthread_create()
 *  crthread[5] = disable            crthread[5] = xxx
 *  thread_idB = &crthread[5]        thread_idA = &crthread[5]
 *  Task B will sleep
 * 
 * EVENTS:
 * [1] Task B calls pthread_create(), one of the crthread, say crthread[5], is assigned to
 *     the target function (e.g. crthread[5] = disable, hence thread_idB = &crthread[5]).
 *     Task B will sleep immediately, and expect to wake until disable() is completed.
 * [2] Idle task will execute the CR_SCHEDULER. If disable() is completed upon exiting the 
 *     scheduler (i.e. no sleep is called), then crthread[5] = CRTHREAD_EMPTY. Indicating
 *     Task B should wake up.
 * [3] If Task A now create a thread and takes up the same position, (i.e. crthread[5] = xxx)
 *     The CRTHREAD_EMPTY status will be erased.
 * [4] When Task B checks for the content in thread_idB, it is not equal to CRTHREAD_EMPTY,
 *     and it continue to sleep.
 *******************************************************************************************/
#ifdef CRTHREAD_SCHED
#define pthread_join(thread, value_ptr)     while(1){\
                                                if(thread==(pthread_t)0) break;\
                                                char pthread_join_i;\
                                                char pthread_join_index = -1;\
                                                for(pthread_join_i=0; pthread_join_i<MAX_CRTHREAD; pthread_join_i++){\
                                                    if(thread == crlist[pthread_join_i].id){\
                                                        pthread_join_index = pthread_join_i;\
                                                    }\
                                                }\
                                                if(pthread_join_index < 0) break;\
                                                else usleep(0);\
                                             }
#else
#define pthread_join(thread, value_ptr)     while(0)
#endif

/*******************************************************************************************
 * Name:        int pthread_mutex_init(
 *                      pthread_mutex_t *mutex, 
 *                      pthread_mutexattr_t *attr)
 * 
 * Function:    The pthread_mutex_init() function shall initialize the mutex referenced 
 *              by mutex with attributes specified by attr. If attr is NULL, the default 
 *              mutex attributes are used; the effect shall be the same as passing the 
 *              address of a default mutex attributes object. Upon successful initialization, 
 *              the state of the mutex becomes initialized and unlocked.
 * 
 * Input:       mutex: Handle to the created semaphore.                                    */
                       //pthread_mutex_t -> xSemaphoreHandle (xQueueHandle, void *, xQUEUE* refer to semphr.h, queue.h, queue.c)
                    #define pthread_mutex_t     xSemaphoreHandle
/*              attr: not implemented, use NULL                                            */
                    #define pthread_mutexattr_t int
/* 
 * Output:      If successful, pthread_mutex_init() functions shall return zero; 
 *              otherwise, an error number shall be returned to indicate the error.
 *******************************************************************************************/
extern int pthread_mutex_init(pthread_mutex_t *mutex, pthread_mutexattr_t *attr);

/*******************************************************************************************
 * Name:        int pthread_mutex_lock(pthread_mutex_t *mutex)
 * 
 * Function:    The mutex object referenced by mutex shall be locked by calling 
 *              pthread_mutex_lock(). If the mutex is already locked, the calling thread 
 *              shall block until the mutex becomes available. This operation shall return 
 *              with the mutex object referenced by mutex in the locked state with the calling
 *              thread as its owner.
 * 
 * Input:       mutex: Handle to the created semaphore.
 * 
 * Output:      If successful, the pthread_mutex_lock() functions shall return zero; 
 *              otherwise, an error number shall be returned to indicate the error.         
 *******************************************************************************************/
extern int pthread_mutex_lock(pthread_mutex_t *mutex);

/*******************************************************************************************
 * Name:        int pthread_mutex_unlock(pthread_mutex_t *mutex)
 * 
 * Function:    The pthread_mutex_unlock() function shall release the mutex object referenced 
 *              by mutex. The manner in which a mutex is released is dependent upon the mutex's 
 *              type attribute. If there are threads blocked on the mutex object referenced by
 *              mutex when pthread_mutex_unlock() is called, resulting in the mutex becoming 
 *              available, the scheduling policy shall determine which thread shall acquire 
 *              the mutex.
 * 
 * Input:       mutex: Handle to the created semaphore.
 * 
 * Output:      If successful, pthread_mutex_unlock() functions shall return zero; 
 *              otherwise, an error number shall be returned to indicate the error.
 * 
 *  
 * pthread_mutex_unlock() -> xSemaphoreGive()
 *******************************************************************************************
 * This must not be used from an ISR. See xSemaphoreGiveFromISR() for an alternative which 
 * can be used from an ISR.
 ********************************************************************************************/
extern int pthread_mutex_unlock(pthread_mutex_t *mutex);

//----------------------------------------------------------------------------





/*************************************************************************
 * CRTHREAD
 *************************************************************************/

/************************************************************************
 * ==Interfaces==
 ************************************************************************/

/************************************************************************
 * ===define.h===
 * 
 * Description:     a configuration file which contains the compile time configuration
 * 
 * Location:        which place in system directory
 * 
 * Content:         see below "Compile Time Configurations"
 *  
 ************************************************************************/

#ifdef CRTHREAD_SCHED
/*******************************************************************************************
 * ===extern crElement_t crlist[]===
 * Description: Array of crElements. Each element contains a function pointer to a crthread
 *              and a crthread_id
 * 
 * Location:    $(BASE_ROOT)/freertos_posix/demo_posix/common/app_idlehook.c
 * 
 * crElement_t: id:         crthread ID
 *                          0  = no task
 *                          >0 = valid id 
 *              crthread:   function pointer type for coroutine  
 *                          coroutine thread should take the form: void* foo(void* arg)
 *                          crthread_t crthread = (crthread_t) &foo;                       */
                            typedef void* (*crthread_t)(void* arg);
/*                          CRTHREAD_EMPTY:  indicate that no crthread has been scheduled  */
                         #define CRTHREAD_EMPTY  (((crthread_t) 0 ) + MAX_CRTHREAD)
/*              arg:        pointer to arguments assocated with the crthread
 *******************************************************************************************/
struct crElement_t{
    pthread_t id;
    crthread_t crthread;
    void* arg;
};
extern struct crElement_t crlist[];

/*******************************************************************************************
 * Function:    pthread_t next_crthread_id(void)
 * 
 * Description: return the next valid crthread id
 * 
 * Input:       none
 * 
 * Output:      a unique and valid crthread id
 *              > 0
 *******************************************************************************************/
extern pthread_t crthread_id_counter;
#define next_crthread_id()      (crthread_id_counter++); \
                                 if(crthread_id_counter == (pthread_t)0) crthread_id_counter++
#endif

/************************************************************************
 * ==Compile Time Configurations==
 ***********************************************************************/

/************************************************************************
 * ===inside define.h===                                                
 * 
 * ====Enable FreeRTOS Scheduler====
 * 1) CRTHREAD Scheduler is designed to use coroutine_st within the idle task
 * 2) Enable the CRTHREAD scheduler and set the maximum queue size for CRTHREAD
 */
#if 0

    /* ====Enable FreeRTOS Scheduler====
     */
    #ifdef FREERTOS_SCHED 
    #   define start_process()          while(1){
    #   define end_process()            }
        //==========Enable Coroutine Thread Scheduler in FREERTOS Scheduler========
        #ifdef CRTHREAD_SCHED
        #define MAX_CRTHREAD            10
        #endif
    #else
    #   include <coroutine_st.h>
    #   define start_process()          scrBegin
    #   define end_process()            scrFinish((void*)0)
    #endif

#endif
 /***********************************************************************/

/************************************************************************
 * ==APIs==
 ************************************************************************/

/************************************************************************
 * ===API Usage Examples===                                                
 * 
 * ====Enable FreeRTOS Scheduler====
 * 1) The following code should be placed at the beginning of the source code for
 *    coroutine threads:
 */
#if 0

    #include <define.h>
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
    
    void* foo(void* ptr){
        static int index = 0;
        start_process();
        init();
        
        for(index=0; index<MAX; index++){
            foo2();
            usleep(0);
        }
        end_process();
    }

#endif
/*
 * 2) Application Task can create CRTHREAD using standard posix pthread_create(), e.g.
 */
#if 0
    
    #include<pthread.h>
    void* task(void* ptr){
        static unsigned char arg = 0;
        static pthread_t thread_cr;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
                    
        pthread_create(&thread_cr, &attr, foo, &arg);
        //Optional to wait until foo(&arg) is finished
        pthread_join(thread_cr, NULL);
    }

#endif
 /***********************************************************************/
#endif /* PTHREAD_H_  */
