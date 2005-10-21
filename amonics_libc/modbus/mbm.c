/*
 * Copyright from pes.free.fr
 * Modify 2005 cheng chung yan <yan@amonics.com> for 8-bit system running in linlike8
 *
 */

// including, get data linking from others ===============================================
//	this appl. layer
#include <pin_define.h>
//	this module
#include "modbus.h"

// data ==================================================================================
#if (MODBUS==1)

#include <signal.h>

int Mb_device;				/* device tu use */
#if 0
int Mbm_Pid_Child;		/* PID child used to read the slave answer */
int Mbm_Pid_Sleep;		/* PID use to wait the end of the timeout */
#endif	// #if 0
byte *Mbm_result;			/* byte readed on the serial port : answer of the slave */
#if 0


/************************************************************************************
		Mbm_get_data : thread reading data on the serial port
*************************************************************************************
input :
-------
len	:	number of data to read;

no output
************************************************************************************/
void Mbm_get_data(int *len )
{
	int i;
	byte read_data;

	Mbm_Pid_Child=getpid();


		if (Mb_verbose)
			fprintf(stderr,"starting receiving data, total length : %d \n",*len);
	for(i=0;i<(*len);i++)
	{
		/* read data */
		read(Mb_device,&read_data,1);

		/* store data to the slave answer packet */
		Mbm_result[i]=read_data;
		
		/* call the pointer function if exist */
#if 0
		if(Mb_ptr_rcv_data!=NULL)
			(*Mb_ptr_rcv_data)(read_data);
#endif
		if (Mb_verbose)
			fprintf(stderr,"receiving byte :0x%x %d (%d)\n",read_data,read_data,Mbm_result[i]);
	}
	if (Mb_verbose)
		fprintf(stderr,"receiving data done\n");

	Mbm_Pid_Child=0;

}
#endif	// #if 0

unsigned char Csm_get_data_status;
// return
//	0 -- timeout
//	1 -- completed
//	-1 -- wait
void mb_timeout()
{
	Csm_get_data_status = 2;
}
	int i_Csm_get_data;
	byte read_data_Csm_get_data;
//	time_t t;
int Csm_get_data(int len, int timeout)
{
	switch (Csm_get_data_status) {
		case 0 : 
		#if (LINUX==1)
			if (Mb_verbose)
				fprintf(stderr,"in get data\n");
		#endif
		#if (LINUX==1)
			signal(SIGALRM, mb_timeout);
			alarm((int)timeout/1000);
		#endif
		//t = (time(NULL) + ((timeout * 2)/1000));
		#if (AVR==1)
		#endif
			Csm_get_data_status++;
			i_Csm_get_data = 0;
			break;
		case 1 : 
		
			while (read(Mb_device,&read_data_Csm_get_data,1)>0) {	// read data
				Mbm_result[i_Csm_get_data]=read_data_Csm_get_data;			// store data to the slave answer packet
#if (LINUX==1)
				if (Mb_verbose)
					fprintf(stderr,"receiving byte :0x%x %d (%d)\n",read_data_Csm_get_data,read_data_Csm_get_data,Mbm_result[i_Csm_get_data]);
#endif
				i_Csm_get_data++;
				if (i_Csm_get_data>=len) {
#if (LINUX==1)
					if (Mb_verbose)
						fprintf(stderr,"receiving data done\n");
#endif
					Csm_get_data_status = 0;				// go back for next time
					return(1);
				}
			}
			break;
		case 2 : 
			Csm_get_data_status = 0;						// go back for next time
	printf("timeout\n");
			return 0;	// timeout
			break;
	}
	return -1;
}


#if 0
/************************************************************************************
		Mbm_sleep : thread wait timeout
*************************************************************************************
input :
-------
timeout : duduration of the timeout in ms

no output
************************************************************************************/
void Mbm_sleep(int *timeout)
{
	Mbm_Pid_Sleep=getpid();
	if (Mb_verbose)
		fprintf(stderr,"sleeping %d ms\n",*timeout);

	usleep(*timeout*1000);

	Mbm_Pid_Sleep=0;
	if (Mb_verbose)
		fprintf(stderr,"Done sleeping %d ms\n",*timeout);

}

