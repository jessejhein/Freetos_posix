/*
 * Copyright from pes.free.fr
 * Modify 2005 cheng chung yan <yan@amonics.com> for 8-bit system running in linlike8
 *
 */

// including, get data linking from others ===============================================
//	this appl. layer
#include <pin_define.h>
//	uart
#if (LINUX==1)
#include <fcntl.h>
#endif
//	uart
#if (AVR==1)
#include <linlike8/uart.h>
#endif
//	this module
#include "modbus.h"

// data =================================================================================

#if (LINUX==1)
struct termios saved_tty_parameters;			// old serial port setting (restored on close)
#endif

// methods ==============================================================================

#if (MODBUS==1)

/************************************************************************************
		Mb_test_crc : check the crc of a packet
*************************************************************************************
input :
-------
trame  : packet with is crc
n      : lenght of the packet without tht crc
                              ^^^^^^^
answer :
--------
1 = crc fealure
0 = crc ok
************************************************************************************/
int Mb_test_crc(byte trame[],int n)
{
	unsigned int crc,i,j,carry_flag,a;
	crc=0xffff;
	for (i=0;i<n;i++)
	{
		crc=crc^trame[i];
		for (j=0;j<8;j++)
		{
			a=crc;
			carry_flag=a&0x0001;
			crc=crc>>1;
			if (carry_flag==1)
				crc=crc^0xa001;
		}
	}
#if (LINUX==1)
   if (Mb_verbose)
      printf("test crc %0x %0x\n",(crc&255),(crc>>8));
#endif
	if ((trame[n+1]!=(crc>>8)) || (trame[n]!=(crc&255)))
      return 1;
   else
      return 0;
}

/************************************************************************************
		Mb_calcul_crc : compute the crc of a packet and put it at the end
*************************************************************************************
input :
-------
trame  : packet with is crc
n      : lenght of the packet without tht crc
                              ^^^^^^^
answer :
--------
crc
************************************************************************************/
int Mb_calcul_crc(byte trame[],int n)
{
	unsigned int crc,i,j,carry_flag,a;
	crc=0xffff;
	for (i=0;i<n;i++)
	{
		crc=crc^trame[i];
		for (j=0;j<8;j++)
		{
			a=crc;
			carry_flag=a&0x0001;
			crc=crc>>1;
			if (carry_flag==1)
				crc=crc^0xa001;
		}
	}
	trame[n+1]=crc>>8;
	trame[n]=crc&255;
	return crc;
}

#if (LINUX==1)
/************************************************************************************
		Mb_close_device : Close the device
*************************************************************************************
input :
-------
Mb_device : device descriptor

no output
************************************************************************************/
void Mb_close_device(int Mb_device)
{
  if (tcsetattr (Mb_device,TCSANOW,&saved_tty_parameters) < 0)
    perror("Can't restore terminal parameters ");
  close(Mb_device);
}

