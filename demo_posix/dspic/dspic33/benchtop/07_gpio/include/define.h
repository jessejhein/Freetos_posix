/************************************************************************************************
 * File:            define.h
 * Description:     this Apllication configurations
 ************************************************************************************************/


#ifndef DEFINE_H_
#define DEFINE_H_


/********************************************************************************
 * Configurations for FreeRTOS and its POSIX
 ********************************************************************************/
#include "platform.h"
#include "pin_define.h"


// Define User Idle Task Here **************************************************
#define UserIdleTask()              idle_process(NULL)

//  =============================================================================
#define DAC_PRIMARY                 DAC_CHB
// keyboard
#define     GR_KEY_ENTER            ((unsigned) 0)
#define     GR_SPECIAL_KEY_ARROW_UP     ((unsigned) 1)
#define     GR_SPECIAL_KEY_ARROW_DOWN   ((unsigned) 2)

#endif /*DEFINE_H_*/
