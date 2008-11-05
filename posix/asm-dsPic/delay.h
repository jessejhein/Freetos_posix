/************************************************************************************************
 * File: 			delay.h
 * Description:		constants for looping delay implimented in delay.c
 ***********************************************************************************************/

#ifndef DELAY_H_
#define DELAY_H_    1

#include <define.h>

#ifndef SYSTEM_CLK_HZ
#error SYSTEM_CLK_HZ must be defined in define.h
#endif

/*
 * Pause for x us using looping, which requires 6 cycles of execution
 *   2*MOV(1)  + m*SUM( 2*SUB(1)+2*ADD(1) + 1*BRA(2) ) = 2 + m*6 cyc
 * 
 * When m -> 1, the 2 mov instr become significant, so not very accurate
 * for x -> 1
 *  
 * 
 *                  1us = 1/1,000,000 s
 *  <------------------------------------------------------->
 *          1 loop             2 loop               x loop
 *  <----------------------><---...---><---...---><---...--->
 *   _   _   _   _   _   _   _      
 * _| |_| |_| |_| |_| |_| |_| |_
 *  <--->
 *   Tcy = 1/SYSTEM_CLK_HZ
 */
#define udelay(x)			\
{							\
	unsigned long _dcnt;		\
	_dcnt=(x)*((unsigned long)SYSTEM_CLK_HZ/(6UL*1000000UL));	\
	while(_dcnt--);				\
}

/*
 * Pause for x ms using looping 
 */
#define mdelay(ms)			\
{							\
	unsigned int _mscnt = (ms); \
    while(_mscnt--)				\
    {							\
    	unsigned int i = 100; \
    	while(i--)				\
    	{						\
        	udelay(10);			\
    	}					\
    }						\
}

#endif /* DELAY_H_ */
