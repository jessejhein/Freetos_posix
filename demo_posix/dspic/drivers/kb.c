/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup kb Keyboard
 * @{
 */

/**
 * \file
 * Keyboard Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

#ifdef KB_MOD

#include <define.h>
#include <fcntl.h>
#include <errno.h>
#include <asm/system.h>
#include <time.h>
#include <nano-X.h>


/************************************************************************************************
 * Buffers for KB data
 ************************************************************************************************/
static unsigned char kb_buf[MAX_KB_BUF];
static unsigned char kb_wr = 0;   //write pointer of cir buf
static unsigned char kb_rd = 0;   //read pointer of cir buf
static int kb_io_flag;
static unsigned char pkey_is_pressing;

/** structure for key used in reenterant coroutine */
struct kb_key_t
{
  unsigned char id;
  clock_t save_time;
  unsigned int cr_st;
};
#ifdef KB_PUSH_KEY
static struct kb_key_t push_key[TOTAL_PUSH_KEY];
#endif /* KB_PUSH_KEY */
#ifdef KB_FN_KEY
static struct kb_key_t fn_key[TOTAL_FN_KEY];
#endif /* KB_FN_KEY */

/**
 * \brief get kb port ready
 * \param flags accessing mode
 * \retval -1 error write mode is selected (errno = EROFS)
 * \retval 0 ok
 * \remarks  _TRIS should be set correctly for kb
 */
int 
kb_open(int flags)
{
  if(flags & O_RDWR || flags & O_WRONLY)
    {
      errno = EROFS;
      return -1;
    }
  else
    {
      kb_io_flag = flags;
      key_config();
#ifdef KB_PUSH_KEY
      int i;
      for(i=0; i<TOTAL_PUSH_KEY; i++)
        {
          push_key[i].id = BASE_PUSH_KEY + i;
        }
#endif /* KB_PUSHKEY */
#ifdef KB_FN_KEY
      int j;
      for(j=0; j<TOTAL_FN_KEY; j++)
        {
          fn_key[j].id = BASE_FN_KEY + j;
        }
#endif /* KB_FN_KEY */
      return 0;
    }    
}


/**
 * \brief copy the received data to buf
 * \param buf pointer to buffer for reading
 * \retval -1 error not opened for read operation (errno = EBADF)
 * \retval 0 indicating no data is available
 * \retval 1 indicating 1 byte has been read
 */
int 
kb_read(unsigned char *buf)
{
  //Perform read if read operation is enabled
  if((kb_io_flag & O_RDONLY) == O_RDONLY)
    {
      int next_data_pos;
      next_data_pos = pre_rd_cir254buf( kb_wr, kb_rd, MAX_KB_BUF );
      //Copy 1 byte when data is available
      if (next_data_pos!=255) 
        {
          *buf = kb_buf[kb_rd];          //copy the stored data to buf
          kb_rd = next_data_pos;            //update the ptr
          return 1;
        }
      //No data can be copied
      else
        {
          return 0;
        }      
    }
  //Error, raise error flag
  else
    {
      errno = EBADF;  //io not opened for reading
      return -1;
    }    
}


/**
 * \brief insert key id into circular buffer
 * \param key_id key id for writing
 * \retval 0 indicating no data is written (buffer is full
 * \retval 1 indicating 1 byte has been written
 */
static int 
kb_write(unsigned char key_id)
{
  unsigned char next_data_pos;
  next_data_pos = pre_wr_cir254buf( kb_wr, kb_rd, MAX_KB_BUF);
  if (next_data_pos!=255) 
    {
      kb_buf[kb_wr] = key_id;
      kb_wr = next_data_pos;
      return 1;
    }
  return 0;
}


#ifdef KB_ROTATE_KEY
/**
 * \brief detect rotary keys by change interrupt
 * \remarks priority is given to push key
 * 
 * \internal
 * Principle of ROTARY KEY
 * 
 * A               +--------------+
 *                 |              |
 *     ------------+              +------------------------------------
 *         
 * B                       +-------------+
 *                         |             |
 *     --------------------+             +----------------------------
 * 
 * or vice versa
 * 
 * Valid sequence:   00 -> 01|10 -> 11 -> 10|01 -> 00
 * 
 */
void _IRQ 
_CNInterrupt(void)
{
  int i, key_id, high;
  unsigned char next_data_pos;
  static unsigned char save_key;
  static unsigned char current_key[TOTAL_ROTARY_KEY];
  static unsigned char state[TOTAL_ROTARY_KEY];

  if(pkey_is_pressing == 0)
    {
      //Scan all rotary keys
      for(i=0, key_id=BASE_ROTARY_KEY; i<TOTAL_ROTARY_KEY; i++, key_id+=2)
        {
          //Check A status
          high = rkey_state(key_id);
          current_key[i] = (high)? (current_key[i] | 0x10) : (current_key[i] & 0x01);
          //Check B status
          high = rkey_state(key_id+1);
          current_key[i] = (high)? (current_key[i] | 0x01) : (current_key[i] & 0x10);
            
          switch(state[i])
            {
              case 0:
                //Change from 0x00
                if(current_key[i] == 0x01)
                  {
                    save_key = (unsigned char) key_id+1;   //key turned down
                    state[i] = 1;
                  }
                else if(current_key[i] == 0x10)
                  {
                    save_key = (unsigned char) key_id;     //key turned up
                    state[i] = 1;
                  }
                break;
              case 1:
                //Change from 0xa~a
                state[i] = (current_key[i] == 0x11) ? 2 : 0;
                break;
              case 2:
                //Change from 0x11
                if(  (current_key[i] == 0x01 && save_key == (unsigned char) key_id) 
                      || (current_key[i] == 0x10 && save_key == (unsigned char) key_id+1)   )
                  {
                    state[i] = 3;
                  }
                else
                  {
                    state[i] = 0;
                  }
                break;
              case 3:
                //Change from 0x~aa
                if(current_key[i] == 0x00)
                  {
                    kb_write(save_key);
                  }
                state[i] = 0;
                break;
            }
        }
    }
#ifdef MPLAB_DSPIC33_PORT
  _CNIF = 0;
#else /* MPLAB_DSPIC30_PORT */
  DISI_PROTECT(_CNIF = 0);
#endif /* MPLAB_DSPIC30_PORT */
}
#endif /* KB_ROTATE_KEY */


// this process wants to use coroutine_st instead of multi-thread when using FreeRTOS
#ifdef FREERTOS_SCHED 
#   undef FREERTOS_SCHED
#   undef start_process
#   undef end_process
#   undef usleep
#   undef sleep
#   define start_process()         switch(((struct kb_key_t*) arg)->cr_st) { case 0:;
#   define end_process()           ((struct kb_key_t*) arg)->cr_st = 0;} return ((void*)0)
#   define usleep(usec)            do {\
                                        (((struct kb_key_t*) arg)->cr_st)=__LINE__;\
                                        return ((void*)-1); case __LINE__:;\
                                     } while (0)
