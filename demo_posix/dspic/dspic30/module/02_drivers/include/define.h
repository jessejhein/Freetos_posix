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
#define PWM_PRIMARY                 3   //Use OC4 (Channel 3)
#define ADC_PRIMARY                 4   //Use AN4 
#define DAC_PRIMARY                 DAC_CHB

#endif /*DEFINE_H_*/
