/*
 * extracted from dm9000a.c 
 */

#include <define.h>
#include <dm9000a.h>
#include <asm/delay.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

/*****************************************************************************
 * Local Function Declaration
 *****************************************************************************/
//  -------------------------Basic I/O operations---------------------------
u8_t inb(int port);
void outb(u8_t value, int port);
u8_t ior(board_info_t *db, u8_t reg);
void iow(board_info_t *db, u8_t reg, u8_t value);
u16_t phy_read(board_info_t *db, u8_t reg);
void phy_write(board_info_t *db, u8_t reg, u16_t value);
//  --------------------Initialization Functions----------------------------
static void dmfe_init_dm9000(board_info_t *db);
static void set_PHY_mode(board_info_t *db);
static void dm9000_hash_table(board_info_t *db);


/*****************************************************************************
 * Local Variables
 *****************************************************************************/
int eth_io_flag;
// --------------------------DM9000A Setup----------------------------------
static int media_mode = DM9KA_10MHD;
board_info_t macData;
struct uip_eth_addr uip_ethaddr = {{0,0,0,0,0,0}};

/*****************************************************************************
 * Name:                int dmfe_open(int flags)
 * 
 * Input:               flags: accessing mode
 * 
 * Output:              -1: eth is not linked (EACCES)
 *                          no lan card (ENXIO)    
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
    
    //check for lan card
    if(ior(db, DM9KA_VIDL) != 0x46)
    {
        struct uip_eth_addr mac = {0,0,0,0,0,0};
        uip_setethaddr(mac);
        errno = ENXIO;
        return -1;
    }
    
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