#   define sleep(sec)              usleep(sec)
#endif
//-----------------------------------------------------------------------------------------------
/**
 * \internal
 * Principle of PUSH key
 * 
 *    -----------|||                        |||----------------------------------
 *               |||------------------------|||
 *      
 *    <Scan for key>|<----- T ----->|<----- T ----->|<----- T ----->|<Scan for key>
 * 
 *                 /|\             /|\             /|\             /|\
 *                  |               |               |               |
 *                press           hold           release         new scan
 * 
 * T: scan period
 */
static void*
check_key(struct kb_key_t *arg)
{
  start_process();
  
  if(key_press(arg->id))
    {
      pkey_is_pressing = 1;
      //key has pressed for at least KB_SCAN_PERIOD
      kb_write(arg->id);

      //check for release key
      while(1)
        {
          arg->save_time = clock();
          while( ((clock_t) (clock() - arg->save_time)) < KB_SCAN_PERIOD ) usleep(0);
          if(key_press(arg->id))
            {
              //key continue to press
              kb_write(arg->id);
            }
          else
            {
              //key released
              pkey_is_pressing = 0;
              kb_write(arg->id | 0x80);
              break;
            }
        }
      
      //time lag to multiple firing of key
      arg->save_time = clock();
      while( ((clock_t) (clock() - arg->save_time)) < KB_SCAN_PERIOD ) usleep(0);
    }
  else
    {
      //no key pressed
      pkey_is_pressing = 0;
    }

  end_process();
}


//------------------------------------------------------------------------------
#ifdef KB_PUSH_KEY
/**
 * \brief detect push key
 * \remarks used in idle task
 */
void 
kb_push_key(void)
{
  unsigned char i;
  for(i=0; i<TOTAL_PUSH_KEY; i++)
    {
      check_key(&push_key[i]);
    }
}
#endif /* KB_PUSH_KEY */


//------------------------------------------------------------------------------
#ifdef KB_FN_KEY
/**
 * \brief detect push key
 * \remarks used in idle task
 */
void* 
kb_fn_key(void)
{
  unsigned char i;
  for(i=0; i<TOTAL_FN_KEY; i++)
    {
     check_key(&fn_key[i]);
    }
}
#endif /* KB_FN_KEY */

#endif /* KB_MOD */

/** @} */
/** @} */
