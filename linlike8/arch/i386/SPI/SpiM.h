/*
 header of SPI Master 
 
 Connection 
         Implementation : i386 SPI Master Hardware driver 
                        |
                Interface : 
                        * init. - char SpiMOpen(unsigned short bAddr);
                        * close - char SpiMClose(void);
                        * send one byte - void SpiMWrite(unsigned char sWrBuf);
                        * read one byte - unsigned char SpiMRead(unsigned char* buf);

 Implementation methods 
        * simulation of SPI only 
                * this device driver only simulate SPI, since i386 PC does not have this device
                * simulation through i386 PC printer port 
        * polling without buffer to hold data 
                * cpu processing is faster than comm.
        * polling with buffer to hold data 
        * interrupt without buffer to hold data 
        * interrupt with buffer to hold data 

 interface Pins to outside world
        MISO    - input         - ACK
        MOSI    - output        - Strobe
        SCLK    - output        - LF
        /SS(s)  - output        - Init

 Mode 
        default as Mode 0, rising edge to acquisition
                
 Examples
        include "Arch/i386/Drivers/SPI/SpiM.h"
        if (SpiMOpen(0x378)==-1)
                return -1;
        while (1) {
                
                //SPIWrite(0x56);
                SpiMWrite(0xa5);
                SpiMRead(&inByte);
                printf("%x\n", inByte);
        }
        if (SpiMClose()==-1)
                return -1;
                
        
*/

char SpiMOpen(unsigned short bAddr);
char SpiMClose(void);
void SpiMWrite(unsigned char sWrBuf);
// return 
//      1       - valid data 
//      0       - non data 
//      -1      - error
unsigned char SpiMRead(unsigned char* buf);
//                        * read status - unsigned char SpiMReadStatus(void);
//unsigned char SpiMReadStatus(void);


