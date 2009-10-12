/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup gpdi General Purpose Digital Input
 * @{
 * \li The driver has a POSIX-compliance interface with open(), read(), and ioctl().
 * \li The driver is for 74LV165APW - Parallel-load 8-bit shift register
 */

/**
 * \file
 * General Purpose Digital Input Driver
 * \author Dennis Tsang <dennis@amonics.com>
 */

#include <define.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <time.h>
#include <asm/delay.h>
#include <asm/system.h>


/** read/write io flag */
static int gpdi_io_flag;
/** current channel selected for writing */
static unsigned int gpdi_ch_select;
/** gpdi status, upto 16 ch */
static __u16 gpdi_status;
/** start time for ctrl */
static clock_t start_time = 0;


int 
gpdi_open (int flags)
{
  gpdi_io_flag = flags;
  GPDI_CONFIG ();
  return 0;
}


int 
gpdi_read (unsigned char *buf)
{
  //Perform read if read operation is enabled
  if ((gpdi_io_flag & O_RDONLY) == O_RDONLY)
    {
      if ( (gpdi_ch_select < GPDI_NUM) && (gpdi_ch_select < 16))
        {
          *buf = (gpdi_status >> gpdi_ch_select) & 0x0001;
          return 1;
        }
      else
        {
          errno = EFBIG; //exceed maximum offset
          return -1;
        }
    }
  //Error, raise error flag
  else
    {
      errno = EBADF;  //io not opened for reading
      return -1;
    }    
}


int 
gpdi_ioctl (int request, unsigned char* argp)
{
  switch (request)
    {
      //Select current channel for writing
      case GPDI_SELECT_CH:
        {
          gpdi_ch_select = argp[0];
          break;
        }
      //request code not recognised
      default:
        {
          return -1;
        }   
    }
  return 0;
}


// this process wants to use coroutine_st instead of multi-thread when using FreeRTOS
#ifdef FREERTOS_SCHED 
#undef FREERTOS_SCHED
#undef start_process
#undef end_process
#include <coroutine_st.h>
#define start_process()           scrBegin
#define end_process()             scrFinish((void*)0)
#endif /* FREERTOS_SCHED */
#include <unistd.h>
//-----------------------------------------------------------------------------------------------
#if (GPDI_NUM <= 8)
#define SHIFT_COUNT               8
#else /* 16 channels */
#define SHIFT_COUNT               16
#endif /* 16 channels */

/**
 * \remarks implemented by coroutine
 * \verbatim
   Shift sequence: H (D7) -> G (D6) -> F (D5) -> E (D4)
                          -> D (D3) -> C (D2) -> B (D1) -> A (D0)
   \endverbatim
 */
void*
gpdi_ctrl (void* arg)
{
  start_process ();

  start_time = clock ();
  while ( ((clock_t) (clock () - start_time)) < GPDI_CTRL_INTERVAL) usleep(0);
  
  //load data
  GPDI_LOAD (0);
  GPDI_LOAD (1);
  GPDI_LOAD (0);
  
  int i;
  for (i = (SHIFT_COUNT - 1); i >= 0; i--)
    {
      //state is high
      if (GPDI_GET_STATE() > 0)
        {
          gpdi_status |= (0x0001 << i);
        }
      //state is low
      else
        {
          gpdi_status &= ~(0x0001 << i);
        }

      //shift data
      GPDI_CLK (0);
      GPDI_CLK (1);
    }
  
  end_process ();
}

/** @} */
/** @} */
