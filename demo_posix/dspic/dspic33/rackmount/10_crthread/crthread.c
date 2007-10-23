/*
 * crthread.c       Example of Coroutine Threads
 */

#include <define.h>

//---------------------------------------------------------
// Include this section of code to map 
//  start_process() -> scrBegin 
//  end_process()   -> scrFinish(0)
//  sleep()         -> scrReturn(-1)
//  usleep()        -> scrReturn(-1)
#ifdef FREERTOS_SCHED
#   undef FREERTOS_SCHED
#   undef start_process
#   undef end_process
#   include <coroutine_st.h>
#   define start_process()          scrBegin
#   define end_process()            scrFinish((void*)0)
#endif
#include <unistd.h>
//---------------------------------------------------------

//Local Variable
static unsigned char busy = 0;

/*
 * adj
 */
void* adj(unsigned char* ptr){
    
    start_process();
    
    while(busy == 1) usleep(0);
    
    busy = 1;
    while(printStr("adj: ") < 0)
        usleep(0);
    while(printDec((int)ptr[0]) < 0)
        usleep(0);
    while(printStr(" ") < 0)
        usleep(0);
    while(printDec((int)ptr[1]) < 0)
        usleep(0);
    while(newline() < 0)
        usleep(0);
    busy = 0;  
    
    end_process();
}

/*
 * enable
 */
void* enable(unsigned char* ptr){
  
    start_process();

    while(busy == 1) usleep(0);

    busy = 1;
    while(printStr("enable: ") < 0)
        usleep(0);
    while(printDec((unsigned int)ptr[0]) < 0)
        usleep(0);
    while(newline() < 0)
        usleep(0);
    busy = 0;
    
    end_process();
}

/*
 * disable
 */
void* disable(unsigned char* ptr){
    
    start_process();
    
    while(busy == 1) usleep(0);
    
    busy = 1;
    while(printStr("disable: ") < 0)
        usleep(0);
    while(printDec((int)ptr[0]) < 0)
        usleep(0);
    while(newline() < 0)
        usleep(0);
    busy = 0;

    end_process();
}
