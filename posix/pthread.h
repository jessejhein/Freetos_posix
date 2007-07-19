/************************************************************************************************
 * File: 			pthread.h
 * Description:		interface between posix thread, mutex and freertos task, semaphore
 ***********************************************************************************************/

#ifndef PTHREAD_H_
#define PTHREAD_H_  1

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

/*******************************************************************************************
 * pthread_t -> xTaskHandle (void *, and tskTCB, refer to task.h, and tash.c)
 *******************************************************************************************/
#define pthread_t	xTaskHandle

/*******************************************************************************************
 * pthread_join() - wait for thread termination
 ******************************************************************************************* 
 * int pthread_join(pthread_t thread, void **value_ptr);
 *******************************************************************************************/
#define pthread_join(thread, value_ptr)     while(0)

/*******************************************************************************************
 * Name:        void pthread_create( 
 *                      pthread_t *restrict thread, 
 *                      const pthread_attr_t *restrict attr, 
 *                      void *(*start_routine)(void*), 
 *                      void *restrict arg)
 * 
 * Function:    Create a new thread, with attributes specified by attr. 
 *              Upon successful completion, pthread_create() shall store the ID of 
 *              the created thread in the location referenced by thread.
 *              The thread is created executing start_routine with arg as its sole argument.
 * 
 * Input:       thread: handler for task
 *              attr: not implemented, use NULL
 *              start_routine: function pointer for the task
 *              arg: arguments to passed to task
 * 
 * Output:      not implemented (int for POSIX compliant)
 *              If successful, the pthread_create() function shall return zero; 
 *              otherwise, an error number shall be returned to indicate the error.
 *******************************************************************************************
 * The task is created with minimal stack size and idle priority 
 *******************************************************************************************/
#define pthread_create(pthread, pattr, pstart_routine, parg) \
			xTaskCreate(pstart_routine, NULL, configMINIMAL_STACK_SIZE, parg, tskIDLE_PRIORITY, pthread)



/*******************************************************************************************
 * pthread_mutex_t -> xSemaphoreHandle (xQueueHandle, void *, xQUEUE* refer to semphr.h, queue.h, queue.c)
 *******************************************************************************************/
#define pthread_mutex_t xSemaphoreHandle

/*******************************************************************************************
 * Name:        int pthread_mutex_init(
 *                      pthread_mutex_t *mutex, 
 *                      const pthread_mutexattr_t *attr)
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
#define pthread_mutexattr_t     int	
extern int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);

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
