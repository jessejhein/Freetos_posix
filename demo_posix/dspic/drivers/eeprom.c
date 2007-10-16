/************************************************************************************************
 * File: 			eeprom.c
 * Description:		access to eeprom (1K)
 ***********************************************************************************************
 * SUMMARY:
 * 1)	5011 has a total of 1Kbyte of EEPROM located at address 0x7FFC00 to 0x7FFFFF.
 * 2)	Addressing in 5011 is 24-bit. The most significant 8-bit is referenced in the 
 * 		Table Page Register (TBLPAG). When concatenate with the least significant 16-bit
 * 		(table offset) in a general purpose register, the addresses in EEPROM can be 
 * 		referenced.
 * 3) 	The table page and table offset can be obtained from the build-in functions, for 
 * 		example:
 * 			TBLPAG = __builtin_tblpage(eeData);
 * 			offset = __builtin_tbloffset(eeData[5]);
 * 4)	The driver has a POSIX-compliant interface with open(), read(), write() and lseek().
 * 5)	readEEByte(), writeEEWord() and eraseEEWord() functions are implemented in assembly 
 * 		code in eeprom_asm.s
 * 6)   User should initialize the EEPROM locally in <eeprom.h>
 ***********************************************************************************************/

#include <define.h>

#if !defined(MPLAB_DSPIC30_PORT) 
#error MPLAB_DSPIC30_PORT not defined! 
#error This module can only be used with dsPIC30F devices.

#elif ( (NVM_MOD>0) & (NVM_SRC==NVM_SRC_ON_CHIP) )

#include <asm/system.h>
#include <fcntl.h>
#include <errno.h>

/****************************************************************************************
 * EEPROM Data and Pointer
 ****************************************************************************************/
#include <eeprom.h>				//Contains initialize data char _EEDATA(2) eeData[1024]
static unsigned int byte_pointer = 0;	//pointer (in bytes) from base address 0x7F, range = 0-1023
static int eeprom_io_flag;

/************************************************************************************************
 * Name:                int eeprom_open(int flags)
 * 
 * Input:               flags: accessing mode
 * 
 * Output:              0: ok
 * 
 * Function:            Initialize EEPROM
 ************************************************************************************************/
int eeprom_open(int flags){
    eeprom_io_flag = flags;
    return 0;
}

/************************************************************************************************
 * Name:                int eeprom_write(unsigned char* buf, int count)
 *
 * Input:               buf: data to write
 *                      count: number of bytes to write
 * 
 * Output:              number of bytes written
 *                      +-- $count: number of bytes written is returned
 *                      +-- <$count: attempts to write data more than the available bytes available
 *                      +-- 0: no data can be written
 *                      +-- -1: not opened for writing error (errno = EBADF)
 * 
 * Function:            copy $count bytes from $buf to eeprom
 * 
 ************************************************************************************************
 * 	At hardware level, erase operation is operated in 16-bit. To conform to POSIX 
 *  standard for writing in 8-bit, bytes that should not be erased are extracted first
 *  and then written with the user provided data.
 ************************************************************************************************
 *	Truth Table:
 *    +--------------+-------+----------+-------------------+-----------------------------+
 *    | byte_pointer | count |max_write |    word_offset    |  Remarks				      |
 *    +--------------+-------+----------+-------------------+-----------------------------+
 *    |    even      | even  |   n>>1   | byte_pointer >> 1 | all bytes valid             |
 *    |              +-------+----------+-------------------+-----------------------------+
 *    |              |  odd  |   n>>1   | byte_pointer >> 1 | last byte not used          |
 *    +--------------+-------+----------+-------------------+-----------------------------+
 *    |     odd      | even  | (n>>1)+1 | byte_pointer >> 1 | first & last bytes not used |
 *    |              +-------+----------+-------------------+-----------------------------+
 *    |              |  odd  | (n>>1)+1 | byte_pointer >> 1 | first byte not used         |
 *    +--------------+-------+----------+-------------------+-----------------------------+
 * 
 ************************************************************************************************/