/************************************************************************************
		Mb_open_device : open the device
*************************************************************************************
input :
-------
Mbc_port   : string with the device to open (/dev/ttyS0, /dev/ttyS1,...)
Mbc_speed  : speed (baudrate)
Mbc_parity : 0=don't use parity, 1=use parity EVEN, -1 use parity ODD
Mbc_bit_l  : number of data bits : 7 or 8 	USE EVERY TIME 8 DATA BITS
Mbc_bit_s  : number of stop bits : 1 or 2    ^^^^^^^^^^^^^^^^^^^^^^^^^^

answer  :
---------
device descriptor
************************************************************************************/
//#include "../../data_access_gsm/gsm_master/core/pnl_sms.h"
int Mb_open_device(char Mbc_port[20], int Mbc_speed, int Mbc_parity, int Mbc_bit_l, int Mbc_bit_s)
{
	int fd;
	
  // open port
  //fd = open(Mbc_port,O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY) ;
  fd = open(Mbc_port,O_RDWR | O_NOCTTY | O_NDELAY) ;
  if(fd<0)
  {
    perror("Open device failure\n") ;
    exit(-1) ;
  }

//  fcntl(fd, F_SETFL, FASYNC);
	fcntl(fd, F_SETFL, FNDELAY);
	
  // save olds settings port
  if (tcgetattr (fd,&saved_tty_parameters) < 0)
  {
    perror("Can't get terminal parameters ");
    return -1 ;
  }

  // settings port
  /*bzero(&Mb_tio,sizeof(&Mb_tio));

  switch (Mbc_speed)
  {
     case 0:
        Mb_tio.c_cflag = B0;
        break;
     case 50:
        Mb_tio.c_cflag = B50;
        break;
     case 75:
        Mb_tio.c_cflag = B75;
        break;
     case 110:
        Mb_tio.c_cflag = B110;
        break;
     case 134:
        Mb_tio.c_cflag = B134;
        break;
     case 150:
        Mb_tio.c_cflag = B150;
        break;
     case 200:
        Mb_tio.c_cflag = B200;
        break;
     case 300:
        Mb_tio.c_cflag = B300;
        break;
     case 600:
        Mb_tio.c_cflag = B600;
        break;
     case 1200:
        Mb_tio.c_cflag = B1200;
        break;
     case 1800:
        Mb_tio.c_cflag = B1800;
        break;
     case 2400:
        Mb_tio.c_cflag = B2400;
        break;
     case 4800:
        Mb_tio.c_cflag = B4800;
        break;
     case 9600:
        Mb_tio.c_cflag = B9600;
        break;
     case 19200:
        Mb_tio.c_cflag = B19200;
        break;
     case 38400:
        Mb_tio.c_cflag = B38400;
        break;
     case 57600:
        Mb_tio.c_cflag = B57600;
        break;
     case 115200:
        Mb_tio.c_cflag = B115200;
        break;
     case 230400:
        Mb_tio.c_cflag = B230400;
        break;
     default:
        Mb_tio.c_cflag = B9600;
  }
  switch (Mbc_bit_l)
  {
     case 7:
        Mb_tio.c_cflag = Mb_tio.c_cflag | CS7;
        break;
     case 8:
     default:
        Mb_tio.c_cflag = Mb_tio.c_cflag | CS8;
        break;
  }
  switch (Mbc_parity)
  {
     case 1:
        Mb_tio.c_cflag = Mb_tio.c_cflag | PARENB;
//        Mb_tio.c_iflag = ICRNL;
        break;
     case -1:
        Mb_tio.c_cflag = Mb_tio.c_cflag | PARENB | PARODD;
//        Mb_tio.c_iflag = ICRNL;
        break;
     case 0:
     default:
//        Mb_tio.c_iflag = IGNPAR | ICRNL;
        Mb_tio.c_iflag = IGNPAR;
//        Mb_tio.c_iflag &= ~ICRNL;
        break;
  }
  Mb_tio.c_iflag &= ~ICRNL;
  Mb_tio.c_iflag &= ~(IXON | IXOFF | IXANY);

  if (Mbc_bit_s==2)
     Mb_tio.c_cflag = Mb_tio.c_cflag | CSTOPB;
     
  Mb_tio.c_cflag = Mb_tio.c_cflag | CLOCAL | CREAD;
	Mb_tio.c_cflag &= ~PARENB;
	Mb_tio.c_cflag &= ~CSTOPB;
	Mb_tio.c_cflag &= ~CSIZE;
	Mb_tio.c_cflag &= ~CRTSCTS;
  Mb_tio.c_oflag = 0;
  Mb_tio.c_lflag = 0; //ICANON;
	Mb_tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  Mb_tio.c_cc[VMIN]=1;
  Mb_tio.c_cc[VTIME]=0;*/

	cfsetispeed(&saved_tty_parameters, B19200);
	cfsetospeed(&saved_tty_parameters, B19200);
	
	saved_tty_parameters.c_cflag |= (CLOCAL | CREAD);
	saved_tty_parameters.c_cflag &= ~PARENB;
	saved_tty_parameters.c_cflag &= ~CSTOPB;
	saved_tty_parameters.c_cflag &= ~CSIZE;
	saved_tty_parameters.c_cflag |= CS8;
	saved_tty_parameters.c_cflag &= ~CRTSCTS;
	
	saved_tty_parameters.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	
	saved_tty_parameters.c_iflag &= ~(IXON | IXOFF | IXANY);
	
  /* activate the settings port */
  //if (tcsetattr(fd,TCSANOW,&Mb_tio) <0)
  if (tcsetattr(fd,TCSANOW,&saved_tty_parameters) <0)
  {
    perror("Can't set terminal parameters ");
    return -1 ;
  }
			
			{
unsigned char wr_buf[5];
wr_buf[0] = 'x';
wr_buf[1] = 'y';
wr_buf[2] = 'z';
//while (write(fd, wr_buf, 3)!=3);
			}
  
  /*

   if (Mb_verbose)
   {
      printf("setting ok:\n");
      printf("device        %s\n",Mbc_port);
      printf("speed         %d\n",Mbc_speed);
      printf("data bits     %d\n",Mbc_bit_l);
      printf("stop bits     %d\n",Mbc_bit_s);
      printf("parity        %d\n",Mbc_parity);
   }*/
   return fd ;

}
#endif	// (LINUX==1)

#if 0
/************************************************************************************
		Mb_rcv_print : print a character
This function can be use with slave or master to print a character when it receive one
*************************************************************************************
input :
-------
c : character

no output
************************************************************************************/
void Mb_rcv_print(unsigned char c)
{
   printf("-> receiving byte :0x%x %d \n",c,c);
}

/************************************************************************************
		Mb_snd_print : print a character
This function can be use with slave or master to print a character when it send one
*************************************************************************************
input :
-------
c : character

no output
************************************************************************************/
void Mb_snd_print(unsigned char c)
{
   printf("<- sending byte :0x%x %d \n",c,c);
}

char *Mb_version(void)
{
   return VERSION;
}
#endif	// #if 0

#endif	// #if (MODBUS==1)
