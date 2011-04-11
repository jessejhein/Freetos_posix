/**
 * \addtogroup drivers Drivers
 * @{
 * 
 * Implementation of Drivers for DsPic
 */

/**
 * \defgroup dm9000a Davicom DM9000A
 * @{
 */

/**
 * \file
 * MAC Module (DAVICOM DM9000A) for uip TCP/IP Stack
 * \author Dennis Tsang <dennis@amonics.com>
 */

/*
 * Copyright (C) 2007-2011  Dennis Tsang <dennis@amonics.com>
 *
 * This file is part of freertos_posix.
 *
 * freertos_posix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * freertos_posix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with freertos_posix.  If not, see <http://www.gnu.org/licenses/>
 */

#include <define.h>
#include "dm9000a.h"
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>

/*****************************************************************************
 * DEBUG 
 *****************************************************************************/
//#define DEBUG_DMFE_RX
//#define DEBUG_DMFE_TX  //Enabling this will slow down communication significantly

/*****************************************************************************
 * Local Function Declaration
 *****************************************************************************/
//  -------------------------Basic I/O operations---------------------------
static u8_t inb (int port);
static void outb (u8_t value, int port);
static u8_t ior (board_info_t *db, u8_t reg);
static void iow (board_info_t *db, u8_t reg, u8_t value);
static u16_t phy_read (board_info_t *db, u8_t reg);
static void phy_write (board_info_t *db, u8_t reg, u16_t value);
#if defined(MAC_USE_EEPROM)
static u16_t eeprom_read (board_info_t *db, u8_t offset);
#endif
//  --------------------Initialisation Functions----------------------------
static void dmfe_init_dm9000 (board_info_t *db);
static void set_PHY_mode (board_info_t *db);
static void dm9000_hash_table (board_info_t *db);
//  -----------------Packet Manipulation Functions--------------------------
static u16_t dmfe_gets (u8_t *val, u16_t len);
static u16_t dmfe_puts (u8_t *val, u16_t len);
//  ----------------------Debug Functions-----------------------------------
#ifdef DEBUG_DMFE_RX
static void dmfe_debug_preamble (DM_PREAMBLE* pRxPreamble);
#endif /* DEBUG_DMFE_RX */
#ifdef DEBUG_DMFE_TX
static void dmfe_debug_mac_header (__u8 *pheader);
static void dmfe_debug_payload (void *addr, int len, int* pos);
#endif /* DEBUG_DMFE_TX */

/*****************************************************************************
 * Local Variables
 *****************************************************************************/
/** store io setting */
static int eth_io_flag;
/** indicate whether I/O functions are called from ISR */
static int eth_io_in_interrupt;
/** dm9000a board information data */
static board_info_t macData;


/**
 * \internal
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
 */
int 
dmfe_open (int flags)
{
  eth_io_flag = flags;

  board_info_t* db = &macData;

  ETH_IOCONFIG ();
  ETH_ISR_EP = ETH_IRQ_POLARITY;
  ETH_ISR_IP = 7;
  ETH_ISR_IF = 0;
  ETH_ISR_IE = 1;
  db->io_addr = CMD_INDEX;
  db->io_data = CMD_DATA;
    
  //check for LAN card
  if (ior (db, DM9KA_VIDL) != 0x46)
    {
      struct uip_eth_addr mac = {0,0,0,0,0,0};
      uip_setethaddr (mac);
      errno = ENXIO;
      return -1;
    }
    
  //Initialise DM9000A registers, set PHY_MODE, and hash table
  dmfe_init_dm9000 (db);

  //wait 5 second for auto-negotiation to complete
#ifndef FREERTOS_SCHED
  mdelay (5000);
#else /* FREERTOS_SCHED */
  sleep (5);
#endif /* FREERTOS_SCHED */

  //Determine whether the link is successful  
  if ((ior (db, DM9KA_NSR) & 0x40) > 0)
    {
      socket_init ();
      return 0;
    }
  else
    {
      errno = EACCES;
      return -1;
    }
}

/**
 * \brief Initialise dm9000a board
 * \param db pointer to board information
 */
