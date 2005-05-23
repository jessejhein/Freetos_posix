ReadMe.txt of linlike8(linux-like in 8-bit mcu) 

Contents: (arithmetical order)
* API (linux-like, i.e. POSIX)
* Contact
* Description 
* Directory Structure 
* Examples
* Experiences in any technique problem
* Features
* GUI
* Modules Design Description
* Parameters
* Post to
* Process 
* Pseudo code
* References
* Requirements
* Supporting Devices
* ToDo
* Version Number

========================================================================================================
Version Number (0.01.XX is testing version, 0.02.XX will be stable version)
        0.01.05 : 
                * i2C base in cypress psoc : bug fixed; in switch mode only
                * semaphore lib.; in switch mode only
                * adding a user_code/ which place the user application code that user can modify by themselve
                * arch/dsl and arch/i386 do not work
        0.01.04 : 
                * switching in compile for push key OR rotary key
        0.01.03 : 
                * i2C base in cypress psoc
                * alarm features
                * adding uart features
                * scan key
                * active which process to run 1st after switch
                * adding timer interrupt to switch process
                * adding msleep()
        0.01.02 : 03-05-2004
                * adding i386_linux platform, let user to simulate the application code in this platform
                * OS API
                        * yield() => sched_yield()
                * adding timer features : sleep()
        0.01.01 : 26-4-2004
                * solve the symbolic-link fault from 0.01.00 and release again
        0.01.00 : 9-4-2004
                * first creation of this linlike8
                * contains code to swtich 2 processes by yield() and do it mannually by programmer

========================================================================================================
Description 
* the name of this os as linux like os in 8-bit mcu
* This os runs inside small mcu devices, most are 8-bit mcu, such as 51 family, Cypress PSoC ...
* Linux like os, since Linux can't support this small device, but want to keep likely linux and easy to port the application back to linux 
	* such as
		* coding style
		* API name (i.e. POSIX API)
		* ... 
        * it is because linux is a open source and very good os, we choose it, and we do not need to maintain a os by ourself, we can concentrate in development applications. However, we hope the application code can be re-used, and we also have 8-bit mcu application, so we develope a linux like os in 8-bit. 

