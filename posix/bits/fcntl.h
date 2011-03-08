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
#define O_RDONLY        0x0000        //Bit 0 indicates R/W
#define O_WRONLY        0x0001
#define O_RDWR          0x0002        //Bit 1 indicates R+W

/** The file is opened in append mode. */
#define O_APPEND        0x0008        

/** If the file does not exist it will be created. */
#define O_CREAT         0x0100
/** When used with O_CREAT, if the file already exists it is an error and the open() will fail */
#define O_EXCL          0x0200

/** When possible, the file is opened in non-blocking mode. */
#define O_NONBLOCK      0x4000        
