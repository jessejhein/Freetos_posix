/**
 * \file
 * Idle task
 * \author Dennis Tsang <dennis@amonics.com>
 */

/**
 * \page app_idlehook Idle task
 * 
 * \section sec1 INTRODUCTION
 * \par Idle task in FreeRTOS
 * Functions include:
 * \li detect push key
 * \li detect function key
 * \li update led status
 * \li user thread
 * 
 * \par Configuration
 * Define user task (void* foo(void*)) in define.h, and implement it in project 
 * \verbatim
    #define UserIdleTask()                  foo(NULL)
   \endverbatim
 * 
 * \par Limitations
 * \li do not call usleep() to suspend task (coroutine sleep is ok)
 * \li do not implement infinite looping unless no other tasks is running
 */

/*
 * Copyright (C) 2007-2011  Dennis Tsang <dennis@amonics.com>
 *
 * This file is part of freertos_posix.
 *
 * freertos_posix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * freertos_posix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with freertos_posix.  If not, see <http://www.gnu.org/licenses/>
 */

#include <define.h>
#include <stddef.h>
#include <key/kb.h>
#include <gpio/led.h>

/**
 * \brief Idle Task
 */
void 
vApplicationIdleHook(void)
{
#ifdef KB_PUSH_KEY
  kb_push_key ();
#endif /* KB_PUSH_KEY */

#ifdef KB_FN_KEY
  kb_fn_key ();
#endif /* KB_FN_KEY */

#ifdef LED_MOD
  led_ctrl (NULL);
#endif /* LED_MOD */

  //Application Idle Task
  UserIdleTask ();
}
