/*
 * flash_eeprom.c
 */

/******************************************************************************
 * This driver serve as an interface between POSIX and Microchip RTSP APIs
 ******************************************************************************/

#include <define.h>
#include <flash_eeprom.h>
#include <rtsp.h>
#include <fcntl.h>
#include <errno.h>

#if( (NVM_MOD>0) & (NVM_SRC==NVM_SRC_FLASH) )

/************************************************************************************************
 * Local Variables
 ************************************************************************************************/
static int flash_eeprom_io_flag;
static unsigned int flash_eeprom_pointer = 0;
static unsigned int flash_eeprom_busy = 0;
static unsigned char image_eeprom[FLASH_EEPROM_SIZE];

/************************************************************************************************
 * Function:    int flash_eeprom_open(int flags)
 * 
 * Description: Initialize EEPROM by Flash
 * 
 * Input:       flags: accessing mode
 * 
 * Output:      0: eeprom opened
 ************************************************************************************************/
int flash_eeprom_open(int flags){
    flash_eeprom_io_flag = flags;
    return 0;
}

/************************************************************************************************
 * Function:    int flash_eeprom_write(unsigned char* buf, int count)
 * 
 * Description: write $count bytes to eeprom
 * 
 * Input:       *buf: pointer of data to write to eeprom
 *              count: number of bytes to be written
 * 
 * Output:      -1: eeprom is busy (errno = EAGAIN)
 *                  eeprom is not opened for writing (errno = EBADF)
 *              >0: number of bytes written 
 ************************************************************************************************/
int flash_eeprom_write(unsigned char* buf, int count){

    //Perform Write if write operation is enabled    
    if(flash_eeprom_io_flag & O_RDWR || flash_eeprom_io_flag & O_WRONLY)
    {
        if(flash_eeprom_busy == 1){
            errno = EAGAIN;
            return -1;
        }
        else{
            flash_eeprom_busy = 1;
            
            unsigned int nvmAdru = __builtin_tblpage(&flash_eeprom_data[0]);
            unsigned int nvmAdr  = __builtin_tbloffset(&flash_eeprom_data[0]);
#ifdef MPLAB_DSPIC33_PORT
            nvmAdr = nvmAdr & 0xFC00; // Get the Flash Page Aligned address
#endif    
            
            flashPageRead(nvmAdru, nvmAdr, (int*)image_eeprom);

            int i;           
            for(i=0; i<count && flash_eeprom_pointer<FLASH_EEPROM_SIZE ; i++, flash_eeprom_pointer++){
                image_eeprom[flash_eeprom_pointer] = buf[i];
            }
            
            flashPageErase(nvmAdru, nvmAdr);

            flashPageWrite(nvmAdru, nvmAdr, (int*)image_eeprom);
    
            flash_eeprom_busy = 0;
        
            return i;
        }
    }
    //Error, raise error flag
    else
    {
        errno = EBADF;  //io not opened for writing
        return -1;
    }     
}

/************************************************************************************************
 * Function:    int flash_eeprom_read(unsigned char* buf, int count)
 * 
 * Description: read $count bytes from eeprom
 * 
 * Input:       *buf: pointer of data to read from eeprom
 *              count: number of bytes to be read
 * 
 * Output:      -1: eeprom is busy (errno = EAGAIN)
 *                  eeprom is not opened for reading (errno = EBADF)
 *              >0: number of bytes read 
 ************************************************************************************************/
int flash_eeprom_read(unsigned char* buf, int count){

    //Perform Read if read operation is enabled
    if(flash_eeprom_io_flag & O_RDWR || flash_eeprom_io_flag & O_RDONLY)
    {
        if(flash_eeprom_busy == 1){
            errno = EAGAIN;
            return -1;
        }
        else{
            flash_eeprom_busy = 1;

            unsigned int nvmAdru = __builtin_tblpage(&flash_eeprom_data[0]);
            unsigned int nvmAdr  = __builtin_tbloffset(&flash_eeprom_data[0]);
#ifdef MPLAB_DSPIC33_PORT
            nvmAdr = nvmAdr & 0xFC00; // Get the Flash Page Aligned address
#endif    
            flashPageRead(nvmAdru, nvmAdr, (int*)image_eeprom);
            
            int i;
            for(i=0; i<count && flash_eeprom_pointer<FLASH_EEPROM_SIZE; i++, flash_eeprom_pointer++){
                buf[i] = image_eeprom[flash_eeprom_pointer];
            }
            
            flash_eeprom_busy = 0;   
            return i;
        }        
    }
    //Error, raise error flag
    else
    {
        errno = EBADF;  //io not opened for writing
        return -1;
    }        
}

/************************************************************************************************
 * Function:    flash_eeprom_lseek(int offset, int whence)
 * 
 * Description: change the current pointer from base address
 * 
 * Input:       offset: offset from location determined by whence
 *              whence:
 *                  +-- SEEK_SET:   BASE (0) + offset
 *                  +-- SEEK_CUR:   unimplement
 *                  +-- SEEK_END:   unimplement
 * 
 * Output:      current offset from base address (can be greater than FLASH_EEPROM_SIZE)
 *              -1 when another read/write is in progress
 ************************************************************************************************/
int flash_eeprom_lseek(int offset, int whence){
    if(flash_eeprom_busy > 0){
        errno = EAGAIN;
        return -1;
    }

    flash_eeprom_pointer = offset;
    return flash_eeprom_pointer;
}

#endif
