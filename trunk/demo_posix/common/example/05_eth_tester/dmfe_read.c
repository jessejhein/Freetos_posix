/*
 * Extracted from dm9000a.c
 */
 
#include <define.h>
#include <dm9000a.h>
#include <errno.h>
#include <fcntl.h>

#define DEBUG_RX

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
//  -----------------Packet Manipulation Functions--------------------------
static u16_t dmfe_gets(u8_t *val, u16_t len);
#if defined (DEBUG_RX) 
static void displayPreamble(DM_PREAMBLE* pRxPreamble);
#endif

/*****************************************************************************
 * Local Variables
 *****************************************************************************/
extern int eth_io_flag;
extern board_info_t macData;

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
            outb(DM9KA_MRCMD, macData.io_addr);
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
    u16_t i;
    outb(DM9KA_MRCMD, macData.io_addr);    
    for(i=0; i<len; i++)
    {
        val[i] = inb(macData.io_data);
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
