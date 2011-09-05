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

/**
 * \page dm9000a LAN CARD Driver Module
 * \verbatim
    MAC HEADER
    ==========
                +-------------------+
    Dest. MAC   | FF FF FF FF FF FF |
                +-------------------+
    Source MAC  | 00 60 6E 00 00 00 |
                +-------+-----------+
    Type        | 08 00 |                           //00=IP, 06=ARP
                +-------+------------------+
    Payload     | XX XX  ... ... ... XX XX |        //0 - 1500 bytes
                +-------------+------------+
    FCS         | 11 22 33 44 |
                +-------------+
   \endverbatim
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

#ifndef DM9000A_H_
#define DM9000A_H_

#include "uip.h"
#include "uip_arp.h"

/*******************************************************************
 * DM9000A Precomplier Operation Setup
 *******************************************************************/
#define CHECKSUM                   //TX generation and RX checking
//#define MAC_USE_EEPROM            //MAC use data in EEPROM

/*******************************************************************
 * DM9000A Constants for Operation
 *******************************************************************/
#define DM9KA_RX_FILTER     0x30    //Discard CRC error and LONG packets 

#define DM9KA_EN_ISR        0x82    //IMR: enable circular RX buffer and TX interrupt
#define DM9KA_DIS_ISR       0x80    //Disable interrupt
#define DM9KA_RX_ISR        0x01
#define DM9KA_TX_ISR        0x02
#define DM9KA_LK_ISR        0x20

#define DM9KA_DWORD_MODE    1       //Values of DM9KA_ISR<7:6>
#define DM9KA_BYTE_MODE     2
#define DM9KA_WORD_MODE     0

#define CMD_INDEX           0       //Logic at CMD pin for address and data access
#define CMD_DATA            1

/*******************************************************************
 * DM9000A Vendor Control and Status Registers
 *******************************************************************/
#define DM9KA_NCR      0x00    //Network Control Register
#define DM9KA_NSR      0x01    //Network Status Register
#define DM9KA_TCR      0x02    //TX Control Register
#define DM9KA_TSR1     0x03    //TX Status Register I
#define DM9KA_TSR2     0x04    //TX Status Register II
#define DM9KA_RCR      0x05    //RX Control Register
#define DM9KA_RSR      0x06    //RX Status Register
#define DM9KA_ROCR     0x07    //Receive Overflow Counter Register
#define DM9KA_BPTR     0x08    //Back Pressure Threshold Register
#define DM9KA_FCTR     0x09    //Flow Control Threshold Register
#define DM9KA_FCR      0x0A    //RX Flow Control Register
#define DM9KA_EPCR     0x0B    //EEPROM & PHY Control Register
#define DM9KA_EPAR     0x0C    //EEPROM & PHY Address Register
#define DM9KA_EPDRL    0x0D    //EEPROM & PHY Low Byte Data Register
#define DM9KA_EPDRH    0x0E    //EEPROM & PHY High Byte Data Register
#define DM9KA_WCR      0x0F    //Wake Up Control Register (in 8-bit mode)

#define DM9KA_PAR      0x10    //Physical Address Register 0
//#define               0x11    //Physical Address Register 1
//#define               0x12    //Physical Address Register 2
//#define               0x13    //Physical Address Register 3
//#define               0x14    //Physical Address Register 4
//#define               0x15    //Physical Address Register 5
#define DM9KA_MAR      0x16    //Multicast Address Register 0
//#define               0x17    //Multicast Address Register 1
//#define               0x18    //Multicast Address Register 2
//#define               0x19    //Multicast Address Register 3
//#define               0x1A    //Multicast Address Register 4
//#define               0x1B    //Multicast Address Register 5
//#define               0x1C    //Multicast Address Register 6
//#define               0x1D    //Multicast Address Register 7
#define DM9KA_GPCR     0x1E    //General Purpose Control Register (in 8-bit mode)
#define DM9KA_GPR      0x1F    //General Purpose Register

//#define               0x20
//#define               0x21
#define DM9KA_TRPAL    0x22    //TX SRAM Read Pointer Address Low Byte
#define DM9KA_TRPAH    0x23    //TX SRAM Read Pointer Address High Byte
#define DM9KA_RWPAL    0x24    //RX SRAM Write Pointer Address Low Byte
#define DM9KA_RWPAH    0x25    //RX SRAM Write Pointer Address High Byte
//#define               0x26
//#define               0x27
#define DM9KA_VIDL     0x28    //Vendor ID Low Byte
#define DM9KA_VIDH     0x29    //Vendor ID High Byte
#define DM9KA_PIDL     0x2A    //Product ID Low Byte
#define DM9KA_PIDH     0x2B    //Product ID High Byte
#define DM9KA_CHIPR    0x2C    //CHIP Revision
#define DM9KA_TCR2     0x2D    //TX Control Register 2
#define DM9KA_OCR      0x2E    //Operation Control Register
#define DM9KA_SMCR     0x2F    //Speical Mode Control Register

