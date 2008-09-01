*freertos_posix is a POSIX wrapper for FreeRTOS. 
*The platform independent files (apart from those those under /posix/asm-dsPic) are saved under /posix. 
*Platform dependent files (such as drivers) are stored under demo_posix/xxx, where xxx is the name of your target platform.

*Not all POSIX functions are implemented, the following is the major functions developed. You are invited to contribute new functions and improve the existing ones.
#POSIX Thread: mapped to FreeRTOS task (pthread.h, and unistd.h)
#Mutex functions: mapped to FreeRTOS semaphore
#Filestream operations (e.g. open, read, write, lseek, etc): not associated with any FreeRTOS functions (unistd.h)
#Time and clock functions (e.g. time and clock): using the context switch timer required in FreeRTOS (time.h)
#Socket API: mapped to uip: Please note that the socket API is only posix-like and it is NOT compatible with standard POSIX operation.

*Extra functions include:
#a coroutine task scheduler, in order to run multiple POSIX threads in a single FreeRTOS Task (see pthread.c and system.c)

*You can also generate HTML documentations in the source code using doxygen (http://www.stack.nl/~dimitri/doxygen/). 
*Goto /posix and use the Doxyfile to generate documentation under /posix/doc 