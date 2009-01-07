#ifndef _FCNTL_H
# error "Never use <bits/fcntl.h> directly; include <fcntl.h> instead."
#endif

/* open/fcntl - O_SYNC is only implemented on blocks devices and on files
   located on an ext2 file system */
//#define O_RDWR           02


/************************************************************************************************
 * Definition of flags in 
 *      int open(const char *pathname, int flags)
 * 
 * Examples:
 *      fd_eeprom = open(EEPROM, O_RDWR | O_NONBLOCK);
 ************************************************************************************************/
//Compulsory flags, choose one of following
#define O_RDONLY        0x00        //Bit 0 indicates R/W
#define O_WRONLY        0x01
#define O_RDWR          0x02        //Bit 1 indicates R+W
//Optional flags
#define O_NONBLOCK      0x04        //Bit 2 indicates Blocking/Non-blocking IO