/************************************************************************************
		Mbm_send_and_get_result : send data, and wait the answer of the slave
*************************************************************************************
input :
-------
trame	  : packet to send
timeout	: duduration of the timeout in ms
long_emit : length of the packet to send
longueur  : length of the packet to read

answer :
--------
0			: timeout failure
1			: answer ok
************************************************************************************/
int Mbm_send_and_get_result(byte trame[], int timeout, int long_emit, int longueur)
{
	int i,stat1=-1,stat2=-1;

	pthread_t thread1,thread2;
	Mbm_result = (unsigned char *) malloc(longueur*sizeof(unsigned char));

	/* clean port */
	tcflush(Mb_device, TCIFLUSH);

	/* create 2 threads for read data and to wait end of timeout*/
	pthread_create(&thread2, NULL,(void*)&Mbm_sleep,&timeout);
	pthread_detach(thread2);
	pthread_create(&thread1, NULL,(void*)&Mbm_get_data,&longueur);
	pthread_detach(thread1);

	if (Mb_verbose)
		fprintf(stderr,"start writing \n");
	for(i=0;i<long_emit;i++)
	{
		/* send data */
		write(Mb_device,&trame[i],1);
		/* call pointer function if exist */
#if 0
		if(Mb_ptr_snd_data!=NULL)
			(*Mb_ptr_snd_data)(trame[i]);
#endif
			
	}

  if (Mb_verbose)
		fprintf(stderr,"write ok\n");

	do {

		if (Mbm_Pid_Child!=0)
			/* kill return 0 if the pid is running or -1 if the pid don't exist */
			stat1=0;
		else
			stat1=-1;

		if (Mbm_Pid_Sleep!=0)
			stat2=0;
		else
			stat2=-1;

		/* answer of the slave terminate or and of the timeout */
		if (stat1==-1 || stat2==-1) 
			break;
		usleep(timeout);

	} while(1); 
	if (Mb_verbose)
	{
		fprintf(stderr,"pid reading %d return %d\n",Mbm_Pid_Child,stat1);
		fprintf(stderr,"pid timeout %d return %d\n",Mbm_Pid_Sleep,stat2);
	}

	/* stop both childs */
	Mbm_Pid_Child=0;
	Mbm_Pid_Sleep=0;
	pthread_cancel(thread1);
	pthread_cancel(thread2);
	/* error : timeout fealure */
	if (stat1==0)
	{
		free(Mbm_result);
		return 0;
	}
	/* ok : store the answer packet in the data trame */
	for (i=0;i<=longueur;i++)
		trame[i]=Mbm_result[i];
	
	free(Mbm_result);
	return 1;
}

#endif	// #if 0

// return
//	0 -- timeout
//	1 -- completed
//	-1 -- wait
unsigned char ret_Csm_send_and_get_result_status = 0;
	int i_Csm_send_and_get_result;
	int ret_Csm_send_and_get_result;
int Csm_send_and_get_result(unsigned char trame[], int timeout, int long_emit, int longueur)
{
	switch (ret_Csm_send_and_get_result_status) {
		case 0 : 
	
	Mbm_result = trame;
	
#if (LINUX==1)
	if (Mb_verbose) {
		fprintf(stderr,"start writing with long_emit=%d\n", long_emit);
	}
#endif
	for(i_Csm_send_and_get_result=0;i_Csm_send_and_get_result<long_emit;i_Csm_send_and_get_result++)
	{
		// send data
//printf("trame %d\n", trame[i_Csm_send_and_get_result]);
		write(Mb_device,&trame[i_Csm_send_and_get_result],1);
		// call pointer function if exist
//		if(Mb_ptr_snd_data!=NULL)
//			(*Mb_ptr_snd_data)(trame[i]);
	}

#if (LINUX==1)
  if (Mb_verbose)
		fprintf(stderr,"write ok\n");
#endif

	ret_Csm_send_and_get_result_status++;

			break;
		case 1 :
			ret_Csm_send_and_get_result = Csm_get_data(longueur, timeout);
			if (ret_Csm_send_and_get_result!=-1)	ret_Csm_send_and_get_result_status++;
			break;
		case 2 :
		
	ret_Csm_send_and_get_result_status = 0;
	return ret_Csm_send_and_get_result;
	
			break;
	}
	
	return -1;

}

