/*********************************************************************
 *
 *     MAC Module (DAVICOM DM9000A) for uip TCP/IP Stack
 *
 *********************************************************************
 * 
 *  MAC HEADER
 *  ==========
 *              +-------------------+
 *  Dest. MAC   | FF FF FF FF FF FF |
 *              +-------------------+
 *  Source MAC  | 00 60 6E 00 00 00 |
 *              +-------+-----------+
 *  Type        | 08 00 |                           //00=IP, 06=ARP
 *              +-------+------------------+
 *  Payload     | XX XX  ... ... ... XX XX |        //0 - 1500 bytes
 *              +-------------+------------+
 *  FCS         | 11 22 33 44 |
 *              +-------------+
 * 
 *********************************************************************/

#include <define.h>
#include <dm9000a.h>
#include <asm/delay.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#if (ETHERNET_MOD == 0)
//do not include ETHERNET module if disabled

#else

/*****************************************************************************
 * DEBUG 
 *****************************************************************************/
//#define DEBUG_RX
//#define DEBUG_TX  //Enabling this will slow down communication significantly

/*****************************************************************************
 * Local Function Declaration
 *****************************************************************************/
//  -------------------------Basic I/O operations---------------------------
static u8_t inb(int port);
static void outb(u8_t value, int port);
static u8_t ior(board_info_t *db, u8_t reg);
static void iow(board_info_t *db, u8_t reg, u8_t value);
static u16_t phy_read(board_info_t *db, u8_t reg);
static void phy_write(board_info_t *db, u8_t reg, u16_t value);
#if defined(MAC_USE_EEPROM)
static u16_t eeprom_read(board_info_t *db, u8_t offset);
#endif
//  --------------------Initialization Functions----------------------------
static void dmfe_init_dm9000(board_info_t *db);
static void set_PHY_mode(board_info_t *db);
static void dm9000_hash_table(board_info_t *db);
//  -----------------Packet Manipulation Functions--------------------------
static u16_t dmfe_gets(u8_t *val, u16_t len);
static u16_t dmfe_puts(u8_t *val, u16_t len);
#if defined (DEBUG_RX) 
static void displayPreamble(DM_PREAMBLE* pRxPreamble);
#endif

/*****************************************************************************
 * Local Variables
 *****************************************************************************/
static int eth_io_flag;
// --------------------------DM9000A Setup----------------------------------
static board_info_t macData;

/*****************************************************************************
 * Name:                int dmfe_open(int flags)
 * 
 * Input:               flags: accessing mode
 * 
 * Output:              -1: eth is not linked (EACCES)
 *                      0:  eth is linked
 * 
 * Function:            Initialize dm9000a
 *****************************************************************************
 * IO_PIN_INIT;
 * +--RG6(IOW#), RG7(IOR#), RG8(CMD), RG9(CS#)
 * +--Data port RD4 - RD11
 * +--INT0 is positive edge triggered
 * 
 * SET_PHY_MODE:
 * +--Enable Auto-negotiation (10Mbps, Half Duplex)
 * +--Disable flow control
 * 
 * SET_HASH_TABLE:
 * +--Set Default MAC Address
 * +--Set to receive Broadcast packets
 * 
 * SET_MAC_REGISTERS:
 * +--Auto append CRC and pads
 * +--TX Buffer: two packet mode
 * +--Enable Rx
 *****************************************************************************/
int dmfe_open(int flags)
{
    eth_io_flag = flags;

    board_info_t* db = &macData;

    ETH_IOCONFIG();             
    PCONFIG(1);             
    ETH_ISR_EP = 0;
    ETH_ISR_IF = 0;
    ETH_ISR_IE = 1;
    db->io_addr = CMD_INDEX;
    db->io_data = CMD_DATA;
    
    //Initialize DM9000A registers, set PHY_MODE, and hash table
    dmfe_init_dm9000(db);

    //wait 3 second for autonegotation to complete
#if(FREERTOS_SCHE_ENABLE == 1)
    sleep(5);
#else    
    mdelay(5000); 
#endif

    //Determine whether the link is successful  
    if((ior(db, DM9KA_NSR) & 0x40) > 0)
    {
        return 0;
    }
    else
    {
        errno = EACCES;
        return -1;
    }
}

