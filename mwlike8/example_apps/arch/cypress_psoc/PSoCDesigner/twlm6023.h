/*
 * header of twlm6023 
 */

// hardware - setting in this mic-P and circuit setting
//		PSoC mcu

#include "m8c.h"									// PSoC hardware's module(s) definition

#define enCsP                   PRT4DR &= ~0x08         				// low active P3.3
#define disCsP                  PRT4DR |= 0x08
#define lowA0P                  PRT4DR &= ~0x20         				// P3.5
#define highA0P                 PRT4DR |= 0x20
#define lowSClk                 PRT4DR &= ~0x80         				// P3.7
#define highSClk                PRT4DR |= 0x80
#define lowSD                   PRT2DR &= ~0x02         				// P2.1
#define highSD                  PRT2DR |= 0x02
#define	enResetP		PRT4DR &= ~0x02					// low active P3.1
#define	disResetP		PRT4DR |= 0x02

