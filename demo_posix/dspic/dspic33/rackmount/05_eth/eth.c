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

/************************************************************************************************
 * Global Variables
 ***********************************************************************************************/
extern int fd_uart;
int fd_eth;
extern board_info_t macData;
u8_t uip_buf[UIP_BUFSIZE+2];

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

static void reg_test(void);
static void io_test(void);

static unsigned char test_flag = IO_TEST;

tskHTTPServer()
{
    start_process();
    
    if (test_flag == IO_TEST){
        io_test();
        test_flag = REG_TEST;
    }
    else{
        reg_test();
        test_flag = IO_TEST;
    }
    
    end_process();
}


/*
 * Register Test
 */
static void reg_test(void)
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
    
    while(printStr("Welcome to IO READ/WRITE TEST")<0);
    while(newline()<0);
    while(printStr("Press h for Help Screen")<0);
    while(newline()<0);

    while(1){
        while(read(fd_uart, &key, 1) < 1)
            usleep(0);
        
        if(key == 'x' || key == 'X') break;
        
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
                while(printStr("RX WP = ")<0);
                while(printHex((ior(db, DM9KA_RWPAH) << 8) + ior(db, DM9KA_RWPAL), 4)<0);
                while(newline()<0);
                break;
            case '2':
                while(printStr("RX RP = ")<0);
                while(printHex((ior(db, DM9KA_MRRH) << 8) + ior(db, DM9KA_MRRL), 4)<0);
                while(newline()<0);
                break;
            case 'h': case 'H':
                while(newline()<0);
                while(printStr("Help Screen")<0);
                while(newline()<0);
                while(printStr("===========")<0);
                while(newline()<0);
                while(printStr("Enter the Following Key:")<0);
                while(newline()<0);
                while(printStr(" t: Basic Test for DM9000A")<0);
                while(newline()<0);
                while(printStr(" p: Read a Physical register (auto-increment)")<0);
                while(newline()<0);
                while(printStr(" n: Read a Normal register (auto-increment)")<0);
                while(newline()<0);
                while(printStr(" 1: Read the rx buf read-pointer")<0);
                while(newline()<0);
                while(printStr(" 2: Read the rx buf write-pointer")<0);
                while(newline()<0);
                while(printStr(" r: Read a packet from rx buf (auto-increment)")<0);
                while(newline()<0);
                while(printStr(" R: Read a packet from rx buf (no increment)")<0);
                while(newline()<0);
                while(printStr(" x: exit IO READ/WRITE TEST")<0);
                while(newline()<0);
                while(newline()<0);
                break;
        }
    }
    close(fd_eth);
    sleep(1);
}

/*
 * IO test
 */
#define CHIP_VERSION    (0x2C)      //Expect 0x19
#define HEX_55          (0x55)      //Expect Don't care
#define HEX_AA          (0xAA)      //Expect Don't care
static void io_test(void)
{
    board_info_t* db = &macData;
    unsigned char key;             //received ASCII char
    unsigned char command = CHIP_VERSION;
    
    //Set up dsPic so that it can communicate with dm9000a
    ETH_IOCONFIG();             //Enable RG6(IOW#), RG7(IOR#), RG8(CMD), RG9(CS#) as output
    PCONFIG(1);                 //Data port as input
    ETH_ISR_EP = 0;             //INT0 is positive edge triggered
    ETH_ISR_IF = 0;             //Clear INT0 interrupt flag
    ETH_ISR_IE = 1;             //Enable INT0 Interrupt
    
    //Initialise address port (CMD) constants for I/O
    db->io_addr = CMD_INDEX;    //CMD = 0 : INDEX port
    db->io_data = CMD_DATA;     //CMD = 1 : DATA port

    while(printStr("Welcome to IO BUS TEST")<0);
    while(newline()<0);
    while(printStr("Press h for Help Screen")<0);
    while(newline()<0);

    while(1){
        if(read(fd_uart, &key, 1) > 0){
            if(key == 'x' || key == 'X') break;
            
            switch(key){
                case '1':
                    command = CHIP_VERSION;
                    break;
                case '2':
                    command = HEX_55;
                    break;
                case '3':
                    command = HEX_AA;
                    break;
                case 'h': case 'H':
                    while(newline()<0);
                    while(printStr("Help Screen")<0);
                    while(newline()<0);
                    while(printStr("===========")<0);
                    while(newline()<0);
                    while(printStr("Enter the Following Key:")<0);
                    while(newline()<0);
                    while(printStr(" 1: CHIP VERSION 0x2C")<0);
                    while(newline()<0);
                    while(printStr(" 2: 0x55")<0);
                    while(newline()<0);
                    while(printStr(" 3: 0xAA")<0);
                    while(newline()<0);
                    while(printStr(" x: exit IO BUS TEST")<0);
                    while(newline()<0);
                    while(newline()<0);
                    break;
                }
        }
        
        ior(db, command);              //Read 0x2C, Expected 0x19
        mdelay(1);
    }
    sleep(1);
}

