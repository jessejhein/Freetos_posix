/************************************************************************************************
 * File:            app_idlehook.c
 * Description:     subroutine in idle task
 ***********************************************************************************************/

#include <define.h>
#include <pthread.h>

/*
 * Keyboard module: Rotory Key
 */
#if(KB_MOD > 0)
#include <time.h>
/************************************************************************************************
 * Principle of ENTER key
 * 
 * ------------------------|||                 |||-----------
 *                         |||-----------------|||
 *  
 *                         |<------- B --------->| 
 * 
 *      |<--A-->|       |        |                   |
 *   case0    case1    case1    case1              case2
 *    ->case1  ->case0  ->case0  ->case2            ->case0
 *                ->case1  ->case1
 * 
 *       A -- each 60mSec to scan
 *       B -- after get a key, waiting until release of key
 ************************************************************************************************/
static unsigned char pkey_state[TOTAL_PUSH_KEY];
static unsigned char pkey_scan_cnt[TOTAL_PUSH_KEY];

extern unsigned char gpio_buf[MAX_GPIO_BUF];
extern unsigned char gpio_wr;   //write pointer of cir buf
extern unsigned char gpio_rd;   //read pointer of cir buf
#endif

/*
 * Coroutine Thread
 */
#if(CRTHREAD_ENABLE > 0)
crthread_t crthread[MAX_CRTHREAD] = {NULL};
unsigned char crPendingCall[MAX_CRTHREAD];
void* crthread_arg[MAX_CRTHREAD];
#endif

/************************************************************************************************
 * void vApplicationIdleHook(void)
 * +-- a subroutine that runs by Idle Task
 * +-- Idle Task will run every 1/configTICK_RATE_HZ (i.e. 10ms) and when there are no other tasks 
 *     running
 * +-- this function must not call usleep()
 * +-- this function must not implement infinite looping unless no other tasks is running 
 ************************************************************************************************/
void vApplicationIdleHook(void)
{
#if(KB_MOD > 0)
    int i, key_id, pressed;
    /*
     * Scan push keys
     */
    for(i=0, key_id=BASE_PUSH_KEY; i<TOTAL_PUSH_KEY; i++, key_id++)
    {
        switch (pkey_state[i])
        {
            case 0:
                //Set detect period as 6 epochs (60ms)
                pkey_scan_cnt[i] = (unsigned char) os_time((time_t*) NULL) + 6;
                pkey_state[i]++;
                break;
            case 1:
                //Time's up, check for key pressed
                if(pkey_scan_cnt[i]==( (unsigned char) os_time((time_t*) NULL) )) 
                {    
                    //Key has pressed
                    KEY_PRESS(key_id, pressed);
                    if(pressed) 
                    {
                        unsigned char next_data_pos;
                        next_data_pos = pre_wr_cir254buf( gpio_wr, gpio_rd, MAX_GPIO_BUF);
                        if (next_data_pos!=255) {
                            gpio_buf[gpio_wr] = (unsigned char) key_id;
                            gpio_wr = next_data_pos;
                        }
                        pkey_state[i]++;    //check for key release
                    }
                    //No Key pushed 
                    else
                    {
                        pkey_state[i] = 0;
                    }
                }
                break;
            case 2:
                //Key has released
                KEY_PRESS(key_id, pressed);
                if( !pressed ) 
                {
                    pkey_state[i] = 0;      //Key has returned to state 1
                }
                break;
        } 
    }
#endif //end KB_MOD

#if(CRTHREAD_ENABLE > 0)
    unsigned char i;
    for(i=0; i<MAX_CRTHREAD; i++)
    {
        //If function is not NULL
        //Check if there is pending calls needed to perform
        //Decrement pending call if crFunction has completed once
        if(crthread[i] != NULL){
            if(crPendingCall[i] > 0){
                if(*( (*crthread[i])(crthread_arg) ) == 0){
                    crPendingCall[i]--;
                }
            }
        }
    }
#endif

    /*
     * Application Idle Task
     */
    UserIdleTask();     //define this task in define.h, 
                        //  #define idle_process     UserIdleTask
}
