/************************************************************************************************
 * File: 			pthread.c
 * Description:		interface between posix thread, mutex and freertos task, semaphore
 ***********************************************************************************************/

#include <pthread.h>

/*******************************************************************************************
 * void vSemaphoreCreateBinary( 
 * 								xSemaphoreHandle xSemaphore 
 * 							);
 *******************************************************************************************
 * FUNCTION:
 * Macro that implements a semaphore by using the existing queue mechanism. The queue length 
 * is 1 as this is a binary semaphore. The data size is 0 as we don't want to actually store 
 * any data - we just want to know if the queue is empty or full.
 * xSemaphore 	Handle to the created semaphore. Should be of type xSemaphoreHandle.
 *******************************************************************************************/
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr){
	vSemaphoreCreateBinary(*mutex);
	return (*mutex != NULL)? 0 : 1;
}

/*******************************************************************************************
 * int xSemaphoreTake(
 * 						xSemaphoreHandle xSemaphore,
 * 						portTickType xBlockTime
 * 	  				  );
 *******************************************************************************************
 * FUNCTION:
 * Macro to obtain a semaphore. The semaphore must of been created using vSemaphoreCreateBinary().
 * xSemaphore	A handle to the semaphore being obtained. This is the handle returned by 
 * 				vSemaphoreCreateBinary ();
 * xBlockTime 	The time in ticks to wait for the semaphore to become available. The macro 
 * 				portTICK_RATE_MS can be used to convert this to a real time. A block time 
 * 				of zero can be used to poll the semaphore.
 *******************************************************************************************
 * RETURN:
 * pdTRUE (1)	if the semaphore was obtained. 
 * pdFALSE (0)	if xBlockTime expired without the semaphore becoming available.
 *******************************************************************************************/
int pthread_mutex_lock(pthread_mutex_t *mutex){
	return (pdTRUE == xSemaphoreTake(*mutex, (portTickType)0)) ? 0 : 1;
}

/******************************************************************************************* 
 * int xSemaphoreGive( 
 * 						xSemaphoreHandle xSemaphore 
 * 					 );
 *******************************************************************************************
 * FUNCTION:
 * Macro to release a semaphore. The semaphore must of been created using vSemaphoreCreateBinary(), 
 * and obtained using vSemaphoreTake().
 * This must not be used from an ISR. See xSemaphoreGiveFromISR() for an alternative which 
 * can be used from an ISR.
 * xSemaphore 	A handle to the semaphore being released. This is the handle returned by 
 * 				vSemaphoreCreateBinary();
 *******************************************************************************************
 * RETURN:
 * pdTRUE (1)	if the semaphore was released. 
 * pdFALSE (0)	if an error occurred. Semaphores are implemented using queues. An error can
 * 				occur if there is no space on the queue to post a message - indicating that 
 * 				the semaphore was not first obtained correctly.
 ********************************************************************************************/
int pthread_mutex_unlock(pthread_mutex_t *mutex){
	return (pdTRUE == xSemaphoreGive(*mutex)) ? 0 : 1;	
}