int eeprom_write(unsigned char* buf, int count)
{
    //Perform Write if write operation is enabled
    if(eeprom_io_flag & O_RDWR || eeprom_io_flag & O_WRONLY)
    {
        char isOddAddr = byte_pointer%2;        //current address is odd
        char isOddByte = count%2;               //number of bytes to write is odd
        //===========================================================================
        unsigned int word_offset = byte_pointer>>1;     //div by 2 and round down
        int max_write;
        max_write = (isOddAddr == 0 && isOddByte == 0) ? (count>>1) : (count>>1)+1;
        //===========================================================================
        unsigned int word_data;         //Store word to be written
        int byte_wr = 0;        //number of bytes written, i.e buffer pointer
        int i = 0;
        //===========================================================================
        for(; i<max_write && word_offset<(EEPROM_SIZE/2); i++, word_offset++){
            if(i==0 && isOddAddr){
                //First byte not used
                //=============================================save first byte
                readEEByte( __builtin_tblpage(eeData), 
                            __builtin_tbloffset(eeData) + byte_pointer - 1,
                            &word_data);
                //============================================================
                word_data = ((unsigned int)buf[0] << 8) + (0xFF & word_data);
                byte_wr++;              //Update buffer pointer
                byte_pointer++;         //Update global pointer
            } else if(i==max_write-1 && ((isOddAddr && isOddByte==0)||(isOddAddr==0 && isOddByte))){
                //Last byte not used
                //=============================================save last byte
                readEEByte( __builtin_tblpage(eeData), 
                            __builtin_tbloffset(eeData) + byte_pointer + 1,
                            &word_data);
                //============================================================
                word_data = (word_data << 8) + buf[byte_wr];
                byte_wr++;              //Update buffer pointer
                byte_pointer++;         //Update global pointer
            } else{
                //Both bytes valid
                word_data = ((unsigned int)buf[byte_wr+1] << 8) + buf[byte_wr];
                byte_wr+=2;             //Update buffer pointer
                byte_pointer+=2;        //Update global pointer
            }
            //===============================================================================
            eraseEEWord(__builtin_tblpage(eeData), 
                        __builtin_tbloffset(eeData) + 2*word_offset);
            writeEEWord(__builtin_tblpage(eeData), 
                        __builtin_tbloffset(eeData) + 2*word_offset,
                        &word_data);
            //===============================================================================
        }
        return byte_wr;     //No. of byte written
    } 
    //Error, raise error flag
    else
    {
        errno = EBADF;  //io not opened for writing
        return -1;
    } 
}


/************************************************************************************************
 * Name:                int eeprom_read(unsigned char* buf, int count)
 *
 * Input:               buf: data to read to
 *                      count: number of bytes to read
 * 
 * Output:              number of bytes read
 *                      +-- $count: number of bytes read is returned
 *                      +-- <$count: attempts to read data more than the available bytes available
 *                      +-- 0: no data can be copied, either end of file already reached or $count = 0
 *                      +-- -1: not opened for reading error (errno = EBADF)
 * 
 * Function:            copy $count bytes to $buf from eeprom
 * 
 ***********************************************************************************************/
int eeprom_read(unsigned char* buf, int count)
{
    //Perform Read if read operation is enabled
    if(eeprom_io_flag & O_RDWR || eeprom_io_flag & O_RDONLY)
    {
        int i=0;
        for(; i<count && byte_pointer < EEPROM_SIZE; i++){
            readEEByte( __builtin_tblpage(eeData), 
                        __builtin_tbloffset(eeData) + byte_pointer, 
                        &buf[i]);
            byte_pointer++;     //Update global pointer
        }
        return i;   //read i bytes successful   
    }
    //Error, raise error flag
    else
    {
        errno = EBADF;  //io not opened for reading
        return -1;
    }     
}

/****************************************************************************************
 * EEPROM lseek
 * Function: change the current pointer from base address
 * 	+-- SEEK_SET:	BASE + offset
 * 	+-- SEEK_CUR:	unimplement
 * 	+-- SEEK_END:	unimplement
 * Return:
 * 	+-- eeprom_offset: current offset from base address (can be greater than 1024)
 ***********************************************************************************************/
int eeprom_lseek(int offset, int whence){
	byte_pointer = offset;
	return byte_pointer;
}

#endif

