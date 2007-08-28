/************************************************************************************************
 * File: 			pthread.h
 * Description:		interface between posix thread, mutex and freertos task, semaphore
 ***********************************************************************************************/

#ifndef PTHREAD_H_
#define PTHREAD_H_  1

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <sched.h>

#if(CRTHREAD_ENABLE > 0)
/*******************************************************************************************
 * Using CRTHREAD Scheduler
 * ========================
 * 1) CRTHREAD Scheduler is designed to use coroutine_st within the idle task
 * 2) In define.h, enable the CRTHREAD scheduler and set the maximum queue size for CRTHREAD
 *          #define CRTHREAD_ENABLE             1
 *          #if(CRTHREAD_ENABLE == 1)
 *          #define MAX_CRTHREAD                10
 *          #endif 
 * 3) Application Task can create CRTHREAD using standard posix pthread_create(), e.g.
 *          pthread_create(&thread, &attr, crfunction, &crarg);
 *    provided 
 *          attr = SCHED_COROUTINE;
 *    and
 *          crarg is declared static
 * 4) The following code should be placed at the beginning of the source code for
 *    coroutine threads:
 *          #include <define.h>
 *          // Include this section of code to map 
 *          //  start_process() -> scrBegin 
 *          //  end_process()   -> scrFinish(0)
 *          //  sleep()         -> scrReturn(-1)
 *          //  usleep()        -> scrReturn(-1)
 *          #ifdef FREERTOS_SCHE_ENABLE
 *          #   undef FREERTOS_SCHE_ENABLE
 *          #   undef start_process
 *          #   undef end_process
 *          #   define FREERTOS_SCHE_ENABLE     0
 *          #   include <coroutine_st.h>
 *          #   define start_process()          scrBegin
 *          #   define end_process()            scrFinish((void*)0)
 *          #endif
 *          #include <unistd.h>
 * 
 * 
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
 * A problem may occur when many CRTHREDs are created so that the the queue is full at most time.
 * In such case, most of the CRTHREADs will be skipped. Whenever one of the CRTHREAD is completed
 * and a space is available in the queue, only one new CRTHREAD (most probably, the one after usleep())
 * can be added. 
 * 
 * To prevent this to occur, the application task should check the return value of
 * pthread_create(). If it is -1, it indicate the queue is full and the application should handle 
 * the issue. Alternatively, you should increase the size of queue in define.h 
 * 
 ******************************************************************************************/ 

/*******************************************************************************************
 * CRTHREAD DEFINES
 * crthread_t:  function pointer type for coroutine  
 *              coroutine thread should take the form: void* foo(void* arg)
 *              crthread_t crthread = (crthread_t) &foo;
 * crthread[i] = CRTHREAD_EMPTY indicate that no crthread has been scheduled
 *******************************************************************************************/
typedef void* (*crthread_t)(void* arg);
#define CRTHREAD_EMPTY  (((crthread_t) 0 ) + MAX_CRTHREAD)

//External Variables
//crthread:        Array of function pointers to store crthreads
//crthread_arg:    Store the pointers for crthread arguments, contents should be static variables
extern crthread_t crthread[];
extern void* crthread_arg[];
#endif




/*******************************************************************************************
 * POSIX THREAD DEFINES
 * pthread_t -> xTaskHandle (void *, and tskTCB, refer to task.h, and tash.c)
 * pthread_attr_t -> unsigned char (expandable if more attributes are needed) 
 * pthread_mutex_t -> xSemaphoreHandle (xQueueHandle, void *, xQUEUE* refer to semphr.h, queue.h, queue.c)
 *******************************************************************************************/
#define pthread_t           xTaskHandle
#define pthread_attr_t      unsigned char
#define pthread_mutex_t     xSemaphoreHandle
#define pthread_mutexattr_t int

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
 *              attr: attribute pointer related to the thread (dynamic selection of scheduling policy,
 *                    do not confuse with FREERTOS_SCHE_ENABLE in define.h)
 *                    NULL/SCHED_FREERTOS use FreeRTOS Task
 *                    SCHED_COROUTINE use coroutine 
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
 * pthread_join() - wait for thread termination
 ******************************************************************************************* 
 * int pthread_join(pthread_t thread, void **value_ptr);
 *******************************************************************************************/
#define pthread_join(thread, value_ptr)     while(0)


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
 * Input:       mutex: Handle to the created semaphore.
 *              attr: not implemented, use NULL
 * 
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

#endif /* PTHREAD_H_  */