/*
 * Initilize dm9000a board
 */
static void dmfe_init_dm9000(board_info_t *db)
{
    //Reset dm9000a
    iow(db, DM9KA_GPR, 1);  
    udelay(500);
    iow(db, DM9KA_GPR, 0);  
    udelay(20);    
    iow(db, DM9KA_NCR, 3);
    udelay(20);
    iow(db, DM9KA_NCR, 3);
    udelay(20);

    //Assign I/O mode:
    db->io_mode = ior(db, DM9KA_ISR) >> 6;
    db->op_mode = DM9KA_10MHD;
    
    //Set PHY_MODE
    set_PHY_mode(db);

    //Program operating register
    iow(db, DM9KA_NCR, 0);      
    iow(db, DM9KA_TCR, 0);
    iow(db, DM9KA_NSR, 0x2c);
    iow(db, DM9KA_ISR, 0x3f);
    iow(db, DM9KA_INTCR, 0x02); 
    //TX UDP/TCP/IP checksum enable
    //Receive checksum enable
#if defined(CHECKSUM)
    iow(db, DM9KA_TCSCR, 0x07);
    iow(db, DM9KA_RCSCSR, 0x03);
#endif
    
    //Set address filter table
    dm9000_hash_table(db);

    //Activate DM9000A 
    iow(db, DM9KA_RCR, DM9KA_RX_FILTER | 1);
    iow(db, DM9KA_IMR, DM9KA_EN_ISR);
    
    //Initialize driver variables
    db->tx_pkt_cnt = 0;
}

/* 
 * Set PHY operating mode
 */
static void set_PHY_mode(board_info_t *db)
{
    u16_t phy_reg0 = 0x1200;
    u16_t phy_reg4 = 0x01e1;

    if(db->op_mode == DM9KA_10MHD){
        phy_reg0 = 0x1000;
        phy_reg4 = 0x21; 
    }
    
    phy_write(db, DM9KA_BMCR, phy_reg0);
    phy_write(db, DM9KA_ANAR, phy_reg4);
}

/*
 * Set DM9000A multicast address
 */
static void dm9000_hash_table(board_info_t *db)
{
    __u32 hash_val;
    u16_t i, oft, hash_table[4];
    struct uip_eth_addr mac;

    //Initialize MAC Address
    //-------------------Obtain MAC address---------------------------
    //-----------either from EEPROM or from application---------------
#if defined(MAC_USE_EEPROM)
    int epr_oft = DM9KA_EP_MACADDR0;
    for(; i<3; i++, epr_oft++)
    {
        ((u16_t *)mac.addr)[i] = eeprom_read(epr_oft);
    }
#else
    u16_t OUI_MSB = phy_read(db, DM9KA_PHYID1);
    u16_t OUI_LSB = phy_read(db, DM9KA_PHYID2); 
    mac.addr[0] = (u8_t)((OUI_MSB & 0xFC00) >> 10);
    mac.addr[1] = (u8_t)((OUI_MSB & 0x03FC) >> 2);
    mac.addr[2] = (u8_t)(((OUI_MSB & 0x0003) << 6) + ((OUI_LSB & 0xFC00) >> 10));
    mac.addr[3] = DEFAULT_NIC_BYTE1;
    mac.addr[4] = DEFAULT_NIC_BYTE2;
    mac.addr[5] = DEFAULT_NIC_BYTE3;
#endif
    //Set MAC address
    for (i=0, oft=DM9KA_PAR; i<6; i++, oft++)
        iow(db, oft, mac.addr[i]);
    uip_setethaddr(mac);
    
    //Initialize Hash Table
    //--------------------Clear Hash Table---------------------------- 
    for (i=0; i<4; i++)
        hash_table[i] = 0x0;
    hash_table[3] = 0x8000;

    //--------------Write the hash table to MAC MD table-------------- 
    for (i=0, oft=DM9KA_MAR; i<4; i++) {
        iow(db, oft++, (u8_t)(hash_table[i] & 0xff));
        iow(db, oft++, (u8_t)((hash_table[i] >> 8) & 0xff));
    }
}



