/************************************************************************************************
 * File: 			app_idle.c
 * Description:		subroutine in idle task
 ***********************************************************************************************/

#include <define.h>
#include <stddef.h>

/************************************************************************************************
 * void* idle_process(void* ptr)
 * +-- a subroutine that runs by Idle Task
 * +-- this function must not call usleep()
 * +-- this function must not implement infinite looping unless no other tasks is running 
 ************************************************************************************************/
void* idle_process(void* ptr)
{
    return NULL;	
}


