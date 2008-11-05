/************************************************************************************************
 * File: 			define.h
 * Description:		Header file for kernel-driver interface
 ***********************************************************************************************/

#ifndef __DEFINE_H
#define __DEFINE_H

#define start_process()      while(1){
#define end_process()        }

/********************************************************************************
 * Define Application Tasks Here
 ********************************************************************************/
#define tskFlashLED()		void* tskFlashLED(void *ptr)

#endif

