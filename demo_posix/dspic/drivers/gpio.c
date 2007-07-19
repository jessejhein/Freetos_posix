/************************************************************************************************
 * File:            gpio.c
 * Description:     detect input keys and store in cirbuf
 ***********************************************************************************************
 * SUMMARY:
 ***********************************************************************************************/

#include <define.h>

#if (KB_MOD>0)
#include <fcntl.h>
#include <errno.h>
#include <asm/system.h>

/************************************************************************************************
 * Buffers for GPIO data
 ************************************************************************************************/
unsigned char gpio_buf[MAX_GPIO_BUF];
unsigned char gpio_wr = 0;   //write pointer of cir buf
unsigned char gpio_rd = 0;   //read pointer of cir buf

static unsigned char current_key[TOTAL_ROTARY_KEY];
static unsigned char previous_key[TOTAL_ROTARY_KEY];

static int gpio_io_flag;

/********************************************************************************************
 * Name:            int gpio_open(int flags)
 * 
 * Function:        get gpio port ready
 * 
 * Input:           flags: accessing mode
 * 
 * Output:          -1 if write mode is selected (errno = EROFS)
 *                  0 if ok
 * 
 * Precondition:    _TRIS should be set correctly for gpio
 * 
 *********************************************************************************************/
int gpio_open(int flags)
{
    if(flags & O_RDWR || flags & O_WRONLY){
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

/********************************************************************************************
 * Name:        int gpio_read(unsigned char *buf)
 * 
 * Function:    copy the received data to buf
 * 
 * Input:       buf: pointer to buffer for reading
 * 
 * Ouput:       1, indicating 1 byte has been read
 *              0, indicating no data is available
 *              -1: error, uart is not opened for read operation (errno = EBADF)
 * 
 *********************************************************************************************/
int gpio_read(unsigned char *buf)
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

/*******************************************************************************
 * ISR for Rotary Keys 
 *******************************************************************************/
void _ISR _CNInterrupt(void)
{
    int i, key_id, high;
    unsigned char next_data_pos;

    //Scan all rotary keys
    for(i=0, key_id=BASE_ROTARY_KEY; i<TOTAL_ROTARY_KEY; i++, key_id+=2)
    {
        //Check A status
        RKEY_STATE(key_id, high);
        current_key[i] = (high)? (current_key[i] | 0x10) : (current_key[i] & 0x01);
        //Check B status
        RKEY_STATE(key_id+1, high);
        current_key[i] = (high)? (current_key[i] | 0x01) : (current_key[i] & 0x10);
        
        switch (previous_key[i])
        {
            case 0x00:
                //Check which direction has turned
                switch (current_key[i])
                {
                    case 0x01:
                        next_data_pos = pre_wr_cir254buf( gpio_wr, gpio_rd, MAX_GPIO_BUF);
                        if (next_data_pos!=255) {
                            gpio_buf[gpio_wr] = (unsigned char) key_id+1;   //key turned down
                            gpio_wr = next_data_pos;
                        }
                        break;
                    case 0x10:
                        next_data_pos = pre_wr_cir254buf( gpio_wr, gpio_rd, MAX_GPIO_BUF);
                        if (next_data_pos!=255) {
                            gpio_buf[gpio_wr] = (unsigned char) key_id;     //key turned up
                            gpio_wr = next_data_pos;
                        }
                        break;
                }
                break;
        }
        previous_key[i] = current_key[i];
    }

#ifdef MPLAB_DSPIC33_PORT
    _CNIF = 0;
#else
    DISI_PROTECT(_CNIF = 0);
#endif    
}

#endif //end KB_MOD

