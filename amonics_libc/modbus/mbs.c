/*
 * Copyright from pes.free.fr
 * Modify 2005 cheng chung yan <yan@amonics.com> for 8-bit system running in linlike8
 *
 */

// including, get data linking from others ===============================================
//	this appl. layer
#include <pin_define.h>
//	uart
#if (AVR==1)
#include <linlike8/uart.h>
#endif
//	big, lttle endian library
#include <netinet/in.h>
//	this module
#include "modbus.h"

#if (MODBUS==1)

unsigned char Mbs_slave;				/* slave number */
#if 0
pthread_t Mbs_thread;					/* slave thread */
#endif	// #if 0
int Mbs_device;							/* slave device */

/***********************************************************************************
   	   Mbs_read : read one data and call pointer function if !=NULL
************************************************************************************
input :
-------
unsigned char c : pointer of the char         

no output
***********************************************************************************/
void Mbs_read(unsigned char *c)
{
   read(Mbs_device,c,1);
//   if(Mb_ptr_rcv_data!=NULL)
//      (*Mb_ptr_rcv_data)(*c);
}

/***********************************************************************************
   	   Mbs_write: write packet and call pointer function if !=NULL
************************************************************************************
input :
-------
trame 	: packet to send
longeur	: length of the packet

no output
***********************************************************************************/
/*void Mbs_write(unsigned char trame[256], int longeur)
{
   int i;
   for(i=0;i<longeur;i++)
   {
      write(Mbs_device,&trame[i],1);
//      if(Mb_ptr_snd_data!=NULL)
//         (*Mb_ptr_snd_data)(trame[i]);
   }
}*/

/**********************************************************************************
   	   Mbs : Main slave function 
   	   interpret modbus cmds
***********************************************************************************
input not used
no output
**********************************************************************************/
#if (AVR==1)
struct WR_COMPL_VAR wcv_hc;
#endif
//void Mbs(void *ptr)
unsigned char Mbs_status;
	unsigned char Mbs_fonction,Mbs_c,Mbs_c1,Mbs_c2,Mbs_trame[73];	// 73 is application depend, so should define in application side
   int Mbs_adresse,Mbs_longueur,Mbs_local_data,Mbs_i;