========================================================================================================
Features 
* manually to yield running process switching to next process
* sleep process
* alarm (but do not test 2 or above alarm in system)
* timer interrupt to switch process
* active which process to run 1st after switch
* latency = how many process number * Tick time(10mSec)
* scan key
* serial uart
* do not support multi-mcus(since they cannot share memory if multi-mcus are used, their memory is embedded inside chips nowadays. A whole system can be multi-mcus, but they just can comm. (such as I2C) bet. each other, but seperated linlike8 inside each mcu

========================================================================================================
Supporting Devices (Platforms)
* i386 linux platform - base i386 linux environment (we SHOULD not implement the kernel code on top of I386 linux environment, since I386 linux platform has already been a very good os, and we can develop application on top of it and run)
* Cypress PSoC
        * CY8C27XXX

========================================================================================================
Requirements
* Task must adding a forever loop, do not return back to system, otherwise system hang; so return type as void of each task
* memory size
        * ROM - 361 bytes (including PSoC system and its booting code) => in version 0.01.01 
        * RAM - 23 bytes => in version 0.01.01 
* idle process(i.e. process0), must always running, can NOT do any sleeping; Otherwise whole system is hang in scheduing
* ROM size must under 65280 bytes
* task number must under 255

========================================================================================================
Parameters
* how to create new task 
        * create a new C file for a new process
        * modify the app.h for 
                * process0 entry
                * entern declar of process0
        * take a guess for stack size of each process
        * how many running task - NR_TASKS in app.h
        * processX stack size - PROCESSX_STACK_SIZE in app.h
* modules selection
        * config.h
        * app.h - modulars
                * enable module - set to 1 
                * disable module - set to 0

========================================================================================================
GUI - see mwlike8

========================================================================================================
Experiences in any technique problem
* General 
        * Case #0
                * situation => process is hang after running for a while
                * reason => not enough stack for process to run, after a while, especially, some interrupt is coming when your code in a high point of stack
                * solution => give more stack size for each process(in app.h)
        * Case #1
                * situation => when a new process doesn't run in sleep() to switch, but it is running in sched_yield()
                * reason => not enough timer 
                * solution => you forget to give enough timer space in app.h
* Cypress PSoC
        * some in http://202.94.233.75/open_data/ElectronicComponents/uP/PSoC/info.txt - Technical experiences
        * Case #0
                * situation => timer function is hang
                * solution => in boot.asm, its interrupt vector for timer is changed to its default value, we must manually to change back to _timer_interrupt for linlike8 timer ISR

========================================================================================================
API (linux-like, i.e. POSIX)
* schedule, including sched.h
        * sched_yield()
                * release current process, let scheduler to place another process to run
                * example 
                        * process 1
                                * ...
                                * while (check flag) sched_yield();   // switch to other process until flag is false
                                * ...
* timer, including timer.h
        * sleep(unsigned char second)
                * let this process to sleep for number of seconds, after time out, wake up automatically again
                * example 
                        * process 1
                                * ...
                                * sleep(2)
                                * ...                           // after 2 seconds and wake-up again to cont. to run
        * msleep(unsigned char milli-second)
                * i know this is not a POSIX API, but i need this now. If time available, i will do the usleep() to replace this one.
        * alarm(p_func function,unsigned char sec)
        * malarm(p_func function, unsigned int msec)
* i2c
        * master
                * i2c_write(buf_ptr, length, slave_id);
                * i2c_read(buf_ptr, length, slave_id);

========================================================================================================
Process 
* Process 0 
        * system process, cannot sleep
        * scan key process if keyboard module enable

========================================================================================================
Examples
* general 
        * test applicatons
                * arch 
                        * Cypress PSoC
                                * CY8C27XXX
                                        * adding hardware port mode => all ports as strong except P2.1 as High-Z (no meaning, just for my circuti only)
                                        * P2.7, P4.7 as signal pin for LEDs
                                        * 24MHz clock
                                        * VC1/6
                                        * (Vdd/2)+/-(Vdd/2)
                * Process0 and Process1 always 1's complement of signal pin
        * scan key
* Cypress PSoC - CY27CXXXX (if tested in other arch., we can move following to General)
        * analog multiplexer

========================================================================================================
Modules Design Description

-----------------------------------------------------------------------------------------------------
Flowing graphic
        +-------------------------------+
        | arch/X_devices/boot/boot.c::  |
        |-------------------------------|
        | C Compiler main():            |
        |   hardware init               |
        |   jmp start_kernel            |
        +-------------------------------+
                        |
                        |<--------------------- ... 
                        v
        +-------------------------------+
        | init/main.c::                 |
        |-------------------------------|
        | start_kernel:                 |
        |   setup_arch                  |
        |   sched_init                  |
        |   time_init                   |
        |   softirq_init                |
        |   kernel_thread               |
        |   process0                    |
        +-------------------------------+
        
        where 
                kernel_thread
                        create "Init Process", PID as 1, by calling fork() (including wake-up command)
                        do_fork()
                                allocate task[]
                                place stack pointer
                                place the start address of process
                                set to task_running
                process0
                        it is a Forever Loop, call Change Cpu Sleep command to save power, then call yield() to enter to other process
                        however, it also can be one of applcation task which needs to looping for something

-----------------------------------------------------------------------------------------------------
Scheduling
* linux like, but due to low power mcu and simple, so 
        * user application calls sched_yield() by itself to exit this task and switch to other application
* Processes concept
        * ReadMe1.gif
                * base linux concept
                * but remove 
                        * User-level thread
                        * Threads Library
                * handle processes
* method
        * Flowing graphic
        +-------------------------------+       +-------------------------------+       +-------------------------------+
        | processX1.c::                 |       | kernel/sched.c::              |       | include/archX/system.h::      |
        |-------------------------------|       |-------------------------------|       |-------------------------------|
        | ...                           |  +--->| sched_yield:                  |    +->| switch_to:                    |
        | user application code         |  |    |   push old_process_addr       |    |  |   hw dependent code           |
        | ...                           |  |    |   enter critical time         |    |  +-------------------------------+
        | sched_yield                   |--+    |   schedule                    |--+ |  
        | ...                           |       |                               |  | |
        | user application code         |       | schedule:                     |  | |
        | ...                           |       |   push yield_addr             |<-+ |
        +-------------------------------+       |   read current task pointer   |    |
                                                |   copy current task struct    |    |
        +-------------------------------+       |   seek a TASK_RUNNING task    |    |
        | processX2.c::                 |       |   adj SP to cancel yield_addr |    |
        |-------------------------------|       |   switch to new task          |----+
        | ...                           |       |   pop directly back to process|
        | user application code         |       |   leave critical time         |
        | ...                           |       +-------------------------------+
        | sched_yield                   |
        | ...                           |       
        | user application code         |       
        | ...                           |       
        +-------------------------------+

                Remarks
                        * enter critical time / leave critical time
                                * stop all interrupts, but make sure the interrupt is pending, later to handle it
                
        * Memory 
                                                +---------------+
                                                |               | high memory
                                                | ...           |
                                                |               |
                                                | ------------- |
                                                | ...           |
                                                |               |
                                                | mcu regs.     |
                                                | LPC           |
                                                | HPC           |
                                                |               |
                                                | ...           |
                                 P1 stack start |               |
                                                | ------------- |
                                                | ...           |
                                                |               |
                                                | mcu regs.     |
                                                | LPC           |
                                                | HPC           |
                                                |               |
                                                | ...           |
                                 P0 stack start |               |
                                                | ------------- |
                                                | ...           |
                                                | ------------- |
                                 state          |               |
                             P1, stack ptr(SP)  |               | 7 
                                                | ------------- |
                                 state          |               |
                task struct: P0, stack ptr(SP)  |               | 5 
                                                | ------------- |
                                                | ...           |
                                                |               | low memory
                                                +---------------+

-----------------------------------------------------------------------------------------------------
Interrupt
* Flowing

        different interrupt sources
        |               |               |               |       ... 
        V               V               V               V       
      ISR#0           ISR#1           ISR#2           ISR#3                     // interrupt are disable in all ISR#*
     mark bh0        mark bh1        mark bh2        mark bh3                   // i changed the name also from bh to softirq
        |               |               |               |
        +---------------+---------------+---------------+
                                |
                                V
                            do_softirq
                       disable softirq nesting
                       enable interrupt again
                do all available softirqs by looping
                               reti
                               
        +-------------------------------+
        | kernel/softirq.c::            |
        |-------------------------------|
        | variable:                     |
        |   softirq_vec                 |
        | softirq_init:                 |
        |   init. cpu state             |
        | do_softirq:                   |
        |   disable softirq nesting     |
        |   enable interrupt again      |
        |   call to soft irqs           |
        +-------------------------------+
                               
* 8 bit global variable for softirq vector (softirq_vec)
        * bit 0 - timer softirq              
* problem in stack, if interrupt is coming, ISR will use the current process stack, so we need to prepare more buffer stack for each process

-----------------------------------------------------------------------------------------------------
Time Management
* each tick between 10mSec
* Flowing
        * init.
                +-------------------------------+
                | arch/X/kernel/time.c::        |
                |-------------------------------|
                | time_init: (fr. start_kernel) |
                |   timer hw init.              |
                +-------------------------------+
        * interrupt 
                +-------------------------------+       +-------------------------------+       +---------------------------------------+
                | arch/X/kernel/time.c::        |       | kernel/timer.c::              |       | include/linlike8/interrupt.h::        |
                |-------------------------------|       |-------------------------------|       |---------------------------------------|
                | timer_interrupt:              |       | variable:                     |   +-->| mark_bh:                              |
                |   do_timer                    |----+  |   jiffies                     |   |   |   set soft irq vector                 |
                |   do_softirq                  |-+  +->| do_timer:                     |   |   +---------------------------------------+   
                |   schedule                    | |     |   update jiffies              |   |
                +-------------------------------+ |     |   mark_bh(0) 0=> timer tasklet|---+
                                                  |     | timer_softirq:                |
                                  do_softirq <----+     |   chk timer event, if match   |
                                                        |       wake-up this process    |
                                                        +-------------------------------+
        * general API - sleep()
                +-------------------------------+
                | kernel/sched.c                |
                |-------------------------------|
                | schedule_timeout(sleep):      |
                |   set process to INTERRUPTIBLE|
                |   add timer                   |
                |   schedule                    |
                |   del timer                   |
                +-------------------------------+
* procedures of each timer interrupt (start from down to top)
        pop PCH
        pop PCL
        pop CPU_F       // it is RETI
        pop regs
        <===> switch
        ISR content
        push regs
        set PCL and PCH
        clr CPU_F, disable intr.
        push CPU_F
        push PCL
        push PCH
* Memory in timer switching                
        stack memory
                regs.
                CPU_F
                PCL
                PCH
                running app. stack

-----------------------------------------------------------------------------------------------------
Memory Management
* share gobal variable(s) in different processes
        * it is be supported, since linlike8 does not real to fork process with different memory pages, processes can access all memory. 
        * 8-bit system does not need to think about security and some virus to attack other process, since all processes are work cooperated under a project. Never, a new process from outside world to attack this system.
        * however, it is dangerous for data corrupt during data accessing after interrupt enable and future preemptive situation
        * it is fast way to share data, and save code size(do not need to make some code to handle this)

-----------------------------------------------------------------------------------------------------
Keyboard Management
* push key implementation
        * hardware setting for this implement method
                        o Vcc
                        |
                        R
                        |	-----
                    o---+-------o   o---+
                       cpu port		|
                                        V Gnd
        * timing 
                    ----+++		+++---------------------+++----------
                        |||		|||			|||
                        +++-------------+++ - - - - - - - - - - +++
                        A  B		C   			D
                A -- 1st interrupt
                B -- after debouncing, should read data from this moment
                C, D -- UP event trigger
        * procedures 
                * interrupt is coming                                           // in interrupt
                * delay for a 10mSec for debouncing
                * chk keyboard                                                  // in alarm subr.
                        * chk which key, then run corresponding appl. code
        * method                
                * using interrupt, interrupt to trigger key(s) input (more good response to user, by interrupt, do not miss any key-in)
        * interface 
                * appl. code in gpio after key-in detection
* rotary key implementation
        * timing 
              A  EFEFEF||      ||EFEFEFEFEF----||       ||-------||       ||-------
                       ||ERERER||              ||-------||       ||-------||
        
              B  ----------||      ||--------------||       ||--------||       ||----      clockwise
                           ||------||              ||-------||        ||-------||
        
           OR B  ---||     ||-------------||       ||-------||        ||-------||          anti-clockwise
                    ||-----||             ||-------||       ||--------||       ||----
                     ^              ^
                     |              |
                     <--- 1 kick --->          --------- cont. rotary ------------>
              Remark : 
                || -- debouncing 
                ER -- enable rising
                EF -- enable falling
        * procedures 
                * after interrupt(falling) from pin A
                        * enable pin A interrupt(rising)
                * after interrupt(rising) from pin A
                        * if pin B is low, clockwise
                        * if pin B is high, anti-clockwise
        * references
                * cypress
                        * http://www.cypressmicro.com/support/appnotes/an2086.pdf - 500uSec for debouncing by a new counter
                        * google search of "encoder switch" inside http://www.cypressmicro.com/
                
-----------------------------------------------------------------------------------------------------
Uart
* Parameters
	* select in %ROOT_DIR%/config.h
* Block Diagram
		   ...
			|
		linlike8
			|
		  psoc--+--uart0
* API(see %ROOT_DIR%/include/asm-cypress_psoc/serial.h)
* Source
	* %ROOT_DIR%/arch/cypress_psoc/drivers/serial.c
	* %ROOT_DIR%/include/asm-cypress_psoc/serial.h
	* %ROOT_DIR%/config.h
-----------------------------------------------------------------------------------------------------
Standard Lib
* Block Diagram
		   ...
			|
		linlike8
			|
		standard lib
			|
		  psoc--+--uart0
		  		+--i2c0
* API(see %ROOT_DIR%/include/linlike8/unistd.h)
* Source
	* %ROOT_DIR%/lib/unistd.c

========================================================================================================
Testing Done for 
* 2 processes forever loop. They will be switched each 10 mSec.
* 1 processes forever loop, another calling sched_yield(). 1st always to run, 2nd one just run once each 10 mSec.
* 1 processes forever loop, another calling sleep(1). 1st always to run, 2nd one just run once each 1 Sec.

========================================================================================================
Directory Structure 
* same to linux 
* added 
 Root Directory
        |
        |-- test_apps/ (testing applications for linlike8, as a examples to user how to use linlike8) 
        |       |
        |       |-- app.h (declaration of different features and parameters of this application) 
        |

========================================================================================================
Post to
* http://www.psocdeveloper.com/
* http://linuxdevices.com/
* http://www.cypress.com/forums/

========================================================================================================
Contact
* yan - yan@amonics.com

========================================================================================================
References
* other non-linux 8-bit os 
        * Small RTOS51 - http://www.zlgmcu.com/
        * The Contiki Operating System and Desktop Environment - http://dunkels.com/adam/contiki/
        * uC/OS-II - http://www.ucos-ii.com/
* existing solutions from other companies
        * http://www.hhcn.com/chinese/cnindex.html
        
========================================================================================================
ToDo
* hang inside scheduing due to all processes to sleep, solve it
* sleep() => change to marco, then reduce some source code in size for X100
* seperate libc from kernel linlike8
* general date type, eg. uint8, see linux format, see the request
        * __u32 in linux for 32 bit variable, study it
* process0 cannot sleep => make scheduler so complex, just leave this as one process for no sleep
* forum feedback
* study the advance linux programming 
* change some examples proces to libraries
* libc, placed inside kernel, later to seperate
        * such as sleep() ... 
* diff. processes how to comm., may be better by global variable
* how to cal. the stack (or by auto. method) of one process
* seek the enable interrupt point in init. from linux, where to place it
* reduce memory size in process0 and process1 in system usage
* task[process_num]
        self-written assembly only support 2 tasks, if more not work
        if not 2 bytes of task_struct, >2 bytes
* i386_linux platform does NOT work, hope to run this linlike8 on i386_linux for simulation, then easy to debug for applicaiton
* wireless, TCP/IP in embedded os, go to study it - http://www.wsdmag.com/Articles/Index.cfm?ArticleID=6539&pg=3
* if time valid, seek some linux application info. to read as references => learn more techinique to write linux applicaiton program
* chk 
        * reentrant
        * global variable 
* keyboard
        * using old method to implement, later to change back to linux method, as one of file, using open(), read(), write() to access
        * related to mwlike8
        * scan key
* serial - do as following linux (device driver)
        * adding UART
                after device driver book
                http://www.linuxdevices.com/articles/AT5727959130.html
                http://www.linuxdevices.com/articles/AT3711365653.html
                Serial Programming Guide for POSIX Operating Systems - http://www.easysw.com/~mike/serial/serial.html
        * Uart
                master (can't existing with slave together)
                        // "called by others", not timing critical
                        // Description : any packages sending out
                        // input : any parameters from upper caller
                        // output : return values to upper caller
                        // todo : 
                        //      * improve resending for handling error tx
                        diff. cmd. as diff. subr.
                                reset cmd
                                        set sending counter
                                        packet a package
                                        send out 
                                        SENDING : if counter full, set false and goto RETURN
                                        inc. counter
                                        start timeout 50mS_timer
                                        circular
                                                Rec. whole package and wait ACK
                                                check timeout 50mS_timer, goto SENDING
                                                sleep (return wait)
                                        start timeout 50mS_timer
                                        circular
                                                Rec. whole package and wait completed 
                                                check timeout 50mS_timer, set false and goto RETURN
                                                sleep (return wait)
                                        FEEDBACK :send response (true/false)
                                        RETURN : return true/false
                                ...
                  #include "uart.h"
                uartInit();
                uart();
                UART/RS232, protocol
                PSOC : serial port in 2 uart, can't do it in same time
        tty layer
                driver_name
        TTY_DRIVER_MAGIC
        tty driver                
keyboard => disable the keyboard by setting the paremeter
        no repeat key needs for encoder switch
* change linlike8 all kernel api as sys_XXXX
* matching posxi
        serial 
                read(&temp) ==> 
                        #define read(temp,1)    serial_rd(temp)
                

