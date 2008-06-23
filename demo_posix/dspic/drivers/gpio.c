/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup gpio Keyboard
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

/************************************************************************************************
 * Buffers for GPIO data
 ************************************************************************************************/
static unsigned char gpio_buf[MAX_GPIO_BUF];
static unsigned char gpio_wr = 0;   //write pointer of cir buf
static unsigned char gpio_rd = 0;   //read pointer of cir buf
static int gpio_io_flag;
static unsigned char pkey_is_pressing;

/**
 * \brief get gpio port ready
 * \param flags accessing mode
 * \retval -1 error write mode is selected (errno = EROFS)
 * \retval 0 ok
 * \remarks  _TRIS should be set correctly for gpio
 */
int 
gpio_open(int flags)
{
  if(flags & O_RDWR || flags & O_WRONLY)
    {
      errno = EROFS;
      return -1;
    }
  else
    {
      gpio_io_flag = flags;
      KEYCONFIG();
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
gpio_read(unsigned char *buf)
{
  //Perform read if read operation is enabled
  if((gpio_io_flag & O_RDONLY) == O_RDONLY)
    {
      int next_data_pos;
      next_data_pos = pre_rd_cir254buf( gpio_wr, gpio_rd, MAX_GPIO_BUF );
      //Copy 1 byte when data is available
      if (next_data_pos!=255) 
        {
          *buf = gpio_buf[gpio_rd];          //copy the stored data to buf
          gpio_rd = next_data_pos;            //update the ptr
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
 * \brief detect enter key
 * \remarks used in idle task
 * 
 * \internal
 * Principle of ENTER key
 * 
 *     ------------------------|||                 |||-----------
 *                             |||-----------------|||
 *      
 *                             |<------- B --------->| 
 *     
 *          |<--A-->|       |        |                   |
 *       case0    case1    case1    case1              case2
 *        ->case1  ->case0  ->case0  ->case2            ->case0
 *                    ->case1  ->case1
 *     
 *           A -- each 60mSec to scan
 *           B -- after get a key, waiting until release of key
 * 
 * When pkey_scan_cnt[i] < 6 (due to os_time() returning values ~0xFFFF)
 * scan frequency will be decreased <60ms
 */
void 
gpio_enter_key(void)
{
  static unsigned char pkey_state[TOTAL_PUSH_KEY];
  static unsigned int pkey_scan_cnt[TOTAL_PUSH_KEY];

  int i, key_id, pressed;
  for(i=0, key_id=BASE_PUSH_KEY; i<TOTAL_PUSH_KEY; i++, key_id++)
    {
      switch (pkey_state[i])
        {
          case 0:
            //Set detect period as 6 epochs (60ms)
            pkey_scan_cnt[i] = (unsigned int) os_time((time_t*) NULL) + 6;
            pkey_state[i]++;
            break;
          case 1:
            //Time's up, check for key pressed
            if(pkey_scan_cnt[i]<=( (unsigned int) os_time((time_t*) NULL) )) 
              {
                //Key has pressed
                KEY_PRESS(key_id, pressed);
                if(pressed) 
                  {
                    pkey_is_pressing = 1;
                    unsigned char next_data_pos;
                    next_data_pos = pre_wr_cir254buf( gpio_wr, gpio_rd, MAX_GPIO_BUF);
                    if (next_data_pos!=255) 
                      {
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
                pkey_is_pressing = 0;
                pkey_state[i] = 0;      //Key has returned to state 1
              }
            break;
        }
    }
}

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
                    next_data_pos = pre_wr_cir254buf( gpio_wr, gpio_rd, MAX_GPIO_BUF);
                    if (next_data_pos!=255) 
                      {
                        gpio_buf[gpio_wr] = save_key;
                        gpio_wr = next_data_pos;
                      }
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

#endif //end KB_MOD

/** @} */
/** @} */