static void 
dmfe_init_dm9000 (board_info_t *db)
{
  //Reset dm9000a
  iow (db, DM9KA_GPR, 1);
  udelay (500);
  iow (db, DM9KA_GPR, 0);
  udelay (20);
  iow (db, DM9KA_NCR, 3);
  udelay (20);
  iow (db, DM9KA_NCR, 3);
  udelay (20);

  //Assign I/O mode:
  db->io_mode = ior (db, DM9KA_ISR) >> 6;
  db->op_mode = DM9KA_10MHD;
    
  //Set PHY_MODE
  set_PHY_mode (db);

  //Program operating register
  iow (db, DM9KA_NCR, 0);
  iow (db, DM9KA_TCR, 0);
  iow (db, DM9KA_NSR, 0x2c);
  iow (db, DM9KA_ISR, 0x3f);
  iow (db, DM9KA_INTCR, 0x02);
  //TX UDP/TCP/IP checksum enable
  //Receive checksum enable
#if defined(CHECKSUM)
  iow (db, DM9KA_TCSCR, 0x07);
  iow (db, DM9KA_RCSCSR, 0x03);
#endif
    
  //Set address filter table
  dm9000_hash_table (db);

  //Activate DM9000A 
  iow (db, DM9KA_RCR, DM9KA_RX_FILTER | 1);
  iow (db, DM9KA_IMR, DM9KA_EN_ISR);
    
  //Initialise driver variables
  db->tx_pkt_cnt = 0;
}

/**
 * \brief Set PHY operating mode
 * \param db pointer to board information
 */
static void 
set_PHY_mode (board_info_t *db)
{
  u16_t phy_reg0 = 0x1200;
  u16_t phy_reg4 = 0x01e1;

  if (db->op_mode == DM9KA_10MHD)
    {
      phy_reg0 = 0x1000;
      phy_reg4 = 0x21; 
    }
    
  phy_write (db, DM9KA_BMCR, phy_reg0);
  phy_write (db, DM9KA_ANAR, phy_reg4);
}

/**
 * \brief Set DM9000A multicast address
 * \param db pointer to board information
 */