//   int Mbs_data_to_write[20];
void Mbs(void)
{
   
//	do {
	switch (Mbs_status) {
		case 0 : 
		
   if (read(Mbs_device,&Mbs_c,1)>0) {									// read slave addr
		if (Mbs_c==Mbs_slave) {
#if (LINUX==1)
	         if (Mb_verbose) fprintf(stderr,"Master call me !\n");
#endif
			Mbs_status++;
		}
   }
			break;
		case 1 : 
			//Mbs_read(&Mbs_fonction);
   if (read(Mbs_device,&Mbs_fonction,1)>0) {							// rd function code
#if (LINUX==1)
			if (Mb_verbose) fprintf(stderr,"function 0x%x \n",Mbs_c);
#endif
	        Mbs_trame[0]=Mbs_slave;
	        Mbs_trame[1]=Mbs_fonction;
			switch (Mbs_fonction)
	   		{
	      		case 0x03:
	      		case 0x04:
					Mbs_status = 2;
    	     		break;
      		case 0x06:
					Mbs_status = 9;
    	     		break;
      		case 0x07:
					Mbs_status = 15;
    	     		break;
				case 0x08:
					//Mbs_status = 18;
    	     		break;
      		case 0x10:
					Mbs_status = 21;
    	     		break;
	      		default :
					Mbs_status = 200;
	   		}
   }
			break;
               /*********************************************************************/
               // 0x03 funciton
		case 2 : 
         		/* read n byte */
               /* get adress */
//            Mbs_read(&Mbs_c1);
   if (read(Mbs_device,&Mbs_c1,1)>0) {									// for 0x03 function, rd Start addr.
			Mbs_status++;
   }
			break;
		case 3 : 
    //           Mbs_read(&Mbs_c2);
   if (read(Mbs_device,&Mbs_c2,1)>0) {
               Mbs_adresse=(Mbs_c1<<8)+Mbs_c2;
               Mbs_trame[2]=Mbs_c1;
               Mbs_trame[3]=Mbs_c2;
#if (LINUX==1)
               if (Mb_verbose) fprintf(stderr,"adress %d (%x %x) \n",Mbs_adresse,Mbs_c1,Mbs_c2);
#endif
			Mbs_status++;
   }
			break;
		case 4 : 
               /* get length */
//               Mbs_read(&Mbs_c1);
   if (read(Mbs_device,&Mbs_c1,1)>0) {									// for 0x03 function, rd no. of pt.
			Mbs_status++;
   }
			break;
		case 5 : 
//               Mbs_read(&Mbs_c2);
   if (read(Mbs_device,&Mbs_c2,1)>0) {
			Mbs_status++;
               Mbs_longueur=(Mbs_c1<<8)+Mbs_c2;
#if (LINUX==1)
               if (Mb_verbose) fprintf(stderr,"lenght(Mbs_longueur) %d \n",Mbs_longueur);
#endif
               Mbs_trame[4]=Mbs_c1;
               Mbs_trame[5]=Mbs_c2;
   }
			break;
		case 6 : 
               /* get crc16 */
    //           Mbs_read(&Mbs_c1);
   if (read(Mbs_device,&Mbs_c1,1)>0) {									// rd CRC
			Mbs_status++;
   }
			break;
		case 7 : 
//               Mbs_read(&Mbs_c2);
   if (read(Mbs_device,&Mbs_c2,1)>0) {
               Mbs_trame[6]=Mbs_c1;
               Mbs_trame[7]=Mbs_c2;
#if (LINUX==1)
               if (Mb_verbose) fprintf(stderr,"crc  %x%x \n",Mbs_c1,Mbs_c2);
#endif

               /* check crc16 */
               if (Mb_test_crc(Mbs_trame,6))
               {
#if (LINUX==1)
                  if (Mb_verbose) fprintf(stderr,"crc error\n");
#endif
                  //if(Mb_ptr_end_slve!=NULL)
                  //   (*Mb_ptr_end_slve)(-1,0,0);
                  break;
               }
#if (LINUX==1)
               if (Mb_verbose) 
                  for(Mbs_i=0;Mbs_i<=7;Mbs_i++)
                     fprintf(stderr,"sended packet[%d] = %0x\n",Mbs_i,Mbs_trame[Mbs_i]);
#endif

              // comput answer packet
				// appl. spec. code
               Mbs_trame[2]=Mbs_longueur*2;
               for (Mbs_i=0;Mbs_i<(Mbs_longueur/2);Mbs_i++) {
					unsigned long pulse_val = htonl(pulse_cnt[/*Mbs_adresse-67+*/Mbs_i]);		// -67 is for this application only, later change to general call
//printf("sended pulse_cnt[] = %lx\n",pulse_cnt[Mbs_i]);
					__u16* p_pulse_val = (__u16*) &pulse_val;										// in compiler option, add -fno-strict-aliasing
					Mbs_trame[3+Mbs_i*4]=(unsigned char) ((*p_pulse_val)&0xff);
					Mbs_trame[4+Mbs_i*4]=(unsigned char) ((*p_pulse_val)>>8);
					p_pulse_val++;
					Mbs_trame[5+Mbs_i*4]=(unsigned char) ((*p_pulse_val)&0xff);
					Mbs_trame[6+Mbs_i*4]=(unsigned char) ((*p_pulse_val)>>8);
#if (LINUX==1)
                  //if (Mb_verbose) 
                  //   fprintf(stderr,"Mbs_data[%d] = %0x\n",Mbs_adresse+Mbs_i,Mbs_data[Mbs_adresse+Mbs_i]);
#endif
               }

               Mb_calcul_crc(Mbs_trame,(Mbs_longueur*2)+3);

#if (LINUX==1)
#if 0
               if (Mb_verbose)
               {
                  fprintf(stderr,"answer :\n");
                  for(Mbs_i=0;Mbs_i<Mbs_longueur*2+5;Mbs_i++)
                     fprintf(stderr,"answer packet[%d] = %0x\n",Mbs_i,Mbs_trame[Mbs_i]);
               }
#endif
#endif
			Mbs_status++;
   }
			break;
		case 8 : 
            //Mbs_write(Mbs_trame,(Mbs_longueur*2)+5);
			{
			int wr_len = (Mbs_longueur*2)+5;
			if (write(Mbs_device, Mbs_trame, wr_len			// tx. out
				#if (AVR==1)
				, &wcv_hc
				#endif
				)==wr_len)
					Mbs_status = 200;
			}
			break;
               /*********************************************************************/
               // 0x06 funciton
		case 9 : 
         		/* write on byte */
               /* get adress */
//               Mbs_read(&Mbs_c1);
   if (read(Mbs_device,&Mbs_c1,1)>0) {
			Mbs_status++;
   }
			break;
		case 10 : 
//               Mbs_read(&Mbs_c2);
   if (read(Mbs_device,&Mbs_c2,1)>0) {
			Mbs_status++;
               Mbs_adresse=(Mbs_c1<<8)+Mbs_c2;
               Mbs_trame[2]=Mbs_c1;
               Mbs_trame[3]=Mbs_c2;
#if (LINUX==1)
               if (Mb_verbose) fprintf(stderr,"adress %d (%x %x) \n",Mbs_adresse,Mbs_c1,Mbs_c2);
#endif
   }
			break;
		case 11 : 
               /* get data */
//               Mbs_read(&Mbs_c1);
   if (read(Mbs_device,&Mbs_c1,1)>0) {
			Mbs_status++;
   }
			break;
		case 12 : 
//               Mbs_read(&Mbs_c2);
   if (read(Mbs_device,&Mbs_c2,1)>0) {
			Mbs_status++;
               Mbs_local_data=(Mbs_c1<<8)+Mbs_c2;
#if (LINUX==1)
               if (Mb_verbose) fprintf(stderr,"Mbs_local_data %d \n",Mbs_local_data);
#endif
               Mbs_trame[4]=Mbs_c1;
               Mbs_trame[5]=Mbs_c2;
   }
			break;
		case 13 : 
               /* get crc16 */
//               Mbs_read(&Mbs_c1);
   if (read(Mbs_device,&Mbs_c1,1)>0) {
			Mbs_status++;
   }
			break;
		case 14 : 
//               Mbs_read(&Mbs_c2);
   if (read(Mbs_device,&Mbs_c2,1)>0) {
			Mbs_status++;
               Mbs_trame[6]=Mbs_c1;
               Mbs_trame[7]=Mbs_c2;
#if (LINUX==1)
               if (Mb_verbose) fprintf(stderr,"crc  %x%x \n",Mbs_c1,Mbs_c2);
#endif
               /* check crc16 */
               if (Mb_test_crc(Mbs_trame,6))
               {
#if (LINUX==1)
                  if (Mb_verbose) fprintf(stderr,"crc error\n");
#endif
                  //if(Mb_ptr_end_slve!=NULL)
                  //   (*Mb_ptr_end_slve)(-1,0,0);
                  break;
               }
#if (LINUX==1)
               if (Mb_verbose) 
                  for(Mbs_i=0;Mbs_i<=7;Mbs_i++)
                     fprintf(stderr,"sended packet[%d] = %0x\n",Mbs_i,Mbs_trame[Mbs_i]);
#endif
               
               /* store data */
               Mbs_data[Mbs_adresse]=Mbs_local_data;		// should not work, pls chk
#if (LINUX==1)
               if (Mb_verbose) 
                  fprintf(stderr,"Mbs_local_data %d stored at : %d %x\n",Mbs_local_data,Mbs_adresse,Mbs_adresse);
#endif
   }               
			break;
		case 15 : 
               /* answer Mbs_trame is the same ;-)*/
            //Mbs_write(Mbs_trame,8);
			if (write(Mbs_device, Mbs_trame, 8			// tx. out
				#if (AVR==1)
				, &wcv_hc
				#endif
				)==8)
					Mbs_status = 200;
			break;
            /*********************************************************************/
		case 16 : 
        		/* read status */
               /* get crc16 */
//               Mbs_read(&Mbs_c2);
   if (read(Mbs_device,&Mbs_c2,1)>0) {
			Mbs_status++;
   }
			break;
		case 17 : 
    //           Mbs_read(&Mbs_c2);
   if (read(Mbs_device,&Mbs_c2,1)>0) {
			Mbs_status++;
               Mbs_trame[2]=Mbs_c1;
               Mbs_trame[3]=Mbs_c2;
#if (LINUX==1)
               if (Mb_verbose) fprintf(stderr,"crc  %x%x \n",Mbs_c1,Mbs_c2);
#endif
               /* check crc16 */
               if (Mb_test_crc(Mbs_trame,2))
               {
#if (LINUX==1)
                  if (Mb_verbose) fprintf(stderr,"crc error\n");
#endif
                  //if(Mb_ptr_end_slve!=NULL)
                  //   (*Mb_ptr_end_slve)(-1,0,0);
                  break;
               }
#if (LINUX==1)
               if (Mb_verbose) 
                  for(Mbs_i=0;Mbs_i<=3;Mbs_i++)
                     fprintf(stderr,"sended packet[%d] = %0x\n",Mbs_i,Mbs_trame[Mbs_i]);
#endif

               /* comput answer packet */
               Mbs_trame[2]=Mb_status;

               Mb_calcul_crc(Mbs_trame,3);

#if (LINUX==1)
               if (Mb_verbose)
               {
                  fprintf(stderr,"answer :\n");
                  for(Mbs_i=0;Mbs_i<=4;Mbs_i++)
                     fprintf(stderr,"answer packet[%d] = %0x\n",Mbs_i,Mbs_trame[Mbs_i]);
               }
#endif
   }

			break;
		case 18 : 
            //Mbs_write(Mbs_trame,5);
			if (write(Mbs_device, Mbs_trame, 5			// tx. out
				#if (AVR==1)
				, &wcv_hc
				#endif
				)==5)
					Mbs_status = 200;
			break;
            /*********************************************************************/
/*		case 19 : 
			Mbs_i=0;
			Mbs_status++;
			break;
		case 20 : 
            Mbs_read(&Mbs_c);
			if (Mbs_c!=0) Mbs_status++;
			else {
				
				Mbs_trame[Mbs_i+2]=Mbs_c;
			}	
			Mbs_i++;
			if (Mbs_i<4) Mbs_status++;
		case 21 : 
                  
			Mbs_status++;
		
		
         		// line test
               for (Mbs_i=0;Mbs_i<4;Mbs_i++)
               {
                  Mbs_read(&Mbs_c);
                  if (Mbs_c!=0) break;
                  Mbs_trame[Mbs_i+2]=Mbs_c;
               }
               
               if (Mbs_c!=0) break;

               // get crc16
               Mbs_read(&Mbs_c1);
               Mbs_read(&Mbs_c2);
               Mbs_trame[6]=Mbs_c1;
               Mbs_trame[7]=Mbs_c2;
               if (Mb_verbose) fprintf(stderr,"crc  %x%x \n",Mbs_c1,Mbs_c2);
               // check crc16
               if (Mb_test_crc(Mbs_trame,6))
               {
                  if (Mb_verbose) fprintf(stderr,"crc error\n");
                  //if(Mb_ptr_end_slve!=NULL)
                  //   (*Mb_ptr_end_slve)(-1,0,0);
                  break;
               }
               if (Mb_verbose) 
                  for(Mbs_i=0;Mbs_i<=7;Mbs_i++)
                     fprintf(stderr,"sended packet[%d] = %0x\n",Mbs_i,Mbs_trame[Mbs_i]);

               // return the same packet, it's cool
               Mbs_write(Mbs_trame,8);
               //if(Mb_ptr_end_slve!=NULL)
               //   (*Mb_ptr_end_slve)(Mbs_fonction,0,0);
			break;*/
	          /*********************************************************************/
               // 0x10 funciton
		case 21 : 
         		/* write n byte  */
               /* get adress */
//               Mbs_read(&Mbs_c1);
   if (read(Mbs_device,&Mbs_c1,1)>0) {									// start addr hi
			Mbs_status++;
   }
			break;
		case 22 : 
    //           Mbs_read(&Mbs_c2);
   if (read(Mbs_device,&Mbs_c2,1)>0) {									// start addr lo
			Mbs_status++;
               Mbs_adresse=(Mbs_c1<<8)+Mbs_c2;
               Mbs_trame[2]=Mbs_c1;
               Mbs_trame[3]=Mbs_c2;
#if (LINUX==1)
               if (Mb_verbose) fprintf(stderr,"adress %d 0x%x%x \n",Mbs_adresse,Mbs_c1,Mbs_c2);
#endif
   }
			break;
		case 23 : 
               /* get length */
//               Mbs_read(&Mbs_c1);
   if (read(Mbs_device,&Mbs_c1,1)>0) {									// no. of reg. hi 
			Mbs_status++;
   }
			break;
		case 24 : 
//               Mbs_read(&Mbs_c2);
   if (read(Mbs_device,&Mbs_c2,1)>0) {									// no. of reg. lo
			Mbs_status++;
               Mbs_longueur=(Mbs_c1<<8)+Mbs_c2;
#if (LINUX==1)
               if (Mb_verbose) fprintf(stderr,"length(Mbs_longueur) %d \n",Mbs_longueur);
#endif
               Mbs_trame[4]=Mbs_c1;
               Mbs_trame[5]=Mbs_c2;
   }
			break;
		case 25 : 
               /* read for nothing */
//               Mbs_read(&Mbs_c);
   if (read(Mbs_device,&Mbs_c,1)>0) {									// byte cnt
			Mbs_status++;
               Mbs_trame[6]=Mbs_c;
   }
			break;
		case 26 : 
               /* read data to write */
			Mbs_i=0;
			Mbs_status++;
			break;
		case 27 : 
//                  Mbs_read(&Mbs_c1);
   if (read(Mbs_device,&Mbs_c1,1)>0) {									// data hi
				Mbs_status++;
   }
			break;
		case 28 : 
    //              Mbs_read(&Mbs_c2);
   if (read(Mbs_device,&Mbs_c2,1)>0) {									// data lo
                  //Mbs_data_to_write[Mbs_i]=(Mbs_c1<<8)+Mbs_c2;
                  Mbs_trame[7+(Mbs_i*2)]=Mbs_c1;
                  Mbs_trame[8+(Mbs_i*2)]=Mbs_c2;
	               Mbs_i++;
               if (Mbs_i<Mbs_longueur) {
					Mbs_status = 27;
               } else 
					Mbs_status++;
   }
			break;
		case 29 : 
               /* get crc16 */
//               Mbs_read(&Mbs_c1);
   if (read(Mbs_device,&Mbs_c1,1)>0) {									// get crc
					Mbs_status++;
   }
			break;
		case 30 : 
//               Mbs_read(&Mbs_c2);
   if (read(Mbs_device,&Mbs_c2,1)>0) {

				Mbs_status++;
				
               Mbs_trame[7+Mbs_longueur*2]=Mbs_c1;
               Mbs_trame[8+Mbs_longueur*2]=Mbs_c2;
#if (LINUX==1)
               if (Mb_verbose) fprintf(stderr,"crc  %x%x \n",Mbs_c1,Mbs_c2);
#endif
               /* check crc16 */
               if (Mb_test_crc(Mbs_trame,7+Mbs_longueur*2))
               {
#if (LINUX==1)
                  if (Mb_verbose) fprintf(stderr,"crc error\n");
#endif
                  //if(Mb_ptr_end_slve!=NULL)
                  //   (*Mb_ptr_end_slve)(-1,0,0);
                  break;
               }
#if (LINUX==1)
               if (Mb_verbose) 
                  for(Mbs_i=0;Mbs_i<=8+Mbs_longueur*2;Mbs_i++)
                     fprintf(stderr,"sended packet[%d] = %0x\n",Mbs_i,Mbs_trame[Mbs_i]);
#endif

			// copy data from modbus data
			// appl. spec. code
			for (Mbs_i=0;Mbs_i<(Mbs_longueur/2);Mbs_i++) {
				pulse_cnt[Mbs_i] = (((unsigned long)(Mbs_trame[7+(Mbs_i*4)]))<<24) + (((unsigned long)(Mbs_trame[8+(Mbs_i*4)]))<<16) + 
									(((unsigned long)(Mbs_trame[9+(Mbs_i*4)]))<<8) + ((unsigned long)(Mbs_trame[10+(Mbs_i*4)]));
//printf("pulse_cnt[%d] = %lx\n",Mbs_i,pulse_cnt[Mbs_i]);
			}

               /* comput answer packet */
               Mb_calcul_crc(Mbs_trame,6);

#if (LINUX==1)
               if (Mb_verbose)
               {
                  fprintf(stderr,"answer :\n");
                  for(Mbs_i=0;Mbs_i<8;Mbs_i++)
                     fprintf(stderr,"answer packet[%d] = %0x\n",Mbs_i,Mbs_trame[Mbs_i]);
               }
#endif
   }
			break;
		case 31 : 
            //Mbs_write(Mbs_trame,8);
			if (write(Mbs_device, Mbs_trame, 8			// tx. out
				#if (AVR==1)
				, &wcv_hc
				#endif
				)==8)
					Mbs_status = 200;
			break;
		case 200 : 
		   	Mbs_status = 0;
			break;
	}
//	} while (1);
}