/************************************************************************************
					Mbm_master : comput and send a master packet
*************************************************************************************
input :
-------
Mb_trame	  : struct describing the packet to comput
						device		: device descriptor
						slave 		: slave number to call
						function 	: modbus function
						address		: address of the slave to read or write
						length		: lenght of data to send
data_in	  : data to send to the slave
data_out	  : data to read from the slave
timeout	  : timeout duration in ms
ptrfoncsnd : function to call when master send data (can be NULL if you don't whant to use it)
ptrfoncrcv : function to call when master receive data (can be NULL if you don't whant to use it)
*************************************************************************************
answer :
--------
1 : WAIT
0 : OK
-1 : unknow modbus function
-2 : CRC error in the slave answer
-3 : timeout error
-4 : answer come from an other slave
*************************************************************************************/
unsigned char Mb_master_status = 0;
int i_Mb_master,longueur_Mb_master,long_emit_Mb_master;
int slave_Mb_master;
int function, adresse, nbre;
byte trame[32];
int Mb_master(Mbm_trame Mbtrame,int data_in[], int data_out[],void *ptrfoncsnd, void *ptrfoncrcv)
{

	switch (Mb_master_status) {
		case 0 : 
	
		Mb_device=Mbtrame.device;
		slave_Mb_master=Mbtrame.slave;
		function=Mbtrame.function;
		adresse=Mbtrame.address;
		nbre=Mbtrame.length;
#if 0
		Mb_ptr_snd_data=ptrfoncsnd;
		Mb_ptr_rcv_data=ptrfoncrcv;
#endif
			
		switch (function)
		{
			case 0x03:
			case 0x04:
				/* read n byte */
				trame[0]=slave_Mb_master;							// slave address
				trame[1]=function;									// function code
				trame[2]=adresse>>8;								// reg. starting addr.
				trame[3]=adresse&0xFF;
				trame[4]=nbre>>8;									// how many reg. need to read
				trame[5]=nbre&0xFF;
				/* comput crc */
				Mb_calcul_crc(trame,6);
				/* comput length of the packet to send */
				long_emit_Mb_master=8;
				break;
			
			case 0x05: //write a single coil
				trame[0]=slave_Mb_master;
				trame[1]=function;
				trame[2]=adresse>>8;
				trame[3]=adresse&0xFF;
				trame[4]=data_in[0]>>8;
				trame[5]=data_in[0]&0xFF;
				/* comput crc */
				Mb_calcul_crc(trame,6);
				/* comput length of the packet to send */
				long_emit_Mb_master=8;
				break;
				
			case 0x06:
				/* write one byte */
				trame[0]=slave_Mb_master;
				trame[1]=function;
				trame[2]=adresse>>8;
				trame[3]=adresse&0xFF;
				trame[4]=data_in[0]>>8;
				trame[5]=data_in[0]&0xFF;
				/* comput crc */
				Mb_calcul_crc(trame,6);
				/* comput length of the packet to send */
				long_emit_Mb_master=8;
				break;
	
			case 0x07:
				/* read status */
				trame[0]=slave_Mb_master;
				trame[1]=function;
				/* comput crc */
				Mb_calcul_crc(trame,2);
				/* comput length of the packet to send */
				long_emit_Mb_master=4;
				break;
				
			case 0x08:
				/* line test */
				trame[0]=slave_Mb_master;
				trame[1]=0x08;
				trame[2]=0;
				trame[3]=0;
				trame[4]=0;
				trame[5]=0;
				Mb_calcul_crc(trame,6);
				/* comput length of the packet to send */
				long_emit_Mb_master=8;
				break;
				
			case 0x10:
				/* write n byte  */
				trame[0]=slave_Mb_master;
				trame[1]=0x10;
				trame[2]=adresse>>8;
				trame[3]=adresse&0xFF;
				trame[4]=nbre>>8;
				trame[5]=nbre&0xFF;
				trame[6]=nbre*2;
				for (i_Mb_master=0;i_Mb_master<nbre;i_Mb_master++)
				{
					trame[7+i_Mb_master*2]=data_in[i_Mb_master]>>8;
					trame[8+i_Mb_master*2]=data_in[i_Mb_master]&0xFF;
				}
				/* comput crc */
				Mb_calcul_crc(trame,7+nbre*2);
				/* comput length of the packet to send */
				long_emit_Mb_master=(nbre*2)+9;
				break;
			default:
				return -1;
		}
		if (Mb_verbose) 
		{
	#if (LINUX==1)
			fprintf(stderr,"send packet length %d\n",long_emit_Mb_master);
			for(i_Mb_master=0;i_Mb_master<long_emit_Mb_master;i_Mb_master++)
				fprintf(stderr,"send packet[%d] = %0x\n",i_Mb_master,trame[i_Mb_master]);
	#endif
		}
		
		/* comput length of the slave answer */
		switch (function)
		{
			case 0x03:
			case 0x04:
				longueur_Mb_master=5+(nbre*2);
				break;
			
			case 0x05:
			case 0x06:
			case 0x08:
			case 0x10:
			longueur_Mb_master=8;
				break;
	
			case 0x07:
			longueur_Mb_master=5;
				break;
	
			default:
				return -1;
				break;
		}
		Mb_master_status++;
		i_Mb_master = 0;
			break;
		case 1 :
		/* send packet & read answer of the slave
			answer is stored in trame[] */
			switch (Csm_send_and_get_result(trame,Mbtrame.timeout,long_emit_Mb_master,longueur_Mb_master)) {		
				case -1 : 						// wait
					break;
				case 0 : 						// return 0 as timeout
					i_Mb_master++;
					if (i_Mb_master>=4) return -3;	// timeout error, if timeout, re-send until 4 times
					break;
				case 1 : 						// completed
					Mb_master_status++;
					break;
			}
			break;
		case 2 :
	
	#if (LINUX==1)
	  	if (Mb_verbose)
		{
			fprintf(stderr,"answer :\n");
			for(i_Mb_master=0;i_Mb_master<longueur_Mb_master;i_Mb_master++)
				fprintf(stderr,"answer packet[%d] = %0x\n",i_Mb_master,trame[i_Mb_master]);
		}
	#endif
	
		if (trame[0]!=slave_Mb_master)
			return -4;	/* this is not the right slave */
	
		switch (function)
		{
			case 0x03:
			case 0x04:
				/* test received data */
				if (trame[1]!=0x03 && trame[1]!=0x04)
					return -2;
				if (Mb_test_crc(trame,3+(nbre*2)))
					return -2;
				/* data are ok */
				if (Mb_verbose)
					fprintf(stderr,"Reader data \n");
				for (i_Mb_master=0;i_Mb_master<nbre;i_Mb_master++)
				{
					data_out[i_Mb_master]=(trame[3+(i_Mb_master*2)]<<8)+trame[4+i_Mb_master*2];
//					if (Mb_verbose)
						fprintf(stderr,"data %d = %0x\n",i_Mb_master,data_out[i_Mb_master]);
				}
				break;
				
			case 0x05: //write a single coil
				/* test received data */
				if (trame[1]!=0x05)
					return -2;
				if (Mb_test_crc(trame,6))
					return -2;
				/* data are ok */
				if (Mb_verbose)
					fprintf(stderr,"data stored succesfull !\n");
				break;
			
			case 0x06:
				/* test received data */
				if (trame[1]!=0x06)
					return -2;
				if (Mb_test_crc(trame,6))
					return -2;
				/* data are ok */
				if (Mb_verbose)
					fprintf(stderr,"data stored succesfull !\n");
				break;
	
			case 0x07:
				/* test received data */
				if (trame[1]!=0x07)
					return -2;
				if (Mb_test_crc(trame,3))
					return -2;
				/* data are ok */
				data_out[0]=trame[2];	/* store status in data_out[0] */
				if (Mb_verbose)
					fprintf(stderr,"data  = %0x\n",data_out[0]);
				break;
	
			case 0x08:
				/* test received data */
				if (trame[1]!=0x08)
					return -2;
				if (Mb_test_crc(trame,6))
					return -2;
				/* data are ok */
				if (Mb_verbose)
					fprintf(stderr,"Loopback test ok \n");
				break;
	
			case 0x10:
				/* test received data */
				if (trame[1]!=0x10)
					return -2;
				if (Mb_test_crc(trame,6))
					return -2;
				/* data are ok */
				if (Mb_verbose)
					fprintf(stderr,"%d setpoint stored succesfull\n",(trame[4]<<8)+trame[5]);
				break;
	
			default:
				return -1;
				break;
		}
		Mb_master_status = 0;
		return 0;
			break;
	}
	return 1;
}

#endif	// #if (MODBUS==1)