/*
 * Test DM9000A
 */
static void test_dm9000a(board_info_t* db)
{
    //Vendor ID Test
    unsigned int vid = (ior(db, DM9KA_VIDH) << 8) + ior(db, DM9KA_VIDL);
    if(vid == 0x0A46){
        while(printStr("Vendor ID OK.")<0);
        while(newline()<0);
    }
    else{
        while(printStr("Vendor ID Test Failed. [Expected 0x0A46, Read 0x")<0);
        while(printHex(vid, 4)<0);
        while(printStr("].")<0);
        while(newline()<0);
    }
    
    //Mode Test
    if((ior(db, DM9KA_ISR) & 0x80) > 0){
        while(printStr("8-bit mode.")<0);
        while(newline()<0);
    }
    else{
        while(printStr("ERR: 16-bit mode detected.")<0);
        while(newline()<0);
    }
         
    //Link Test
    if((ior(db, DM9KA_NSR) & 0x40) > 0){
        while(printStr("Link OK.")<0);
        while(newline()<0);
        //Speed Test
        if(phy_read(db, DM9KA_ANLPAR) & 0x0020){
            while(printStr("10Base-T Half duplex obtained.")<0);
            while(newline()<0);
        }
        else{
            while(printStr("ERR: Speed is not 10Base-T Half duplex.")<0);
            while(newline()<0);
        }
    }    
    else{
        while(printStr("ERR: DM9000A is not able to link to the Internet.")<0);
        while(newline()<0);
    }
        
    while(newline()<0);
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
        
    while(printHex((DM9KA_PHY | phy_index), 4)<0); 
    while(printStr(" = ")<0);
    while(printHex(result, 4)<0);        
    while(newline()<0);

    phy_index++;
    
    if(phy_index>DM9KA_ANER && phy_index<DM9KA_DSCR){
        phy_index = DM9KA_DSCR;
    } else if(phy_index>DM9KA_PWDOR && phy_index<DM9KA_SCR){
        phy_index = DM9KA_SCR;
    } else if(phy_index>DM9KA_SCR){
        phy_index = 0;
        while(newline()<0);
    }
}

/*
 * Read Normal Registers
 */
static void readNormalReg(board_info_t* db)
{
    unsigned int result = ior(db, reg_index);

    while(printHex(reg_index, 2)<0);
    while(printStr(" = ")<0); 
    while(printHex(result, 2)<0);        
    while(newline()<0);
    
    if(reg_index==DM9KA_IMR){
        reg_index = 0;
        while(newline()<0);
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
    while(printStr(label)<0);
    while(printHex(rxbyte, 2)<0);
    while(newline()<0);
        
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
    while(printStr("status = ")<0);
    while(printHex(status, 2)<0);
    while(newline()<0);
            
    unsigned int len = inb(db->io_data) + (inb(db->io_data) << 8);
    while(printStr("len = ")<0);
    while(printHex(len, 4)<0);
    while(newline()<0);

    /*
     * Get packet
     */
    int rx_cnt=0, k=0;

    /*
     * Destination address
     */
    while(printStr("DA: ")<0);
    for(k=0; k<6; k++, rx_cnt++){
        rxbyte = inb(db->io_data);
        while(printHex(rxbyte, 2)<0);
    }
    while(newline()<0);

    /*
     * Source address
     */
    while(printStr("SA: ")<0);
    for(k=0; k<6; k++, rx_cnt++){
        rxbyte = inb(db->io_data);
        while(printHex(rxbyte, 2)<0);
    }
    while(newline()<0);

    /*
     * Type/len
     */
    while(printStr("TL: ")<0);
    for(k=0; k<2; k++, rx_cnt++){
        rxbyte = inb(db->io_data);
        while(printHex(rxbyte, 2)<0);
    }
    while(newline()<0);
     
    /*
     * Data
     */
    int row = 0;
    for(; rx_cnt<len; rx_cnt++, row++){
        rxbyte = inb(db->io_data);
        while(printHex(rxbyte, 2)<0);
        if(row%16 == 7){
            while(printStr(" ")<0);
        }
        if(row%16 == 15){
            while(newline()<0);
        }
    }
    while(newline()<0);
    while(newline()<0);
    
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