/*****************************************************************************
 * Name:                int dmfe_close()
 * 
 * Input:               None
 * 
 * Output:              0: ok
 * 
 * Function:            Reset PHY and power down PHY
 *                      Disable interrupts and receive packet
 *****************************************************************************/
int dmfe_close(void)
{
    eth_io_flag = 0;

    board_info_t* db = &macData;    

    phy_write(db, DM9KA_BMCR, 0x8000);
    iow(db, DM9KA_GPR, 0x01);
    iow(db, DM9KA_IMR, DM9KA_DIS_ISR);
    iow(db, DM9KA_RCR, 0x00);

    return 0;
}



/********************************************************************************************
 * Name:        int dmfe_read(void)
 * 
 * Function:    copy the received packet to default buffer
 * 
 * Input:       None
 * 
 * Ouput:       >0, indicating a packet is received
 *              0, indicating no data is available
 *              -1: error 
 *                  +-- eth is not opened for read operation (errno = EBADF)
 *                  +-- eth link is broken (errno =EIO)
 *********************************************************************************************/
int dmfe_read(void)
{
    board_info_t *db = &macData;

    //Ethernet link is broken
    if((ior(db, DM9KA_NSR) & 0x40) == 0)
    {
        errno = EIO;
        return -1;
    }
    
    //Perform read if read operation is enabled
    if(eth_io_flag & O_RDWR || eth_io_flag & O_RDONLY)
    {
        DM_PREAMBLE RxPreamble;
        
        //Determine if a valid packet is in the rx buffer
        ior(db, DM9KA_MRCMDX);
        u8_t ready = inb(db->io_data);
    
#if !defined(CHECKSUM)
        if(!(ready == 0x01)) return 0;
#else
        if(!(ready & 0x01)) return 0;
#endif
   
        //Extract information from preamble 
        dmfe_gets((u8_t*)&RxPreamble, sizeof(RxPreamble));   
#if defined (DEBUG_RX) 
        displayPreamble(&RxPreamble);
#endif
    
        //Check packet status for errors and discard packet when there are errors
        if( (RxPreamble.StatusVector & 0xBF) || (RxPreamble.PacketLength > UIP_CONF_BUFFER_SIZE) ){
            u16_t index;
            //Read the remaining bytes
            outb(DM9KA_MRCMD, db->io_addr);
            for (index=0; index<RxPreamble.PacketLength; index++){
                inb(db->io_data);
            }
#if defined (DEBUG_RX) 
            printStr("**RX BAD PACKET**");
            newline();
#endif
            return 0;
    }
    
    //Copy packet to uip global buffer
    return (int) dmfe_gets(uip_buf, RxPreamble.PacketLength);
        
    }
    //Error, raise error flag
    else
    {
        errno = EBADF;  //io not opened for reading
        return -1;
    } 
}

/*
 * Extract data from rx packet
 * u16_t dmfe_gets(u8_t *val, u16_t len)
 */
static u16_t dmfe_gets(u8_t *val, u16_t len)
{
    board_info_t *db = &macData;

    u16_t i;
    outb(DM9KA_MRCMD, db->io_addr);    
    for(i=0; i<len; i++)
    {
        val[i] = inb(db->io_data);
    }
    return i;
}

/*
 * display the content of the rx preamble
 */
#if defined (DEBUG_RX) 
static void displayPreamble(DM_PREAMBLE* pRxPreamble)
{
    unsigned int status = (unsigned int)(pRxPreamble->StatusVector);
    printStr("status = ");
    printHex(status, 2);
    newline();
            
    unsigned int len = (unsigned int)(pRxPreamble->PacketLength);
    printStr("len = ");
    printHex(len, 4);
    newline();
}
#endif





/********************************************************************************************
 * Name:        int dmfe_write(unsigned char device, unsigned char *buf, int count)
 * 
 * Function:    copy count bytes of the data from buf and transmit
 * 
 * Input:       None       
 * 
 * Ouput:       the number of bytes written (>= 0)
 *              -1: error, uart is not opened for writing (errno = EBADF)
 * 
 *********************************************************************************************/