#define DM9KA_ETXCSR   0x30    //Early Transmit Control/Status Register
#define DM9KA_TCSCR    0x31    //Transmit Check Sum Control Register
#define DM9KA_RCSCSR   0x32    //Receive Check Sum Control Status Register
#define DM9KA_MPAR     0x33    //MII PHY Address Register
#define DM9KA_LEDCR    0x34    //LED Pin Control Register
//#define               0x35
//#define               0x36
//#define               0x37
#define DM9KA_BUSCR    0x38    //Processor Bus Control Register
#define DM9KA_INTCR    0x39    //INT Pin control Register

#define DM9KA_PHY      0x40    //PHY address 0x01

#define DM9KA_SCCR     0x50    //System Clock Turn ON Control Register
#define DM9KA_RSCCR    0x51    //Resume System Clock Control Register

#define DM9KA_MRCMDX   0xF0    //Memory Data Pre-Fetch Read Command Without Address Increment Register
#define DM9KA_MCRMDX1  0xF1    //Memory Data Read Command Without Address Increment Register
#define DM9KA_MRCMD    0xF2    //Memory Data Read Command With Address Increment Register
//#define               0xF3
#define DM9KA_MRRL     0xF4    //Memory Data Read Address Register Low Byte
#define DM9KA_MRRH     0xF5    //Memory Data Read Address Register High Byte
#define DM9KA_MWCMDX   0xF6    //Memory Data Write Command Without Address Increment Register
//#define               0xF7
#define DM9KA_MWCMD    0xF8    //Memory Data Write Command With Address Increment Register
//#define               0xF9
#define DM9KA_MWRL     0xFA    //Memory Data Write Address Register Low Byte
#define DM9KA_MWRH     0xFB    //Memory Data Write Address Register High Byte
#define DM9KA_TXPLL    0xFC    //TX Packet Length Low Byte Register
#define DM9KA_TXPLH    0xFD    //TX Packet Length High Byte Register
#define DM9KA_ISR      0xFE    //Interrupt Status Register
#define DM9KA_IMR      0xFF    //Interrupt Mask Register

/*******************************************************************
 * DM9000A PHY Registers
 * Operations:
 * 1)   Load DM9KA_EPAR
 *      +--Set DM9KA_EPAR<5:0> = one of the following address
 *      +--Set DM9KA_EPAR<7:6> = 01 (DM9KA_PHY)
 * 2a)  PHY Write
 *      +--Load DM9KA_EPDRH and DM9KA_EPDRL with data
 *      +--Set DM9KA_EPCR = 0x0a
 *      +--Wait for completion (5us)
 *      +--Reset DM9KA_EPCR = 0x00 
 * 2b)  PHY Read
 *      +--Set DM9KA_EPCR = 0x0c
 *      +--Wait for completion (5us)
 *      +--Reset DM9KA_EPCR = 0x00 
 *      +--Read DM9KA_EPDRH and DM9KA_EPDRL with data
 *******************************************************************/
#define DM9KA_BMCR      0x00    //Basic Mode Control Register
#define DM9KA_BMSR      0x01    //Basic Mode Status Register
#define DM9KA_PHYID1    0x02    //PHY ID Identifier Register #1
#define DM9KA_PHYID2    0x03    //PHY ID Identifier Register #2
#define DM9KA_ANAR      0x04    //Auto-negotiation Advertisement Register
#define DM9KA_ANLPAR    0x05    //Auto-negotiation Link Partner Ability Register
#define DM9KA_ANER      0x06    //Auto-negotiation Expansion Register
//#define
#define DM9KA_DSCR      0x16    //DAVICOM Specified Configuration Register
#define DM9KA_DSCSR     0x17    //DAVICOM Specified Configuration and Status Register
#define DM9KA_10BTCSR   0x18    //10BASE-T Configuration/Status
#define DM9KA_PWDOR     0x19    //Power Down Control Register
//#define
#define DM9KA_SCR       0x20    //Specified Configuration Register

/*******************************************************************
 * DM9000A EEPROM FORMAT
 * Operations:
 * 1)   Load DM9KA_EPAR
 *      +--Set DM9KA_EPAR<7:0> = one of the following address
 * 2a)  EEPROM Write
 *      +--DM9KA_EPCR = 0x12 for EEPROM Write?
 * 2b)  EEPROM Read
 *      +--Set DM9KA_EPCR = 0x04 for EEPROM Read
 *      +--Wait for completion (200us)
 *      +--Reset DM9KA_EPCR = 0x00 
 *      +--Read DM9KA_EPDRL and DM9KA_EPDRH with data
 *******************************************************************/
