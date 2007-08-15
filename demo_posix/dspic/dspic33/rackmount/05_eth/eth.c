/*
 * eth.c 
 */

#include <define.h>
#include <unistd.h>
#include <fcntl.h>
#include <dm9000a.h>
#include <asm/delay.h>
#include "uip.h"
#include "uip_arp.h"


static void displayRxPacket(board_info_t* db, int flag);


/************************************************************************************************
 * Global Variables
 ***********************************************************************************************/
extern int fd_uart;
int fd_eth;
extern board_info_t macData;
u8_t uip_buf[UIP_BUFSIZE+2];

#define DEFAULT_TEST    0
#define REG_TEST        1
#define IO_TEST         2

#define TEST_SELECT     DEFAULT_TEST

#if (TEST_SELECT == IO_TEST)
/*
 * IO test
 */
tskHTTPServer()
{
    board_info_t* db = &macData;
    
    //Set up dsPic so that it can communicate with dm9000a
    ETH_IOCONFIG();             //Enable RG6(IOW#), RG7(IOR#), RG8(CMD), RG9(CS#) as output
    PCONFIG(1);                 //Data port as input
    ETH_ISR_EP = 0;             //INT0 is positive edge triggered
    ETH_ISR_IF = 0;             //Clear INT0 interrupt flag
    ETH_ISR_IE = 1;             //Enable INT0 Interrupt
    
    //Initialise address port (CMD) constants for I/O
    db->io_addr = CMD_INDEX;    //CMD = 0 : INDEX port
    db->io_data = CMD_DATA;     //CMD = 1 : DATA port

    start_process();
    
    ior(db, 0x2C);
    
    mdelay(1);
    
    end_process();
}
#elseif(TEST_SELECT == REG_TEST)
/*
 * Register Test
 */
tskHTTPServer()
{
    /*
     * Initialize dspic and dm9000a
     */
    fd_eth = open(ETHERNET, O_RDWR);
    
    /*
     * Common Variable 
     */
    board_info_t* db = &macData;    //ethernet object
    unsigned char key;             //received ASCII char

    u8_t reg_index = 0x00;
    u8_t phy_index = 0x00;      

    start_process();
    
    while(read(fd_uart, &key, 1) < 1)
        usleep(0);
    
    /*
     * Display register in PHY
     */
    if(key == 'p' || key == 'P')
    {
        u16_t result = phy_read(db, phy_index);
        
        printHex((DM9KA_PHY | phy_index), 4); printStr(" = "); printHex(result, 4);        
        newline();

        phy_index++;
        
        if(phy_index>DM9KA_ANER && phy_index<DM9KA_DSCR){
            phy_index = DM9KA_DSCR;
        } else if(phy_index>DM9KA_PWDOR && phy_index<DM9KA_SCR){
            phy_index = DM9KA_SCR;
        } else if(phy_index>DM9KA_SCR){
            phy_index = 0;
        }
    }
    /*
     * Display register in normal register
     */
    else if(key == 'n' || key == 'N')
    {
        unsigned int result = ior(db, reg_index);


        printHex(reg_index, 2); printStr(" = "); printHex(result, 2);        
        newline();

        reg_index++;
        
        if(reg_index>DM9KA_GPR && reg_index<DM9KA_TRPAL){
            reg_index = DM9KA_TRPAL;
        } else if(reg_index>DM9KA_RWPAH && reg_index<DM9KA_VIDL){
            reg_index = DM9KA_VIDL;
        } else if(reg_index>DM9KA_LEDCR && reg_index<DM9KA_BUSCR){
            reg_index = DM9KA_BUSCR;
        } else if(reg_index>DM9KA_INTCR && reg_index<DM9KA_SCCR){
            reg_index = DM9KA_SCCR;
        } else if(reg_index>DM9KA_RSCCR && reg_index<DM9KA_MRCMDX){
            reg_index = DM9KA_MRCMDX;
        } else if(reg_index>DM9KA_MRCMD && reg_index<DM9KA_MRRL){
            reg_index = DM9KA_MRRL;
        } else if(reg_index>DM9KA_MWCMDX && reg_index<DM9KA_MWCMD){
            reg_index = DM9KA_MWCMD;
        } else if(reg_index>DM9KA_MWCMD && reg_index<DM9KA_MWRL){
            reg_index =DM9KA_MWRL;
        }            
    }
    /*
     * Display value in Rx Buffer
     */ 
    else if(key == 'r' || key == 'R')
    {
        ior(db, DM9KA_MRCMDX);      //Dummy read, without increment of pointer

        unsigned int rxbyte = inb(db->io_data);  //Get most updated data
        
        char label[] = "rx = ";
        printStr(label);
        printHex(rxbyte, 2);
        newline();
        
        //Packet available, reset read pointer if 'R'
        if( (rxbyte & 0x01) ){
            int flag = (key == 'r')? 0 : 1;
            displayRxPacket(db, flag);
        }
    }
    else if(key == '1'){
        unsigned int rd_ptr = (ior(db, DM9KA_RWPAH) << 8) + ior(db, DM9KA_RWPAL);
        
        char label[] = "RX WP = ";
        printStr(label);
        printHex(rd_ptr, 4);
        newline();
    }
    else if(key == '2'){
        unsigned int rd_ptr = (ior(db, DM9KA_MRRH) << 8) + ior(db, DM9KA_MRRL);

        char label[] = "RX RP = ";
        printStr(label);
        printHex(rd_ptr, 4);
        newline();
    }
    
    end_process();
}
#else
/*
 * Default Test
 */