int dmfe_write(void)
{
    board_info_t *db = &macData;

    //Perform write if write operation is enabled
    if(eth_io_flag & O_RDWR || eth_io_flag & O_WRONLY)
    {
        //There is an empty buffer space available
        if(db->tx_pkt_cnt == 0)
        {
            db->tx_pkt_cnt++;
            iow(db, DM9KA_IMR, DM9KA_DIS_ISR);

            //
            // MAC header
            //
            dmfe_puts(&uip_buf[0], UIP_LLH_LEN);
#if defined (DEBUG_TX)
            int k=0, row=0;
            printStr("   Tx Packet: ");
            printDec((unsigned int) uip_len);
            newline(); 
            printStr("   ======================");
            newline(); 
            printMACHeader(uip_buf);
#endif

            //
            // Short packet < MAC+TCPIP header len
            //
            if(uip_len <= UIP_LLH_LEN + UIP_TCPIP_HLEN) {
                dmfe_puts(&uip_buf[UIP_LLH_LEN], uip_len - UIP_LLH_LEN);
#if defined (DEBUG_TX)
                printPayload(&uip_buf[UIP_LLH_LEN], uip_len - UIP_LLH_LEN, &row);
                newline();
                newline();
#endif
            } 

            //
            // TCPIP header + payload
            //
            else {
                dmfe_puts(&uip_buf[UIP_LLH_LEN], UIP_TCPIP_HLEN);
                dmfe_puts(uip_appdata, uip_len - UIP_TCPIP_HLEN - UIP_LLH_LEN);
#if defined (DEBUG_TX)
                printPayload(&uip_buf[UIP_LLH_LEN], UIP_TCPIP_HLEN, &row);
                printPayload(uip_appdata, uip_len - UIP_TCPIP_HLEN - UIP_LLH_LEN, &row);
                newline();
                newline();
#endif
            }
            
            //
            // Wait until all transimssion stop
            // Then, start transmission
            //
            iow(db, DM9KA_TXPLL, (uip_len & 0xff));
            iow(db, DM9KA_TXPLH, (uip_len >> 8) & 0xff);
            iow(db, DM9KA_TCR, 0x1);
            iow(db, DM9KA_IMR, DM9KA_EN_ISR);

            return uip_len;
        }
        //dm9000a is busy in transmitting, comes back later
        else
        {
            errno = EAGAIN;  //io is busy
            return -1;
        }
    }
    //Error, raise error flag
    else
    {
        errno = EBADF;  //io not opened for writing
        return -1;
    }
}

/*
 * Put data to tx buffer
 * u16_t dmfe_puts(u8_t *val, u16_t len)
 */
static u16_t dmfe_puts(u8_t *val, u16_t len)
{
    board_info_t *db = &macData;

    u16_t i;
    outb(DM9KA_MWCMD, db->io_addr);
    for (i=0; i<len; i++)
    {
        outb(val[i], db->io_data);
    }
    return i;
}




/*****************************************************************************
 * void dmfe_interrupt(void)
 *****************************************************************************
 * Process Tx Interrupt
 * 
 *      1st          1st
 *    write()     Interrupt
 *       |            |
 *      \|/          \|/
 * ------------------------------------------------------------>Time
 *                              /|\         /|\
 *                               |           |
 *                              2nd         2nd
 *                            write()     Interrupt
 * 
 *       |<--cnt=1-->|<--cnt=0-->|<--cnt=1-->|<--cnt=0-->|
 *        no more     2nd
 *        write()     write()
 *        allowed     allowed
 * 
 *****************************************************************************/
void dmfe_interrupt(void)
{
    board_info_t *db = &macData;
    int isr_status, i;
    u8_t reg_save;
    
    //Pre-handling of interrupt
    reg_save = inb(db->io_addr);
    iow(db, DM9KA_IMR, DM9KA_DIS_ISR);
    isr_status = ior(db, DM9KA_ISR);
    iow(db, DM9KA_ISR, (u8_t)isr_status);

    //Transmit Interrupt: Free a transmit buffer 
    db->tx_pkt_cnt = 0;

    //Post-handling of interrupt
    iow(db, DM9KA_IMR, DM9KA_EN_ISR);
    outb(reg_save, db->io_addr);

    ETH_ISR_IF = 0;       //Clear Interrupt Flag
}




