/*
 *	linlike8/include/asm-cypress_psoc/system.h
 *
 *	15-04-2004	yan	create this following linux
 *	30-04-2004	yan	modification from "directly back to process from switch" to "visit previous schedule subr.", i.e following
 *					* in linlike8/include/asm-cypress_psoc/system.h - switch_to() - ;add SP,-2      ; remove yield() return address \n"		\
 *
 */

#include <m8c.h>        // part specific constants and macros

#if (CONTEXT_SW==1)
		 // this assembly use A and virtaul registers
		 // assume SP and X index registers have not been modify from yield() to here
#define switch_to() do {				\
		asm(";add SP,-2								; remove yield() return address \n"		\
			"push X								; keep X index register\n"			\
			"swap A, SP \n							; save previous stack pointer, mov p->SP, SP\n"	\
			"mov [__r1], [_prev+1]\n"	\
			"mvi [__r1], A							; 	__r1 + 0, so stack ptr\n"		\
			"mov [__r1],[_current_ptr]					; get next stack pointer, mov SP, n->SP\n"	\
			"mov [__r0],0 \n"		\
			"asl [__r1] \n"			\
			"rlc [__r0] \n"			\
			"add [__r1],<_task						; 	_task+0, so stack ptr\n"		\
			"adc [__r0],>_task \n"		\
			"mvi A,[__r1] \n"		\
			"swap A, SP \n"			\
			"pop X\n");			\
	} while (0)									//	stack data from process to here
											//			<-	high memory
											//		X index
											//		L Process()
											//		H
											//		process stack	low memory
#endif
#define	LINLIKE8_SYS_STACK_SIZE	3							// in schedule(), this value comes from: (1) push processX PC, (2) X index for current process
		 // this assembly use A and virtaul registers, but must keep X index
#define get_process0_sp() do {				\
		asm("mov [__r1],<_task						; 	_task+0, so stack ptr\n"		\
			";mov [__r0],>_task \n"		\
			"swap A, SP \n"			\
			"mvi [__r1], A \n"		\
			"swap A, SP \n");		\
	} while (0)									// task[0].stack_ptr = SP
		 // place process stack content
#define place_process_stack() do {										\
		*( (unsigned int*) task[process_num].stack_ptr) = *( (const unsigned int*) process);		\
		task[process_num].stack_ptr += LINLIKE8_SYS_STACK_SIZE;						\
	} while (0)									// in cypress_psoc, function ptr as 16 bit data, so change to unsigned int to keep it
#define cli() M8C_DisableGInt
#define sti() M8C_EnableGInt
		// process0 must have stack size at least 12 bytes, since 
		//	4 - parameters of do_fork()
		//	2 - call do_fork()
		//	1 - X inside do_fork()
		//	2 - parameters for wake_up_process()
		//	2 - call wake_up_process()
		//	1 - X inside wake_up_process()
#define PROCESS0_SYS_APP_STACK_SIZE	(((PROCESS0_STACK_SIZE + LINLIKE8_SYS_STACK_SIZE)<12) ? 12 : (PROCESS0_STACK_SIZE + LINLIKE8_SYS_STACK_SIZE))