#define DM9KA_EP_MACADDR0   0x00    //MAC Address Byte 0 and Byte 1
#define DM9KA_EP_MACADDR1   0x01    //MAC Address Byte 2 and Byte 3
#define DM9KA_EP_MACADDR2   0x02    //MAC Address Byte 5 and Byte 6
#define DM9KA_EP_ALC        0x03    //Auto Load Control
#define DM9KA_EP_VID        0x04    //Vendor ID
#define DM9KA_EP_PID        0x05    //Product ID
#define DM9KA_EP_PC         0x06    //Pin Control
#define DM9KA_EP_WMC        0x07    //Wake-up Mode Control 

/*******************************************************************
 * TX and RX BUFFER DEFINITIONS
 * 1) Total RAM Size:       0x4000 = 16kB
 * 2) TX RAM Size:          0x0C00 =  3kB
 * 3) RX RAM Size:          0x3400 = 13kB
 * 4) Type:                 Circular
 * 
 * TX:
 * 1) Start Addr:           0x0000
 * 2) End Addr:             0x0BFF
 * 
 * RX:
 * 1) Start Addr:           0x0C00
 * 2) End Addr:             0x3FFF
 *******************************************************************/
/*
#define RAMSIZE            0x4000    
#define TXSTART            0x0000
#define TXSTOP             0x0BFF
#define TXSIZE             (TXSTOP-TXSTART+1)
#define RXSTART            0x0C00
#define RXSTOP             0x3FFF
#define RXSIZE             (RXSTOP-RXSTART+1)
*/

/*******************************************************************
 * Structure/enum declaration
 *******************************************************************/
/** Information for LAN Card */
typedef struct board_info 
{
  int io_addr;                //Register I/O base address: CMD = 0 for registers
  int io_data;                //Data I/O address: CMD = 1 for data
  int tx_pkt_cnt;             //Number of buffer available for tx 

  u8_t op_mode;               //PHY operation mode
  u8_t io_mode;               //0:word, 2:byte
} board_info_t;

enum DM9KA_PHY_mode 
{
  DM9KA_10MHD   = 0, 
  DM9KA_100MHD  = 1, 
  DM9KA_10MFD   = 4,
  DM9KA_100MFD  = 5, 
  DM9KA_AUTO    = 8, 
};

/*
 * 
 *
 * StatusVector

 */
/**
 * \brief Header for Ethernet packet
 * \remarks A header appended at the start of all RX frames by the hardware
 * \verbatim
   +--------+----------------------------------------+
   |  BIT   | RX PACKET STATUS                       |
   +--------+----------------------------------------+
   |   7    | RUNT: size < 64bytes                   |
   +--------+----------------------------------------+
   |   6    | MUITICAST                              |
   +--------+----------------------------------------+
   |   5    | LATE COLLISION                         |
   +--------+----------------------------------------+
   |   4    | RX WATCHDOG TIME OUT: size > 2048bytes |
   +--------+----------------------------------------+
   |   3    | PHYSICAL LAYER ERROR                   |
   +--------+----------------------------------------+
   |   2    | ALIGNMENT ERROR                        |
   +--------+----------------------------------------+
   |   1    | CRC ERROR                              |
   +--------+----------------------------------------+
   |   0    | FIFO OVERFLOW ERROR                    |
   +--------+----------------------------------------+
   \endverbatim
 */
typedef struct _DM_PREAMBLE
{
  u8_t  Validity;         //0x01 = valid packet,
  u8_t  StatusVector;
  u16_t PacketLength;     //MAC header + Data
} DM_PREAMBLE;


//------------------------------------------------------------------------------------------

/**
 * \brief Initialise dm9000a
 * \param flags accessing mode
 * \retval -1 error
 * \n         Ethernet is not linked (EACCES)
 * \n         no LAN card (ENXIO)
 * \retval 0  Ethernet is linked
 */
extern int dmfe_open (int flags);


/**
 * \brief close Ethernet connection
 * \retval 0 OK
 */
extern int dmfe_close (void);


/**
 * \brief copy the received packet to default buffer
 * \retval >0 indicating a packet is received
 * \retval 0 indicating no data is available
 * \retval -1 error
 * \n         Ethernet is not opened for read operation (errno = EBADF)
 * \n         Ethernet link is broken (errno =EIO)
 */
extern int dmfe_read (void);


/**
 * \brief copy count bytes of the data from buffer and transmit
 * \retval >=0 the number of bytes written
 * \retval -1 error
 * \n         Ethernet is not opened for writing (errno = EBADF)
 */
extern int dmfe_write (void);

#endif /* DM9000A_H_ */

/** @} */
/** @} */
