/**
 * demo config
 */

#ifndef DEFINE_H_
#define DEFINE_H_


/********************************************************************************
 * Configurations for FreeRTOS and its POSIX
 ********************************************************************************/
#include "platform.h"
#include <pthread.h>

/** mutex for threads */
extern pthread_mutex_t myMutex;

/** global counter, shared by threads */
extern unsigned int counter;

#endif /* DEFINE_H_ */