/*********************************************************************
 * Basic I/O Operations
 *********************************************************************
 * 
 *      -----------------------------------------------------
 *      word level              phy_read()      phy_write()
 *      -----------------------------------------------------
 *      register/data           ior()           iow()
 *      -----------------------------------------------------
 *      byte level              inb()           outb()
 *      =====================================================
 *                          HARDWARE
 * 
 *********************************************************************/

/*
 * inb()
 * +-- read a byte from data port
 * +-- refer to section 10.3.3 of dm9000a datasheet
 */ 
static u8_t inb(int port)
{
    u8_t data;
    
    SR |= 0x00e0;

    PCONFIG(1);
    ETH_IOCMD( (port<<2)|0x01 );        
    Nop(); Nop();
    Nop(); Nop();
    Nop(); Nop();
    Nop(); Nop();
    Nop(); Nop();
    Nop(); Nop();
    Nop(); Nop();
    Nop(); Nop();
    data = PREAD();
    ETH_IOCMD( 0x0F );
    PCONFIG(0);
    PWRITE(0x00);
    
    SR &= 0x00e0;

    return data;
}

/*
 * outb()
 * +-- sends a byte ($value) to data port
 * +-- refer to section 10.3.4 of dm9000a datasheet
 */ 
static void outb(u8_t value, int port)
{
    SR |= 0x00e0;
    
    PCONFIG(0);
    PWRITE(value);
    ETH_IOCMD( (port<<2)|0x02 );
    Nop(); Nop();
    Nop(); Nop();                           
    ETH_IOCMD( 0x0F );                  
    PWRITE(0x00);
    
    SR &= 0x00e0;                    
}

/*
 * Read a byte at register ($reg)
 * 
 * CS#      ________       ___________         ________
 *                  |     |           |       |
 *                  |_____|           |_______|
 *                  <-130-><---550---><--200-->
 *  
 * IOW#/CMD ________       ____________________________
 *                  |     |           
 *                  |_____|           
 * 
 * IOR#     __________________________         ________
 *                                    |       |
 *                                    |_______|
 *           <-230->       <70>       <5>      <60>
 * DATA     -<================>----------<========>----
 *           <------430------->          <--260--->
 * 
 * All values in ns, base on 2 Nop() in inb() and 1 Nop() in outb()
 */
static u8_t ior(board_info_t *db, u8_t reg)
{
    //IOW#=1 to IOR#=0 requires about 600ns
    outb(reg, db->io_addr);
    return inb(db->io_data);
}

/*
 * Write a byte ($value) to register ($reg)
 */
static void iow(board_info_t *db, u8_t reg, u8_t value)
{
    outb(reg, db->io_addr);
    outb(value, db->io_data);
}

/*
 * Read a word from PHY Registers
 * See dm9000a.h for description
 */
static u16_t phy_read(board_info_t *db, u8_t reg)
{
    iow(db, DM9KA_EPAR, (u8_t)(DM9KA_PHY | reg));
    iow(db, DM9KA_EPCR, 0xc);
    udelay(50);
    iow(db, DM9KA_EPCR, 0x0);
    return ((u16_t)ior(db, DM9KA_EPDRH) << 8) | ((u16_t)ior(db, DM9KA_EPDRL) & 0xff);
}

/*
 * Write a word to phyxcer
 * See dm9000a.h for description
 */
static void phy_write(board_info_t *db, u8_t reg, u16_t value)
{
    iow(db, DM9KA_EPAR, (u8_t)(DM9KA_PHY | reg));
    iow(db, DM9KA_EPDRL, (u8_t)(value & 0xff));
    iow(db, DM9KA_EPDRH, (u8_t)( (value >> 8) & 0xff));
    iow(db, DM9KA_EPCR, 0xa);
    udelay(50);
    iow(db, DM9KA_EPCR, 0x0);
}

/*
 * Read a word data from EEPROM (untested)
 */
#if defined(MAC_USE_EEPROM) 
static u16_t eeprom_read(board_info_t *db, u8_t offset)
{
    iow(db, DM9KA_EPAR, offset);
    iow(db, DM9KA_EPCR, 0x4);
    udelay(200);
    iow(db, DM9KA_EPCR, 0x0);
    return ((u16_t)ior(db, DM9KA_EPDRH) << 8) | ((u16_t)ior(db, DM9KA_EPDRL) & 0xff);
}
#endif

#endif //ETHERNET_MOD
