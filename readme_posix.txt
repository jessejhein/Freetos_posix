INTRODUCTION
============
All source codes found under freertos_posix is designed to work in companion with 
FreeRTOS version 4.1.3. FreeRTOS is downloadable from http://www.freertos.org/
or https://freertos.svn.sourceforge.net/svnroot/freertos


STRUCTURE
=========
+--Source			:The files in this folder should replace the ones in FreeRTOS
	+--include			:portable.h added a new port for dsPic30 series uP 
	+--portable			:port.c added a new port for dsPic30 series uP
					 	:portmacro.h is identical to FreeRTOS/Source/portable/MPLAB/PIC24_dsPIC/portmacro.h

+--demo_posix		:Application source codes for different uP
	+--common			:hardware independent files
	+--dspic			:hardware dependent files
		+--drivers			:common drivers for dsPics
		+--dspic30			:dsPic30 specific files
		+--dspic33			:dsPic33 specific files
	+--XXXX				:add different families of uC

+--posix			:Contains POSIX API related header files (equivalent to /usr/include/)
	+--asm				:soft link to asm-XXXX
	+--asm-XXXX			:uC dependent includes
	+--bits				:definition of bits for control activities
	+--sys				:system includes
	+--src				:*.c implementation for POSIX
	


POINTS TO NOTE
==============
1)	In ./include/FreeRTOSConfig.h, modify the values to the followings
		a)	uP speed for 30MIPs
			#define configCPU_CLOCK_HZ				( ( unsigned portLONG ) 30000000 )
		b)	Set the desired kernel clock rate
			#define configTICK_RATE_HZ				( ( portTickType ) 100 )   	//10ms
		c)	Set desired HEAP_SIZE managed by kernel
			#define configTOTAL_HEAP_SIZE			( ( size_t ) 1024 )
		d)	Set desired stack size for task (e.g. sprintf() requires extra stack space)
			#define configMINIMAL_STACK_SIZE		( 205 )		//x2 for number of bytes
		e)	Reduce RAM requirement by lowering the priority
			#define configMAX_PRIORITIES			( ( unsigned portBASE_TYPE ) 1 )	//Use tskIDLE_PRIORITY only
		f)	Reduce RAM requirement by using cooperative scheduling
			#define configUSE_PREEMPTION			0	//Use cooperative scheduling
		g)	Reduce code size by excluding unnecessary task functions
			#define INCLUDE_vTaskPrioritySet		0
			#define INCLUDE_uxTaskPriorityGet		0
			#define INCLUDE_vTaskDelete				0
			#define INCLUDE_vTaskCleanUpResources	0
			#define INCLUDE_vTaskSuspend			0
			#define INCLUDE_vTaskDelayUntil			0
			#define INCLUDE_vTaskDelay				1


