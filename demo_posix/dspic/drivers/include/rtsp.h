/*
 * RTSP.h
 */

/***************************************************************************************
 * Concept for RTSP (Real Time Self Programming)
 * 
 * Flash must be ERASED in step of PAGE
 * Flash must be WRITTEN in step of ROW
 * 
 * For dsPic33: 
 *   Each PAGE has 8 ROWS of flash 
 *   Each ROW corresponds to 64 instructions or 192 bytes (high byte of upper word is phathom)
 * For dsPic30: 
 *   Each PAGE has 1 ROW of flash 
 *   Each ROW corresponds to 32 instructions or 96 bytes (high byte of upper word is phathom)
 **************************************************************************************/
 
#ifndef __RTSP_H__
#define __RTSP_H__ 

/************************************************************************
 * ==Interfaces==
 ************************************************************************/

/************************************************************************
 * ===define.h===
 * 
 * Description:     a configuration file which contains the compile time configuration
 * 
 * Location:        which place in system directory
 * 
 * Content:         see below "Compile Time Configurations"
 *  
 ************************************************************************/

/************************************************************************
 * ==Compile Time Configurations==
 ***********************************************************************/
/************************************************************************
 * ===inside define.h===                                                
 */ 
#if 0
#include <p33FJ128GP306.h>
//or
#include <p30f5011.h>
#endif
 /***********************************************************************/

/************************************************************************
 * ==APIs==
 ************************************************************************/

/************************************************************************
 * ===API Usage Examples===
 */ 
#if 0

void foo(int* buf, int count){
    int image_buf[NVM_PAGE_SIZE];
    unsigned int nvmAdru = __builtin_tblpage(&nvm_data[0]);
    unsigned int nvmAdr  = __builtin_tbloffset(&nvm_data[0]);
#ifdef MPLAB_DSPIC33_PORT
    nvmAdr = nvmAdr & 0xFC00; // Get the Flash Page Aligned address
#endif    
    
    flashPageRead(nvmAdru, nvmAdr, (int*)image_buf);
        
    for(i=0; i<count && nvm_pointer<NVM_SIZE ; i++, nvm_pointer++){
        image_buf[nvm_pointer] = buf[i];
    }
        
    flashPageErase(nvmAdru, nvmAdr);
        
    flashPageWrite(nvmAdru, nvmAdr, (int*)image_buf);
}

#endif
 /************************************************************************/


/***************************************************************************************
 * Function:    int flashPageErase(unsigned int nvmAdru, unsigned int nvmAdr);
 * 
 * Description: erase a page of flash specified by nvmAdru and nvmAdr
 * 
 * Input:       nvmAdru:    Selects the upper 8bits of the location to erase flash
 *                          nvmAdru = __builtin_tblpage(&array[0])
 *              nvmAdr:     Selects the location to erase in flash
 *                          It must be aligned to a page boundary
 *                          nvmAdr = __builtin_tbloffset(&array[0])
 * 
 * Output:      -1 if it is not successful
 *               0, if successful
 **************************************************************************************/
extern int flashPageErase(unsigned int nvmAdru, unsigned int nvmAdr);

/***************************************************************************************
 * Function:    int flashPageRead(unsigned int nvmAdru, unsigned int nvmAdr, int *buf);
 * 
 * Description: erase a page of flash specified by nvmAdru and nvmAdr
 * 
 * Input:       nvmAdru:    Selects the upper 8bits of the location to read flash
 *                          nvmAdru = __builtin_tblpage(&array[0])
 *              nvmAdr:     Selects the location to read in flash
 *                          It must be aligned to a page boundary
 *                          nvmAdr = __builtin_tbloffset(&array[0])
 *              buf:        destination buffer
 * 
 * Output:      -1 if it is not successful
 *               0, if successful
 **************************************************************************************/
extern int flashPageRead(unsigned int nvmAdru, unsigned int nvmAdr, int *buf);

/***************************************************************************************
 * Function:    int flashPageWrite(unsigned int nvmAdru, unsigned int nvmAdr, int *buf);
 * 
 * Description: erase a page of flash specified by nvmAdru and nvmAdr
 * 
 * Input:       nvmAdru:    Selects the upper 8bits of the location to write flash
 *                          nvmAdru = __builtin_tblpage(&array[0])
 *              nvmAdr:     Selects the location to write in flash
 *                          It must be aligned to a page boundary
 *                          nvmAdr = __builtin_tbloffset(&array[0])
 *              buf:        source buffer
 * 
 * Output:      -1 if it is not successful
 *               0, if successful
 **************************************************************************************/
extern int flashPageWrite(unsigned int nvmAdru, unsigned int nvmAdr, int *buf);

#endif

