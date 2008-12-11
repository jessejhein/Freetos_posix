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
#ifdef KB_PUSH_KEY
      KEYCONFIG();
#endif /* KB_PUSHKEY */
#ifdef KB_ROTATE_KEY
      RKEYCONFIG();
#endif /* KB_ROTARY */
#ifdef KB_FN_KEY
      FN_KEYCONFIG();
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
  static unsigned char rkey_state[TOTAL_ROTARY_KEY];

  if(pkey_is_pressing == 0)
    {
      //Scan all rotary keys
      for(i=0, key_id=BASE_ROTARY_KEY; i<TOTAL_ROTARY_KEY; i++, key_id+=2)
        {
          //Check A status
          RKEY_STATE(key_id, high);
          current_key[i] = (high)? (current_key[i] | 0x10) : (current_key[i] & 0x01);
          //Check B status
          RKEY_STATE(key_id+1, high);
          current_key[i] = (high)? (current_key[i] | 0x01) : (current_key[i] & 0x10);
            
          switch(rkey_state[i])
            {
              case 0:
                //Change from 0x00
                if(current_key[i] == 0x01)
                  {
                    save_key = (unsigned char) key_id+1;   //key turned down
                    rkey_state[i] = 1;
                  }
                else if(current_key[i] == 0x10)
                  {
                    save_key = (unsigned char) key_id;     //key turned up
                    rkey_state[i] = 1;
                  }
                break;
              case 1:
                //Change from 0xa~a
                rkey_state[i] = (current_key[i] == 0x11) ? 2 : 0;
                break;
              case 2:
                //Change from 0x11
                if(  (current_key[i] == 0x01 && save_key == (unsigned char) key_id) 
                      || (current_key[i] == 0x10 && save_key == (unsigned char) key_id+1)   )
                  {
                    rkey_state[i] = 3;
                  }
                else
                  {
                    rkey_state[i] = 0;
                  }
                break;
              case 3:
                //Change from 0x~aa
                if(current_key[i] == 0x00)
                  {
                    kb_write(save_key);
                  }
                rkey_state[i] = 0;
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
#   include <coroutine_st.h>
#   define start_process()          scrBegin
#   define end_process()            scrFinish((void*)0)
#endif
#include <unistd.h>
//-----------------------------------------------------------------------------------------------
#ifdef KB_PUSH_KEY
/* save time */
static clock_t pkey_save_time[TOTAL_PUSH_KEY];

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
check_push_key(unsigned char i, unsigned char key_id)
{
  start_process();
  
  int pressed;
  KEY_PRESS(key_id, pressed);
  if(pressed)
    {
      pkey_is_pressing = 1;
      //key has pressed for at least KB_SCAN_PERIOD
      kb_write(key_id);
          
      //check for release key
      while(1)
        {
          pkey_save_time[i] = clock();
          while( ((clock_t) (clock() - pkey_save_time[i])) < KB_SCAN_PERIOD ) usleep(0);
          KEY_PRESS(key_id, pressed);
          if(pressed)
            {
              //key continue to press
              kb_write(key_id);
            }
          else
            {
              //key released
              pkey_is_pressing = 0;
              kb_write(key_id | 0x80);
              break;
            }
        }
      
      //time lag to multiple firing of key
      pkey_save_time[i] = clock();
      while( ((clock_t) (clock() - pkey_save_time[i])) < KB_SCAN_PERIOD ) usleep(0);
    }
  else
    {
      //no key pressed
      pkey_is_pressing = 0;
    }

  end_process();
}


/**
 * \brief detect push key
 * \remarks used in idle task
 */
void* 
kb_push_key(void)
{
  start_process();
  
  unsigned char i;
  for(i=0; i<TOTAL_PUSH_KEY; i++)
    {
      check_push_key(i, BASE_PUSH_KEY+i);
    }
  
  end_process(); 
}
#endif /* KB_PUSH_KEY */


//------------------------------------------------------------------------------
#ifdef KB_FN_KEY
/* save time */
static clock_t fn_key_save_time[TOTAL_FN_KEY];

/**
 * \internal
 * Principle of FUNCTION KEY same as PUSH KEY
 */
static void*
check_fn_key(unsigned char i, unsigned char key_id)
{
  start_process();
  
  int pressed;
  FN_KEY_PRESS(key_id, pressed);
  if(pressed)
    {
      pkey_is_pressing = 1;
      //key has pressed for at least KB_SCAN_PERIOD
      kb_write(key_id);
          
      //check for release key
      while(1)
        {
          fn_key_save_time[i] = clock();
          while( ((clock_t) (clock() - fn_key_save_time[i])) < KB_SCAN_PERIOD ) usleep(0);
          FN_KEY_PRESS(key_id, pressed);
          if(pressed)
            {
              //key continue to press
              kb_write(key_id);
            }
          else
            {
              //key released
              pkey_is_pressing = 0;
              kb_write(key_id | 0x80);
              break;
            }
        }
      
      //time lag to multiple firing of key
      fn_key_save_time[i] = clock();
      while( ((clock_t) (clock() - fn_key_save_time[i])) < KB_SCAN_PERIOD ) usleep(0);
    }
  else
    {
      //no key pressed
      pkey_is_pressing = 0;
    }

  end_process();
}


/**
 * \brief detect push key
 * \remarks used in idle task
 */
void* 
kb_fn_key(void)
{
  start_process();
  
  unsigned char i;
  for(i=0; i<TOTAL_FN_KEY; i++)
    {
      check_fn_key(i, BASE_PUSH_KEY+i);
    }
  
  end_process(); 
}
#endif /* KB_FN_KEY */

#endif /* KB_MOD */

/** @} */
/** @} */
