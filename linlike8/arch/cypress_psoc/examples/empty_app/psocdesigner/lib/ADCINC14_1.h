//*****************************************************************************
//*****************************************************************************
//  FILENAME:  ADCINC14_1.h
//  Version: 1.3, Updated on 2005/03/06 at 17:14:19
//  Generated by PSoC Designer ver 4.2  b1013 : 02 September, 2004
//
//  DESCRIPTION:  ADCINC14 User Module C Language interface file for the
//                22/24/25/26/27xxx PSoC family of devices.
//-----------------------------------------------------------------------------
//      Copyright (c) Cypress MicroSystems 2000-2003. All Rights Reserved.
//*****************************************************************************
//*****************************************************************************

#include <m8C.h>

//-------------------------------------------------
// Defines for ADCINC14_1 API's.
//-------------------------------------------------

// Power Setting Constants
#define ADCINC14_1_OFF            0
#define ADCINC14_1_LOWPOWER       1
#define ADCINC14_1_MEDPOWER       2
#define ADCINC14_1_HIGHPOWER      3

// Function Fastcall Pragmas
#pragma fastcall16 ADCINC14_1_Start
#pragma fastcall16 ADCINC14_1_SetPower
#pragma fastcall16 ADCINC14_1_GetSamples
#pragma fastcall16 ADCINC14_1_StopAD
#pragma fastcall16 ADCINC14_1_Stop
#pragma fastcall16 ADCINC14_1_fIsData
#pragma fastcall16 ADCINC14_1_fIsDataAvailable
#pragma fastcall16 ADCINC14_1_iGetData
#pragma fastcall16 ADCINC14_1_iGetDataClearFlag
#pragma fastcall16 ADCINC14_1_ClearFlag

//-------------------------------------------------
// Prototypes of the ADCINC14_1 API.
//-------------------------------------------------

extern void ADCINC14_1_Start(BYTE bPower);
extern void ADCINC14_1_SetPower(BYTE bPower);
extern void ADCINC14_1_GetSamples(BYTE bNumSamples);
extern void ADCINC14_1_StopAD(void);
extern void ADCINC14_1_Stop(void);
extern CHAR ADCINC14_1_fIsData(void);
extern CHAR ADCINC14_1_fIsDataAvailable(void);
extern INT  ADCINC14_1_iGetData(void);
extern INT  ADCINC14_1_iGetDataClearFlag(void);
extern void ADCINC14_1_ClearFlag(void);

//-------------------------------------------------
// Hardware Register Definitions
//-------------------------------------------------

// Integrator Block Register Definitions
#pragma ioport  ADCINC14_1_bfAtoDcr0:   0x088
BYTE            ADCINC14_1_bfAtoDcr0;
#pragma ioport  ADCINC14_1_bfAtoDcr1:   0x089
BYTE            ADCINC14_1_bfAtoDcr1;
#pragma ioport  ADCINC14_1_bfAtoDcr2:   0x08a
BYTE            ADCINC14_1_bfAtoDcr2;
#pragma ioport  ADCINC14_1_bfAtoDcr3:   0x08b
BYTE            ADCINC14_1_bfAtoDcr3;

// Counter Block Register Definitions
#pragma ioport  ADCINC14_1_fCounterFN:  0x120
BYTE            ADCINC14_1_fCounterFN;
#pragma ioport  ADCINC14_1_fCounterSL:  0x121
BYTE            ADCINC14_1_fCounterSL;
#pragma ioport  ADCINC14_1_fCounterOS:  0x122
BYTE            ADCINC14_1_fCounterOS;
#pragma ioport  ADCINC14_1_bCount:  0x020
BYTE            ADCINC14_1_bCount;
#pragma ioport  ADCINC14_1_bPeriod: 0x021
BYTE            ADCINC14_1_bPeriod;
#pragma ioport  ADCINC14_1_bCompare:    0x022
BYTE            ADCINC14_1_bCompare;
#pragma ioport  ADCINC14_1_bCounter_CR0:    0x023
BYTE            ADCINC14_1_bCounter_CR0;


// PWM24 Block Register Definitions
#pragma ioport  ADCINC14_1_bfPWM_LSB_FN:    0x12c
BYTE            ADCINC14_1_bfPWM_LSB_FN;
#pragma ioport  ADCINC14_1_bfPWM_ISB_FN:    0x130
BYTE            ADCINC14_1_bfPWM_ISB_FN;
#pragma ioport  ADCINC14_1_bfPWM_MSB_FN:    0x134
BYTE            ADCINC14_1_bfPWM_MSB_FN;
#pragma ioport  ADCINC14_1_fPWM_LSB_CR0:    0x02f
BYTE            ADCINC14_1_fPWM_LSB_CR0;
#pragma ioport  ADCINC14_1_fPWM_ISB_CR0:    0x033
BYTE            ADCINC14_1_fPWM_ISB_CR0;
#pragma ioport  ADCINC14_1_fPWM_MSB_CR0:    0x037
BYTE            ADCINC14_1_fPWM_MSB_CR0;
#pragma ioport  ADCINC14_1_bPWM_Count_MSB:  0x034
BYTE            ADCINC14_1_bPWM_Count_MSB;
#pragma ioport  ADCINC14_1_bPWM_Count_LSB:  0x02c
BYTE            ADCINC14_1_bPWM_Count_LSB;
#pragma ioport  ADCINC14_1_bPWM_Count_ISB:  0x030
BYTE            ADCINC14_1_bPWM_Count_ISB;
#pragma ioport  ADCINC14_1_bPWM_Period_MSB: 0x035
BYTE            ADCINC14_1_bPWM_Period_MSB;
#pragma ioport  ADCINC14_1_bPWM_Period_LSB: 0x02d
BYTE            ADCINC14_1_bPWM_Period_LSB;
#pragma ioport  ADCINC14_1_bPWM_Period_ISB: 0x031
BYTE            ADCINC14_1_bPWM_Period_ISB;
#pragma ioport  ADCINC14_1_bPWM_IntTime_LSB:    0x02e
BYTE            ADCINC14_1_bPWM_IntTime_LSB;
#pragma ioport  ADCINC14_1_bPWM_IntTime_ISB:    0x032
BYTE            ADCINC14_1_bPWM_IntTime_ISB;
#pragma ioport  ADCINC14_1_bPWM_IntTime_MSB:    0x036
BYTE            ADCINC14_1_bPWM_IntTime_MSB;
#pragma ioport  ADCINC14_1_bfPWM_LSB_FN:    0x12c
BYTE            ADCINC14_1_bfPWM_LSB_FN;
#pragma ioport  ADCINC14_1_bfPWM_ISB_FN:    0x130
BYTE            ADCINC14_1_bfPWM_ISB_FN;
#pragma ioport  ADCINC14_1_bfPWM_MSB_FN:    0x134
BYTE            ADCINC14_1_bfPWM_MSB_FN;

// end of file ADCINC14_1.h
