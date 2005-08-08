//*****************************************************************************
//*****************************************************************************
//  FILENAME:  UART_1.h
//  Version: 5.2, Updated on 2005/01/10 at 15:23:03
//  Generated by PSoC Designer ver 4.2  b1013 : 02 September, 2004
//
//  DESCRIPTION:  UART User Module C Language interface file for the
//                22/24/25/26/27xxx PSoC family of devices.
//-----------------------------------------------------------------------------
//      Copyright (c) Cypress MicroSystems 2000-2003. All Rights Reserved.
//*****************************************************************************
//*****************************************************************************

// include the global header file
#include <m8c.h>

#define UART_1_RXBUF_ENABLE 0

//-------------------------------------------------
// Prototypes of the UART_1 API.
//-------------------------------------------------

#if ( UART_1_RXBUF_ENABLE )
extern char UART_1_aRxBuffer[];
extern BYTE UART_1_bRxCnt;
extern BYTE UART_1_fStatus;
#endif



// Create pragmas to support proper argument and return value passing

#pragma fastcall16  UART_1_SetTxIntMode
#pragma fastcall16  UART_1_EnableInt
#pragma fastcall16  UART_1_DisableInt
#pragma fastcall16  UART_1_Start
#pragma fastcall16  UART_1_Stop
#pragma fastcall16  UART_1_SendData
#pragma fastcall16  UART_1_bReadTxStatus
#pragma fastcall16  UART_1_bReadRxData
#pragma fastcall16  UART_1_bReadRxStatus
#pragma fastcall16  UART_1_PutSHexByte
#pragma fastcall16  UART_1_PutSHexInt
#pragma fastcall16  UART_1_CPutString
#pragma fastcall16  UART_1_PutString
#pragma fastcall16  UART_1_PutChar
#pragma fastcall16  UART_1_Write
#pragma fastcall16  UART_1_CWrite

#pragma fastcall16  UART_1_cGetChar
#pragma fastcall16  UART_1_cReadChar
#pragma fastcall16  UART_1_iReadChar
#pragma fastcall16  UART_1_IntCntl
#pragma fastcall16  UART_1_TxIntMode
#pragma fastcall16  UART_1_PutCRLF

#if ( UART_1_RXBUF_ENABLE )
#pragma fastcall16  UART_1_CmdReset
#pragma fastcall16  UART_1_bCmdCheck
#pragma fastcall16  UART_1_bErrCheck
#pragma fastcall16  UART_1_bCmdLength
#pragma fastcall16  UART_1_szGetParam
#pragma fastcall16  UART_1_szGetRestOfParams
#endif

//**************************************************
// Prototypes of UART API. For a definition of
// functions see UART_1.inc.
//**************************************************
extern void  UART_1_SetTxIntMode(BYTE bTxIntMode);
extern void  UART_1_EnableInt(void);
extern void  UART_1_DisableInt(void);
extern void  UART_1_Start(BYTE bParitySetting);
extern void  UART_1_Stop(void);
extern void  UART_1_SendData(BYTE bTxData);
extern BYTE  UART_1_bReadTxStatus(void);
extern BYTE  UART_1_bReadRxData(void);
extern BYTE  UART_1_bReadRxStatus(void);

// High level TX functions
extern void   UART_1_CPutString(const BYTE * szRomString);
extern void   UART_1_PutString(BYTE * szRamString);
extern void   UART_1_PutChar(CHAR cData);
extern void   UART_1_Write(BYTE * szRamString, BYTE bCount);
extern void   UART_1_CWrite(const BYTE * szRomString, INT iCount);
extern void   UART_1_PutSHexByte(BYTE bValue);
extern void   UART_1_PutSHexInt(INT iValue);
extern void   UART_1_PutCRLF(void);
extern void   UART_1_TxIntMode(BYTE bMask);

// High level RX functions
extern CHAR         UART_1_cGetChar(void);
extern CHAR         UART_1_cReadChar(void);
extern INT          UART_1_iReadChar(void);

extern void         UART_1_IntCntl(BYTE bMask);

#if ( UART_1_RXBUF_ENABLE )
extern void   UART_1_CmdReset(void);
extern BYTE   UART_1_bCmdCheck(void);
extern BYTE   UART_1_bErrCheck(void);
extern BYTE   UART_1_bCmdLength(void);
extern BYTE * UART_1_szGetParam(void);
extern BYTE * UART_1_szGetRestOfParams(void);
#endif

//-------------------------------------------------
// Defines for UART_1 API's.
//-------------------------------------------------

//------------------------------------
// Transmitter Interrupt masks
//------------------------------------
#define UART_1_TX_INT_MASK:                        (0x40)
#define UART_1_TX_INT_REG:                         (0x0e1) 
#define UART_1_RX_INT_MASK:                        (0x04) 
#define UART_1_RX_INT_REG:                         (0x0e1) 