/***********************************************************************************
   	   Mb_slave : start slave thread
************************************************************************************
input :
-------
mbsdevice  : device to use
msslave    : slave number
ptrfoncsnd : function to call when slave send data (can be NULL if you don't whant to use it)
ptrfoncrcv : function to call when slave receive data (can be NULL if you don't whant to use it)
ptrfoncend : function to call when slave finish to send answer (can be NULL if you don't whant to use it)

no output
***********************************************************************************/
void Mb_slave(int mbsdevice,int mbsslave
#if (LINUX==1)
	, void *ptrfoncsnd, void *ptrfoncrcv, void *ptrfoncend
#endif
	)
{
   Mbs_device=mbsdevice;
	Mbs_slave=mbsslave;
//   Mb_ptr_snd_data=ptrfoncsnd;
//  Mb_ptr_rcv_data=ptrfoncrcv;
//   Mb_ptr_end_slve=ptrfoncend;
//   pthread_create(&Mbs_thread, NULL,(void*)&Mbs,NULL);
}

#if 0

/***********************************************************************************
   	   Mb_slave_stop : stop slave thread
************************************************************************************
no input
no output
***********************************************************************************/
void Mb_slave_stop()
{
   pthread_cancel(Mbs_thread);
}
#endif	// #if 0

#endif	// #if (MODBUS==1)
