/************************************************************************************************
 * File: 			app_idle.c
 * Description:		subroutine in idle task
 ***********************************************************************************************/

#include <define.h>

/************************************************************************************************
 * tskIdle()
 * +-- a subroutine that runs by Idle Task
 * +-- this function must not call usleep()
 * +-- this function must not implement infinite looping unless no other tasks is running 
 ************************************************************************************************/
tskIdle()
{
	/* Schedule the co-routines from within the idle task hook. */
//	vCoRoutineSchedule();
}


