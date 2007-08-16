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

#define REG_TEST        0
#define IO_TEST         1
#define TEST_SELECT     REG_TEST

/************************************************************************************************
 * Global Variables
 ***********************************************************************************************/
extern int fd_uart;
int fd_eth;
extern board_info_t macData;
u8_t uip_buf[UIP_BUFSIZE+2];

#if (TEST_SELECT == REG_TEST)
/*
 * Global Variables
 */
static u8_t reg_index = 0x00;
static u8_t phy_index = 0x00; 
/*
 * Local Variables
 */
static void readPHYReg(board_info_t* db);
static void readNormalReg(board_info_t* db);
static void readRXBuf(board_info_t* db, unsigned char enter_key);
static void displayRxPacket(board_info_t* db, int flag);
static void test_dm9000a(board_info_t* db);
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

    start_process();
    
    while(read(fd_uart, &key, 1) < 1)
        usleep(0);
    
    /*
     * Display register in PHY
     */
    switch(key){
        case 't': case 'T':
            test_dm9000a(db);
            break;
        case 'p': case 'P':
            readPHYReg(db);
            break;
        case 'n': case 'N':
            readNormalReg(db);           
            break;
        case 'r': case 'R':
            readRXBuf(db, key);
            break;
        case '1':
            printStr("RX WP = ");
            printHex((ior(db, DM9KA_RWPAH) << 8) + ior(db, DM9KA_RWPAL), 4);
            newline();
            break;
        case '2':
            printStr("RX RP = ");
            printHex((ior(db, DM9KA_MRRH) << 8) + ior(db, DM9KA_MRRL), 4);
            newline();
            break;
        default:
            newline();
            printStr("Help Screen");
            newline();
            printStr("===========");
            newline();
            printStr("Enter the Following Key:");
            newline();
            printStr(" t: Basic Test for DM9000A");
            newline();
            printStr(" p: Read a Physical register (auto-increment)");
            newline();
            printStr(" n: Read a Normal register (auto-increment)");
            newline();
            printStr(" 1: Read the rx buf read-pointer");
            newline();
            printStr(" 2: Read the rx buf write-pointer");
            newline();
            printStr(" r: Read a packet from rx buf (auto-increment)");
            newline();
            printStr(" R: Read a packet from rx buf (no increment)");
            newline();
            newline();
    }
    
    end_process();
}
#else
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
    
    ior(db, 0x2C);              //Read 0x2C, Expected 0x19
    
    mdelay(1);
    
    end_process();
}
#endif

/*
 * Test DM9000A
 */
static void test_dm9000a(board_info_t* db)
{
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
}

/*
 * Read Physical Registers
 */
static void readPHYReg(board_info_t* db)
{
    u16_t result = phy_read(db, phy_index);
    
    if(phy_index == DM9KA_BMSR){
        //Read once more for status register
        result = phy_read(db, phy_index);
    }
        
    printHex((DM9KA_PHY | phy_index), 4); printStr(" = "); printHex(result, 4);        
    newline();

    phy_index++;
    
    if(phy_index>DM9KA_ANER && phy_index<DM9KA_DSCR){
        phy_index = DM9KA_DSCR;
    } else if(phy_index>DM9KA_PWDOR && phy_index<DM9KA_SCR){
        phy_index = DM9KA_SCR;
    } else if(phy_index>DM9KA_SCR){
        phy_index = 0;
        newline();
    }
}

/*
 * Read Normal Registers
 */
static void readNormalReg(board_info_t* db)
{
    unsigned int result = ior(db, reg_index);

    printHex(reg_index, 2); printStr(" = "); printHex(result, 2);        
    newline();
    
    if(reg_index==DM9KA_IMR){
        reg_index = 0;
        newline();
    } 
    else{
        reg_index++;
        
        if(reg_index>DM9KA_GPR && reg_index<DM9KA_TRPAL){
            reg_index = DM9KA_TRPAL;
        } else if(reg_index>DM9KA_RWPAH && reg_index<DM9KA_VIDL){
            reg_index = DM9KA_VIDL;
        } else if(reg_index>DM9KA_LEDCR && reg_index<DM9KA_BUSCR){
            reg_index = DM9KA_BUSCR;
        } else if(reg_index>DM9KA_INTCR && reg_index<DM9KA_SCCR){
            reg_index = DM9KA_SCCR;
        } else if(reg_index>DM9KA_RSCCR && reg_index<DM9KA_ISR){
            reg_index = DM9KA_ISR;
        }        
    }
}

/*
 * Read RX Buffer
 */
static void readRXBuf(board_info_t* db, unsigned char enter_key)
{
    ior(db, DM9KA_MRCMDX);      //Dummy read, without increment of pointer

    unsigned int rxbyte = inb(db->io_data);  //Get most updated data
        
    char label[] = "rx = ";
    printStr(label);
    printHex(rxbyte, 2);
    newline();
        
    //Packet available, reset read pointer if 'R'
    if( (rxbyte & 0x01) ){
        int flag = (enter_key == 'r')? 0 : 1;
        displayRxPacket(db, flag);
    }
}

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

/*
 * Dummy Laser Ctrl Task
 */
tskLaserCtrl()
{
    start_process();
    {
        usleep(0);
    }
    end_process();
}
