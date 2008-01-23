/*
 * Extracted from dm9000a.c
 */

#include <define.h>
#include <dm9000a.h>
#include <asm/delay.h>

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
u8_t inb(int port)
{
    u8_t data;
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
    return data;
}

/*
 * outb()
 * +-- sends a byte ($value) to data port
 * +-- refer to section 10.3.4 of dm9000a datasheet
 */ 
void outb(u8_t value, int port)
{
    PCONFIG(0);
    PWRITE(value);
    ETH_IOCMD( (port<<2)|0x02 );
    Nop(); Nop();
    Nop(); Nop();
    ETH_IOCMD( 0x0F );                  
    PWRITE(0x00);                       
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
 * All values in ns
 */
u8_t ior(board_info_t *db, u8_t reg)
{
    //IOW#=1 to IOR#=0 requires about 600ns
    outb(reg, db->io_addr);
    return inb(db->io_data);
}

/*
 * Write a byte ($value) to register ($reg)
 */
void iow(board_info_t *db, u8_t reg, u8_t value)
{
    outb(reg, db->io_addr);
    outb(value, db->io_data);
}

/*
 * Read a word from PHY Registers
 * See dm9000a.h for description
 */
u16_t phy_read(board_info_t *db, u8_t reg)
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
void phy_write(board_info_t *db, u8_t reg, u16_t value)
{
    iow(db, DM9KA_EPAR, (u8_t)(DM9KA_PHY | reg));
    iow(db, DM9KA_EPDRL, (u8_t)(value & 0xff));
    iow(db, DM9KA_EPDRH, (u8_t)( (value >> 8) & 0xff));
    iow(db, DM9KA_EPCR, 0xa);
    udelay(50);
    iow(db, DM9KA_EPCR, 0x0);
}