//------------------------------------
//  Parity masks
//------------------------------------
#define  UART_1_PARITY_NONE        0x00
#define  UART_1_PARITY_EVEN        0x02
#define  UART_1_PARITY_ODD         0x06
//------------------------------------
//  Transmitter Status Register masks
//------------------------------------
#define  UART_1_TX_COMPLETE        0x20
#define  UART_1_TX_BUFFER_EMPTY    0x10

//------------------------------------
//  Receiver Status Register masks
//------------------------------------
#define  UART_1_RX_ACTIVE          0x10
#define  UART_1_RX_COMPLETE        0x08
#define  UART_1_RX_REG_FULL        0x08
#define  UART_1_RX_PARITY_ERROR    0x80
#define  UART_1_RX_OVERRUN_ERROR   0x40
#define  UART_1_RX_FRAMING_ERROR   0x20
#define  UART_1_RX_ERROR           0xE0
#define  UART_1_RX_NO_ERROR        0xE0          // This symbol is deprecated and will removed in the future

#define  UART_1_RX_NO_DATA         0x01

#define  UART_1_RX_BUF_ERROR              0xF0  // Mask for any Rx that may occur.
#define  UART_1_RX_BUF_OVERRUN            0x10  // This indicates the software buffer has
                                                           // been over run.
#define  UART_1_RX_BUF_CMDTERM            0x01  // Command terminator has been received.


//-------------------------------------------------
// Register Address Constants for UART_1
//-------------------------------------------------

#pragma ioport  UART_1_TX_CONTROL_REG:  0x03b               // Control register
BYTE            UART_1_TX_CONTROL_REG;
#pragma ioport  UART_1_TX_SHIFT_REG:    0x038               // TX Shift Register register
BYTE            UART_1_TX_SHIFT_REG;
#pragma ioport  UART_1_TX_BUFFER_REG:   0x039               // TX Buffer Register
BYTE            UART_1_TX_BUFFER_REG;
#pragma ioport  UART_1_TX_FUNC_REG: 0x138                   // Function register
BYTE            UART_1_TX_FUNC_REG;
#pragma ioport  UART_1_TX_INPUT_REG:    0x139               // Input register
BYTE            UART_1_TX_INPUT_REG;
#pragma ioport  UART_1_TX_OUTPUT_REG:   0x13a               // Output register
BYTE            UART_1_TX_OUTPUT_REG;
#pragma ioport  UART_1_RX_CONTROL_REG:  0x02b               // Control register
BYTE            UART_1_RX_CONTROL_REG;
#pragma ioport  UART_1_RX_SHIFT_REG:    0x028               // RX Shift Register register
BYTE            UART_1_RX_SHIFT_REG;
#pragma ioport  UART_1_RX_BUFFER_REG:   0x02a               // RX Buffer Register
BYTE            UART_1_RX_BUFFER_REG;
#pragma ioport  UART_1_RX_FUNC_REG: 0x128                   // Function register
BYTE            UART_1_RX_FUNC_REG;
#pragma ioport  UART_1_RX_INPUT_REG:    0x129               // Input register
BYTE            UART_1_RX_INPUT_REG;
#pragma ioport  UART_1_RX_OUTPUT_REG:   0x12a               // Output register
BYTE            UART_1_RX_OUTPUT_REG;

// Interrupt control
#define UART_1_ENABLE_RX_INT  0x01
#define UART_1_ENABLE_TX_INT  0x02
#define UART_1_DISABLE_RX_INT 0x00
#define UART_1_DISABLE_TX_INT 0x00

#define UART_1_INT_MODE_TX_REG_EMPTY 0x00
#define UART_1_INT_MODE_TX_COMPLETE  0x01





//-------------------------------------------
//       WARNING WARNING WARNING
// The following defines and function prototypes
// are for backwards compatibility only and
// should not be used for new designs.
//-------------------------------------------
#pragma fastcall16  bUART_1_ReadTxStatus
#pragma fastcall16  bUART_1_ReadRxData
#pragma fastcall16  bUART_1_ReadRxStatus
extern BYTE  bUART_1_ReadTxStatus(void);
extern BYTE  bUART_1_ReadRxData(void);
extern BYTE  bUART_1_ReadRxStatus(void);
//------------------------------------
//  Parity masks
//------------------------------------
#define  UART_PARITY_NONE        0x00
#define  UART_PARITY_EVEN        0x02
#define  UART_PARITY_ODD         0x06

//------------------------------------
//  Transmitter Status Register masks
//------------------------------------
#define  UART_TX_COMPLETE        0x20
#define  UART_TX_BUFFER_EMPTY    0x10

//------------------------------------
//  Receiver Status Register masks
//------------------------------------
#define  UART_RX_ACTIVE          0x10
#define  UART_RX_COMPLETE        0x08
#define  UART_RX_PARITY_ERROR    0x80
#define  UART_RX_OVERRUN_ERROR   0x40
#define  UART_RX_FRAMING_ERROR   0x20
#define  UART_RX_NO_ERROR        0xE0

//-------------------------------------------
//             END WARNING
//-------------------------------------------
// end of file UART_1.h