static void 
dm9000_hash_table (board_info_t *db)
{
  __u32 hash_val;
  u16_t i, oft, hash_table[4];
  struct uip_eth_addr mac;

  //Initialise MAC Address
  //-------------------Obtain MAC address---------------------------
  //-----------either from EEPROM or from application---------------
#if defined(MAC_USE_EEPROM)
  int epr_oft = DM9KA_EP_MACADDR0;
  for (; i<3; i++, epr_oft++)
    {
      ((u16_t *)mac.addr)[i] = eeprom_read (epr_oft);
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
  for (i = 0, oft = DM9KA_PAR; i < 6; i++, oft++)
    iow (db, oft, mac.addr[i]);
  uip_setethaddr (mac);
    
  //Initialise Hash Table
  //--------------------Clear Hash Table---------------------------- 
  for (i = 0; i < 4; i++)
    hash_table[i] = 0x0;
  hash_table[3] = 0x8000;

  //--------------Write the hash table to MAC MD table-------------- 
  for (i = 0, oft = DM9KA_MAR; i < 4; i++)
    {
      iow (db, oft++, (u8_t)(hash_table[i] & 0xff));
      iow (db, oft++, (u8_t)((hash_table[i] >> 8) & 0xff));
    }
}


/**
 * \internal
 * Reset PHY and power down PHY
 * Disable interrupts and receive packet
 */
int 
dmfe_close (void)
{
  eth_io_flag = 0;

  board_info_t* db = &macData;    

  phy_write (db, DM9KA_BMCR, 0x8000);
  iow (db, DM9KA_GPR, 0x01);
  iow (db, DM9KA_IMR, DM9KA_DIS_ISR);
  iow (db, DM9KA_RCR, 0x00);

  return 0;
}


int 
dmfe_read (void)
{
  board_info_t *db = &macData;

  //Ethernet link is broken
  if ((ior(db, DM9KA_NSR) & 0x40) == 0)
    {
      errno = EIO;
      return -1;
    }
    
  //Perform read if read operation is enabled
  if (eth_io_flag & O_RDWR || eth_io_flag & O_RDONLY)
    {
      DM_PREAMBLE RxPreamble;
        
      //Determine if a valid packet is in the rx buffer
      ior (db, DM9KA_MRCMDX);
      u8_t ready = inb (db->io_data);
    
#if !defined(CHECKSUM)
      if (!(ready == 0x01)) return 0;
#else
      if (!(ready & 0x01)) return 0;
#endif
   
      //Extract information from preamble 
#ifdef DEBUG_DMFE_RX
      printf("\n   Rx Packet: ");
#endif /* DEBUG_DMFE_RX */
      dmfe_gets ((u8_t*)&RxPreamble, sizeof (RxPreamble));
#ifdef DEBUG_DMFE_RX
      printf("\n   ======================\n");
      dmfe_debug_preamble (&RxPreamble);
#endif /* DEBUG_DMFE_RX */
    
      //Check packet status for errors and discard packet when there are errors
      if ( (RxPreamble.StatusVector & 0xBF) || (RxPreamble.PacketLength > UIP_CONF_BUFFER_SIZE) )
        {
          u16_t index;
          //Read the remaining bytes
          outb (DM9KA_MRCMD, db->io_addr);
          for (index = 0; index < RxPreamble.PacketLength; index++)
            {
              inb (db->io_data);
            }
#ifdef DEBUG_DMFE_RX 
          printf("**RX BAD PACKET**\n");
#endif /* DEBUG_DMFE_RX */
          return 0;
        }
    
      //Copy packet to uip global buffer
      return (int) dmfe_gets (uip_buf, RxPreamble.PacketLength);
    }
  //Error, raise error flag
  else
    {
      errno = EBADF;
      return -1;
    } 
}

/**
 * \brief Extract data from rx packet
 * \param val destination buffer
 * \param len number of bytes to read
 */
static u16_t 
dmfe_gets (u8_t *val, u16_t len)
{
  board_info_t *db = &macData;

  u16_t i;
  outb (DM9KA_MRCMD, db->io_addr);
  for (i = 0; i < len; i++)
    {
      val[i] = inb (db->io_data);
    }
#ifdef DEBUG_DMFE_RX 
  int k, r;
  for (k = 0; k < len; k++, r++)
    {
      printf("%02X", ((__u8*)val)[k]);
      switch (r%16)
        {
          case 3: case 7: case 11:
            printf(" ");
            break;
          case 15:
            printf("\n");
            break;
        }
    }
#endif /* DEBUG_DMFE_RX */
    return i;
}

#ifdef DEBUG_DMFE_RX 
/**
 * \brief display the content of the rx preamble
 * \param pRxPreamble destination data structure
 */
static void 
dmfe_debug_preamble (DM_PREAMBLE* pRxPreamble)
{
  unsigned int status = (unsigned int)(pRxPreamble->StatusVector);
  printf("status = %02X\n", status);
            
  unsigned int len = (unsigned int)(pRxPreamble->PacketLength);
  printf("len = %04X\n", len);
}
#endif /* DEBUG_DMFE_RX */


int 
dmfe_write (void)
{
  board_info_t *db = &macData;

  //Perform write if write operation is enabled
  if (eth_io_flag & O_RDWR || eth_io_flag & O_WRONLY)
    {
      //There is an empty buffer space available
      if (db->tx_pkt_cnt == 0)
        {
          db->tx_pkt_cnt++;
          iow (db, DM9KA_IMR, DM9KA_DIS_ISR);

          //
          // MAC header
          //
          dmfe_puts (&uip_buf[0], UIP_LLH_LEN);
#ifdef DEBUG_DMFE_TX
          int k=0, row=0;
          printf("   Tx Packet: %d\n", (unsigned int) uip_len);
          printf("   ======================\n");
          dmfe_debug_mac_header (uip_buf);
#endif /* DEBUG_DMFE_TX */

          //
          // Short packet < MAC+TCPIP header len
          //
          if (uip_len <= UIP_LLH_LEN + UIP_TCPIP_HLEN)
            {
              dmfe_puts (&uip_buf[UIP_LLH_LEN], uip_len - UIP_LLH_LEN);
#ifdef DEBUG_DMFE_TX
              dmfe_debug_payload (&uip_buf[UIP_LLH_LEN], uip_len - UIP_LLH_LEN, &row);
              printf("\n\n");
#endif /* DEBUG_DMFE_TX */
            }

          //
          // TCPIP header + payload
          //
          else 
            {
              dmfe_puts (&uip_buf[UIP_LLH_LEN], UIP_TCPIP_HLEN);
              dmfe_puts (uip_appdata, uip_len - UIP_TCPIP_HLEN - UIP_LLH_LEN);
#ifdef DEBUG_DMFE_TX
              dmfe_debug_payload (&uip_buf[UIP_LLH_LEN], UIP_TCPIP_HLEN, &row);
              dmfe_debug_payload (uip_appdata, uip_len - UIP_TCPIP_HLEN - UIP_LLH_LEN, &row);
              printf("\n\n");
#endif /* DEBUG_DMFE_TX */
            }
            
          //
          // Wait until all transmission stop
          // Then, start transmission
          //
          iow (db, DM9KA_TXPLL, (uip_len & 0xff));
          iow (db, DM9KA_TXPLH, (uip_len >> 8) & 0xff);
          iow (db, DM9KA_TCR, 0x1);
          iow (db, DM9KA_IMR, DM9KA_EN_ISR);

          return uip_len;
        }
      //dm9000a is busy in transmitting, comes back later
      else
        {
          errno = EAGAIN;
          return -1;
        }
    }
  //Error, raise error flag
  else
    {
      errno = EBADF;
      return -1;
    }
}

/**
 * \brief Put data to tx buffer
 * \param val source buffer
 * \param len number of bytes to write
 */
static u16_t 
dmfe_puts (u8_t *val, u16_t len)
{
  board_info_t *db = &macData;

  u16_t i;
  outb (DM9KA_MWCMD, db->io_addr);
  for (i = 0; i < len; i++)
    {
      outb(val[i], db->io_data);
    }
  return i;
}

#ifdef DEBUG_DMFE_TX
/**
 * \brief display MAC header
 * \param pheader pointer to MAC header
 * \remarks Example
 * \verbatim
   DA: FF-FF-FF-FF-FF-FF
   SA: 00-06-0E-00-00-01
   TL: 0800
   \endverbatim
 */
static void 
dmfe_debug_mac_header (__u8 *pheader)
{
  __u8 *addr;

  addr = pheader;
  printf("DA: ");
  printf("%02X-%02X-%02X-%02X-%02X-%02X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

  addr = pheader + 6;
  printf("SA: ");
  printf("%02X-%02X-%02X-%02X-%02X-%02X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

  printf("TL: ");
  addr = pheader + 12;
  printf("%02X%02X\n", addr[0], addr[1]);
}

/**
 * \brief display payload of MAC packet
 * \param addr pointer to start of payload position
 * \param len length of payload
 * \param pos counter to record how many bytes has been printed
 * \remarks Example
 * \verbatim
   00112233 44556677 8899AABB CCDDEEFF
   00112233 44556677 8899AABB CCDDEEFF
   00112233 4455
   \endverbatim
 */
static void 
dmfe_debug_payload (void *addr, int len, int* pos)
{
  int k;
  for (k = 0; k < len; k++, (*pos)++)
    {
      printf("%02X", ((__u8*)addr)[k]);
      switch ((*pos)%16)
        {
          case 3: case 7: case 11:
            printf(" "); 
            break;
          case 15:
            printf("\n"); 
            break;
        }
    }
}
#endif /* DEBUG_DMFE_TX */





/**
 * \brief Interrupt Service Routine after the packet has been transmitted
 * \remarks Process Tx Interrupt
 * \verbatim
  
       1st          1st
     write()     Interrupt
        |            |
       \|/          \|/
   ----------------------------------------------------------->Time
                               /|\         /|\
                                |           |
                               2nd         2nd
                             write()     Interrupt
  
        |<--cnt=1-->|<--cnt=0-->|<--cnt=1-->|<--cnt=0-->|
         no more     2nd
         write()     write()
         allowed     allowed
   \endverbatim
 */ 
void 
dmfe_interrupt (void)
{
  board_info_t *db = &macData;
  int isr_status, i;
  u8_t reg_save;

  //set io rd rw
  eth_io_in_interrupt = 1;
  
  //Pre-handling of interrupt
  reg_save = inb (db->io_addr);
  iow (db, DM9KA_IMR, DM9KA_DIS_ISR);
  isr_status = ior (db, DM9KA_ISR);
  iow (db, DM9KA_ISR, (u8_t)isr_status);

  //Transmit Interrupt: Free a transmit buffer 
  db->tx_pkt_cnt = 0;

  //Post-handling of interrupt
  iow (db, DM9KA_IMR, DM9KA_EN_ISR);
  outb (reg_save, db->io_addr);

  //resume normal io rd wr
  eth_io_in_interrupt = 0;

  //Clear Interrupt Flag
  ETH_ISR_IF = 0;
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

/**
 * \brief read a byte from data port
 * \param port read address/data
 * \return value of address/data
 * \remarks refer to section 10.3.3 of dm9000a datasheet
 */ 
static u8_t 
inb (int port)
{
  u8_t data;

  if (eth_io_in_interrupt == 0) SR |= 0x00e0;

  PCONFIG (1);
  //[bit 4] | [bit 3] | [bit 2] | bit 1]
  //CS# = 0 |ADDR=port|IOW# = 1 |IOR# = 0
  ETH_IOCMD ( (port << 2) | 0x02 );
  Nop(); Nop();
  Nop(); Nop();
  Nop(); Nop();
  Nop(); Nop();
  Nop(); Nop();
  Nop(); Nop();
  Nop(); Nop();
  Nop(); Nop();
  data = PREAD();
  //[bit 4] | [bit 3] | [bit 2] | bit 1]
  //CS# = 1 |ADDR = 1 |IOW# = 1 |IOR# = 1
  ETH_IOCMD (0x0F);
  PCONFIG (0);
  PWRITE (0x00);
  
  if (eth_io_in_interrupt == 0) SR &= 0x00e0;

  return data;
}

/**
 * \brief sends a byte ($value) to data port
 * \param value value of address/data
 * \param port write to address/data
 * \remarks refer to section 10.3.4 of dm9000a datasheet
 */ 
static void 
outb (u8_t value, int port)
{
  if (eth_io_in_interrupt == 0) SR |= 0x00e0;

  PCONFIG (0);
  PWRITE (value);
  //[bit 4] | [bit 3] | [bit 2] | bit 1]
  //CS# = 1 |ADDR = 1 | IOW# = 0  |IOR# = 1
  ETH_IOCMD ( (port << 2) | 0x01 );
  Nop(); Nop();
  Nop(); Nop();
  //[bit 4] | [bit 3] | [bit 2] | bit 1]
  //CS# = 1 |ADDR = 1 |IOW# = 1 |IOR# = 1
  ETH_IOCMD (0x0F);
  PWRITE (0x00);

  if (eth_io_in_interrupt == 0) SR &= 0x00e0;
}

/**
 * \brief Read a byte at register ($reg)
 * \param db board information
 * \param reg register address
 * \return data in address
 * 
 * \verbatim
   CS#      ________       ___________         ________
                    |     |           |       |
                    |_____|           |_______|
                    <-130-><---550---><--200-->
  
   IOW#/CMD ________       ____________________________
                    |     |           
                    |_____|           
  
   IOR#     __________________________         ________
                                      |       |
                                      |_______|
             <-230->       <70>       <5>      <60>
   DATA     -<================>----------<========>----
             <------430------->          <--260--->
   
   All values in ns, base on 2 Nop() in inb() and 1 Nop() in outb()
   \endverbatim
 */
static u8_t 
ior (board_info_t *db, u8_t reg)
{
  //IOW#=1 to IOR#=0 requires about 600ns
  outb (reg, db->io_addr);
  return inb (db->io_data);
}

/**
 * \brief Write a byte ($value) to register ($reg)
 * \param db board information
 * \param reg register address
 * \param value data to set in register address 
 */
static void 
iow (board_info_t *db, u8_t reg, u8_t value)
{
  outb (reg, db->io_addr);
  outb (value, db->io_data);
}

/**
 * \brief Read a word from PHY Registers
 * \param db board information
 * \param reg register address
 * \return data in address
 * \remarks See dm9000a.h for description
 */
static u16_t 
phy_read (board_info_t *db, u8_t reg)
{
  iow (db, DM9KA_EPAR, (u8_t)(DM9KA_PHY | reg));
  iow (db, DM9KA_EPCR, 0xc);
  udelay (50);
  iow (db, DM9KA_EPCR, 0x0);
  return ((u16_t)ior(db, DM9KA_EPDRH) << 8) | ((u16_t)ior(db, DM9KA_EPDRL) & 0xff);
}

/**
 * \brief Write a word to phyxcer
 * \param db board information
 * \param reg register address
 * \param value data to set in register address 
 * \remarks See dm9000a.h for description
 */
static void 
phy_write (board_info_t *db, u8_t reg, u16_t value)
{
  iow (db, DM9KA_EPAR, (u8_t)(DM9KA_PHY | reg));
  iow (db, DM9KA_EPDRL, (u8_t)(value & 0xff));
  iow (db, DM9KA_EPDRH, (u8_t)( (value >> 8) & 0xff));
  iow (db, DM9KA_EPCR, 0xa);
  udelay (50);
  iow (db, DM9KA_EPCR, 0x0);
}

#if defined(MAC_USE_EEPROM) 
/**
 * \brief Read a word data from EEPROM (untested)
 * \param db board information
 * \param offset location of data
 * \return value in EEPROM
 */
static u16_t 
eeprom_read (board_info_t *db, u8_t offset)
{
  iow (db, DM9KA_EPAR, offset);
  iow (db, DM9KA_EPCR, 0x4);
  udelay (200);
  iow (db, DM9KA_EPCR, 0x0);
  return ((u16_t)ior(db, DM9KA_EPDRH) << 8) | ((u16_t)ior(db, DM9KA_EPDRL) & 0xff);
}
#endif

/** @} */
/** @} */