tskHTTPServer()
{
    /*
     * Initialize dspic and dm9000a
     */
    fd_eth = open(ETHERNET, O_RDWR);
    
    /*
     * Common Variable 
     */
    board_info_t* db = &macData;    //ethernet object
    unsigned char key;             //received ASCII char
    
    //Initialise address port (CMD) constants for I/O
    db->io_addr = CMD_INDEX;    //CMD = 0 : INDEX port
    db->io_data = CMD_DATA;     //CMD = 1 : DATA port

    start_process();
    
    //wait for user input
    while(read(fd_uart, &key, 1) < 1)
        usleep(0);
    
    //Vendor ID Test
    unsigned int vid = (ior(db, DM9KA_VIDH) << 8) + ior(db, DM9KA_VIDL);
    if(vid == 0x0A46){
        printStr("Vendor ID OK.");
        newline();
    }
    else{
        printStr("Vendor ID Test Failed. [Expected 0x0A46, Read 0x");
        printHex(vid, 4);
        printStr("].");
        newline();
    }
    
    //Mode Test
    if((ior(db, DM9KA_ISR) & 0x80) > 0){
        printStr("8-bit mode.");
        newline();
    }
    else{
        printStr("ERR: 16-bit mode detected.");
        newline();
    }
     
    //Link Test
    if((ior(db, DM9KA_NSR) & 0x40) > 0){
        printStr("Link OK.");
        newline();
        //Speed Test
        if(phy_read(db, DM9KA_ANLPAR) & 0x0020){
            printStr("10Base-T Half duplex obtained.");
            newline();
        }
        else{
            printStr("ERR: Speed is not 10Base-T Half duplex.");
            newline();
        }
    }    
    else{
        printStr("ERR: DM9000A is not able to link to the Internet.");
        newline();
    }
    
    newline();
    
    end_process();
}
#endif

/*
 * Display rx packet on console
 * flag:    0, read pointer will not be reset
 */
static void displayRxPacket(board_info_t* db, int flag)
{
    /*store the value of Memory Data Read address register*/
    u8_t MDRAH=ior(db, DM9KA_MRRH);
    u8_t MDRAL=ior(db, DM9KA_MRRL);
    
    unsigned int rxbyte;

    /*
     * Get 4-byte header
     */
    outb(DM9KA_MRCMD, db->io_addr); //read with increment

    rxbyte = inb(db->io_data);      //do not display ready code
    
    unsigned int status = inb(db->io_data);
    printStr("status = ");
    printHex(status, 2);
    newline();
            
    unsigned int len = inb(db->io_data) + (inb(db->io_data) << 8);
    printStr("len = ");
    printHex(len, 4);
    newline();

    /*
     * Get packet
     */
    int rx_cnt=0, k=0;

    /*
     * Destination address
     */
    printStr("DA: ");
    for(k=0; k<6; k++, rx_cnt++){
        rxbyte = inb(db->io_data);
        printHex(rxbyte, 2);
    }
    newline();

    /*
     * Source address
     */
    printStr("SA: ");
    for(k=0; k<6; k++, rx_cnt++){
        rxbyte = inb(db->io_data);
        printHex(rxbyte, 2);
    }
    newline();

    /*
     * Type/len
     */
    printStr("TL: ");
    for(k=0; k<2; k++, rx_cnt++){
        rxbyte = inb(db->io_data);
        printHex(rxbyte, 2);
    }
    newline();
     
    /*
     * Data
     */
    int row = 0;
    for(; rx_cnt<len; rx_cnt++, row++){
        rxbyte = inb(db->io_data);
        printHex(rxbyte, 2);
        if(row%16 == 7){
            printStr(" ");
        }
        if(row%16 == 15){
            newline();
        }
    }
    newline();
    newline();
    
    /*
     * Reset ptr if needed
     */
    if(flag != 0){
        iow(db, DM9KA_MRRH, MDRAH);
        iow(db, DM9KA_MRRL, MDRAL);        
    }
}

tskLaserCtrl()
{
    start_process();
    {
        usleep(0);
    }
    end_process();
}
