/**
 * \file
 * led task employing posix thread and file API
 * \author Dennis Tsang <dennis@amonics.com>
 */

#ifndef MUTEX_H_
#define MUTEX_H_

/**
 * \brief Increment $counter 5000 times, then prints "Ty: counter = x"
 * \param ptr [0]: thread index
 * \return never return
 */
extern void* tskMutex(void* ptr);

#endif /* LED_H_ */
