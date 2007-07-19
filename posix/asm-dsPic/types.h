/************************************************************************************************
 * File: 			types.h
 ***********************************************************************************************/

#ifndef TYPES_H_
#define TYPES_H_    1

/*
 * The following data type location based on C30 Compiler
 * refer to "dsPIC30F language tools quick reference card"
 */
typedef unsigned char          __u8;
typedef unsigned int           __u16;
typedef int                     __s16;
typedef unsigned long          __u32;
typedef long                    __s32;
typedef unsigned long long    __u64;
typedef long long              __s64;

/*
 * Data type for time for <time.h>
 */
typedef unsigned long         time_t;

/*
 * Deprecated
 */
//#define OS_DATA_LOC     
//#define GUI_DATA_LOC   
//#define APP_DATA_LOC    

//#define CODE_LOC        const


#endif /*TYPES_H_*/
