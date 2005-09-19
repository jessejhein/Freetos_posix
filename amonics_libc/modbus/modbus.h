/*
#ifndef __modbus__

#define __modbus__ 1
*/

#define	MODBUS			1

#if (MODBUS==1)

#if (LINUX==1)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif
//#if 0
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#endif	// #if 0
#include <unistd.h>
#if (LINUX==1)
#include <termios.h>
#endif
//#include <pthread.h>

#if 0
#define VERSION "0.0.4"

#endif	// #if 0
struct termios saved_tty_parameters;			/* old serial port setting (restored on close) */
struct termios Mb_tio;								/* new serail port setting */


int Mb_verbose;										/* print debug informations */
int Mb_status;											/* stat of the software : This number is free, it's use with function #07 */


int *Mbs_data;											/* slave modbus data */
#if 0
int Mbs_pid;											/* PID of the slave thread */
#endif	// #if 0

typedef unsigned char byte;						/* create byte type */

/* master structure */
typedef struct {
   int device;											/* modbus device (serial port: /dev/ttyS0 ...) */
   int slave; 											/* number of the slave to call*/
   int function; 										/* modbus function to emit*/
   int address;										/* slave address */
   int length;											/* data length */
   int timeout;										/* timeout in ms */
} Mbm_trame;

/*pointer functions */
void (*Mb_ptr_rcv_data) ();						/* run when receive a char in master or slave */
void (*Mb_ptr_snd_data) ();						/* run when send a char  in master or slave */
#if 0
void (*Mb_ptr_end_slve) ();						/* run when slave finish to send response trame */

#endif	// #if 0
/* master main function :
- trame informations
- data in
- data out
- pointer function called when master send a data on serial port (can be NULL if not use)
- pointer function called when master receive a data on serial port (can be NULL if not use)*/
int Mb_master(Mbm_trame, int [] , int [], void*, void*);

/* slave main function (start slave thread) :
- device
- slave number
- pointer function called when slave send a data on serial port (can be NULL if not use)
- pointer function called when slave receive a data on serial port (can be NULL if not use)
- pointer function called when slave finish to send data on serial port (can be NULL if not use)*/
void Mb_slave(int, int, void*, void*, void*);
void Mbs(void);

#if 0

void Mb_slave_stop(void);											/* stop slave thread */

#endif	// #if 0
/* commun functions */
int Mb_open_device(char [], int , int , int ,int );		/* open device and configure it */
void Mb_close_device();												/* close device and restore old parmeters */
int Mb_test_crc(unsigned char[] ,int );						/* check crc16 */
int Mb_calcul_crc(unsigned char[] ,int );						/* compute crc16 */

#if 0
void Mb_rcv_print(unsigned char);				/* print a char (can be call by master or slave with Mb_ptr_rcv_data)*/
void Mb_snd_print(unsigned char);				/* print a char (can be call by master or slave with Mb_ptr_rcv_data)*/
char *Mb_version(void);								/* return libmodbus version */
int Mbd_dial(int device, char *phonenumber, int timeout);
int Mbd_setup(int device, char *setup, int timeout);
int Mbd_hangup(int device, int timeout);
#endif	// #if 0

#endif	// #if (MODBUS==1)
