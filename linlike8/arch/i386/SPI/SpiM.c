/*
 SPI Master 
*/

//---------------------------------------------------------------------------------------
// Include files
//---------------------------------------------------------------------------------------
        // Linux System module(s) definition
                // standard i/o
#include <stdio.h>
                // IO
#include <asm/io.h>                             
#include <sys/perm.h>
        // local definition
#include "SpiM.h"

//---------------------------------------------------------------------------------------
// declarations
//---------------------------------------------------------------------------------------
void SpiMComm(void);

        // SPI system
unsigned short baseAddr = 0x378;                                // default as 0x378
unsigned char ctrlReg;
        // writing buffer
unsigned char SpiMWrBuf;
        // reading
                // buffer
unsigned char SpiMRdBuf;
                // flag
                        // no comm. before, please do it
        #define NO_COMM         0
                        // comm. already done
        #define DONE_COMM       1
                        // comm. overrun
        #define OVER_RUN_COMM   2
unsigned char SpiMRdFlag = NO_COMM;
        
// input base address for SPI simulation
// return : -1 as error
char SpiMOpen(unsigned short bAddr)
{
        char retCode = 0;
        unsigned short backupBAddr;
        
        //for (;;) {
                
                // get base printer port address
                backupBAddr = baseAddr;
                baseAddr = bAddr;
                
                // directly get access to the i/o ports 
                if (ioperm(baseAddr, 3, 1)) {                           // printer port only need 3 bytes to access
                        retCode = -1;
                        //break;                                  // do not need to cont.
                }
                
                // backup original control register content
                ctrlReg = inb(baseAddr+2);

                // completed
                //break;                                          
                
        //}
        
        if (retCode==-1) {                                       // error
                baseAddr = backupBAddr;
        }
                
        return retCode;
}

// return : -1 as error
char SpiMClose(void)
{
        unsigned char retCode = 0;
        
                // We don't need the ports anymore
        if (ioperm(baseAddr, 3, 0)) {
                retCode = -1;
        }
        
        return retCode;
}

// Comm. format
//      * LSB sending 1st 
// Tx data 
//      * if none to send out, just send 0x00
//      * packet level will interprete whole packet, not only 0x00 to stop
void SpiMComm(void)
{
        int i;
        
        // select which slave to comm.; low active
        outb(ctrlReg &= ~0x04, baseAddr+2);
        
        for (i=0;i<8;i++) {
                
                // one clock cycle, takes around 5uSec
                
                // SCLK low
                outb(ctrlReg &= ~0x02, baseAddr+2);
                
                // prepare MOSI 
                if (SpiMWrBuf & 0x01) outb(ctrlReg |= 0x01, baseAddr+2);
                else outb(ctrlReg &= ~0x01, baseAddr+2);
                SpiMWrBuf >>= 1;
                
                // SCLK high
                outb(ctrlReg |= 0x02, baseAddr+2);
                
                // read MISO
                SpiMRdBuf >>= 1;
//printf("%x\n", inb(baseAddr+1));
                if (inb(baseAddr+1) & 0x40) SpiMRdBuf |= 0x80;
                
//printf("%x\n", SPIRdBuf);
//getchar();
                
        }
        
        // de-select which slave to comm.
        outb(ctrlReg |= 0x04, baseAddr+2);
        
        // clear Tx buffer
        SpiMWrBuf = 0;
        
}

// call SPIWrite once, should call SPIRead once; otherwise 
void SpiMWrite(unsigned char sWrBuf)
{
        SpiMWrBuf = sWrBuf;
        
        SpiMComm();
        
        if (SpiMRdFlag!=NO_COMM) SpiMRdFlag = OVER_RUN_COMM;              // if not NO_COMM, i.e. previous data is over-written
        else SpiMRdFlag = DONE_COMM;
        
}

unsigned char SpiMReadStatus(void)
{
        return SpiMRdFlag;
}

unsigned char SpiMRead(unsigned char* buf)
{
        //unsigned char retCode;
        
        if (SpiMRdFlag==OVER_RUN_COMM)
                return -1;                      // error
                
        if (SpiMRdFlag==NO_COMM)
                SpiMComm();
               
        *buf = SpiMRdBuf;
        
        SpiMRdBuf = 0;                   // clear reading buffer for next input data
        
        SpiMRdFlag = NO_COMM;
        
        //return retCode;
        return 1;                       // only one byte only each times in spi
}

/*

        
        a = inb(PRINTER_BASE_ADDR+2);                           // default should be 0xcc
        printf("Control as %x \n",a);
        printf("Status as %x \n",a);


        #define TNT_Out(reg,byte)       outb(byte, TNT_BASE_ADDRESS+reg)
        //#define TNT_Out_Word(reg,word)  outw(word, TNT_BASE_ADDRESS+reg)
        //#define TNT_In_Word(reg)        ((int)inw(TNT_BASE_ADDRESS+reg))
        #define TNT_In(reg)             ((unsigned short) inb(TNT_BASE_ADDRESS+reg))

        
        
        
        
*/
