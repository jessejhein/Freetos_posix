#line 1 "./irq.c"

#line 7 "./irq.c"

#line 1 "./system.h"

#line 9 "./system.h"

#line 1 "C:/PROGRA~1/CYPRES~2/PSOCDE~1/tools/include/CY8C27~1/m8c.h"




















typedef  unsigned char  BOOL;
typedef  unsigned char  BYTE;
typedef  signed   char  CHAR;
typedef  unsigned int   WORD;
typedef  signed   int   INT;
typedef  unsigned long  DWORD;
typedef  signed   long  LONG;



































#pragma  ioport   PRT0DR:     0x000
BYTE              PRT0DR;
#pragma  ioport   PRT0IE:     0x001
BYTE              PRT0IE;
#pragma  ioport   PRT0GS:     0x002
BYTE              PRT0GS;
#pragma  ioport   PRT0DM2:    0x003
BYTE              PRT0DM2;

#pragma  ioport   PRT1DR:     0x004
BYTE              PRT1DR;
#pragma  ioport   PRT1IE:     0x005
BYTE              PRT1IE;
#pragma  ioport   PRT1GS:     0x006
BYTE              PRT1GS;
#pragma  ioport   PRT1DM2:    0x007
BYTE              PRT1DM2;

#pragma  ioport   PRT2DR:     0x008
BYTE              PRT2DR;
#pragma  ioport   PRT2IE:     0x009
BYTE              PRT2IE;
#pragma  ioport   PRT2GS:     0x00A
BYTE              PRT2GS;
#pragma  ioport   PRT2DM2:    0x00B
BYTE              PRT2DM2;

#pragma  ioport   PRT3DR:     0x00C
BYTE              PRT3DR;
#pragma  ioport   PRT3IE:     0x00D
BYTE              PRT3IE;
#pragma  ioport   PRT3GS:     0x00E
BYTE              PRT3GS;
#pragma  ioport   PRT3DM2:    0x00F
BYTE              PRT3DM2;

#pragma  ioport   PRT4DR:     0x010
BYTE              PRT4DR;
#pragma  ioport   PRT4IE:     0x011
BYTE              PRT4IE;
#pragma  ioport   PRT4GS:     0x012
BYTE              PRT4GS;
#pragma  ioport   PRT4DM2:    0x013
BYTE              PRT4DM2;

#pragma  ioport   PRT5DR:     0x014
BYTE              PRT5DR;
#pragma  ioport   PRT5IE:     0x015
BYTE              PRT5IE;
#pragma  ioport   PRT5GS:     0x016
BYTE              PRT5GS;
#pragma  ioport   PRT5DM2:    0x017
BYTE              PRT5DM2;







#pragma  ioport   DBB00DR0:   0x020
BYTE              DBB00DR0;
#pragma  ioport   DBB00DR1:   0x021
BYTE              DBB00DR1;
#pragma  ioport   DBB00DR2:   0x022
BYTE              DBB00DR2;
#pragma  ioport   DBB00CR0:   0x023
BYTE              DBB00CR0;


#pragma  ioport   DBB01DR0:   0x024
BYTE              DBB01DR0;
#pragma  ioport   DBB01DR1:   0x025
BYTE              DBB01DR1;
#pragma  ioport   DBB01DR2:   0x026
BYTE              DBB01DR2;
#pragma  ioport   DBB01CR0:   0x027
BYTE              DBB01CR0;


#pragma  ioport   DCB02DR0:   0x028
BYTE              DCB02DR0;
#pragma  ioport   DCB02DR1:   0x029
BYTE              DCB02DR1;
#pragma  ioport   DCB02DR2:   0x02A
BYTE              DCB02DR2;
#pragma  ioport   DCB02CR0:   0x02B
BYTE              DCB02CR0;


#pragma  ioport   DCB03DR0:   0x02C
BYTE              DCB03DR0;
#pragma  ioport   DCB03DR1:   0x02D
BYTE              DCB03DR1;
#pragma  ioport   DCB03DR2:   0x02E
BYTE              DCB03DR2;
#pragma  ioport   DCB03CR0:   0x02F
BYTE              DCB03CR0;


#pragma  ioport   DBB10DR0:   0x030
BYTE              DBB10DR0;
#pragma  ioport   DBB10DR1:   0x031
BYTE              DBB10DR1;
#pragma  ioport   DBB10DR2:   0x032
BYTE              DBB10DR2;
#pragma  ioport   DBB10CR0:   0x033
BYTE              DBB10CR0;


#pragma  ioport   DBB11DR0:   0x034
BYTE              DBB11DR0;
#pragma  ioport   DBB11DR1:   0x035
BYTE              DBB11DR1;
#pragma  ioport   DBB11DR2:   0x036
BYTE              DBB11DR2;
#pragma  ioport   DBB11CR0:   0x037
BYTE              DBB11CR0;


#pragma  ioport   DCB12DR0:   0x038
BYTE              DCB12DR0;
#pragma  ioport   DCB12DR1:   0x039
BYTE              DCB12DR1;
#pragma  ioport   DCB12DR2:   0x03A
BYTE              DCB12DR2;
#pragma  ioport   DCB12CR0:   0x03B
BYTE              DCB12CR0;


#pragma  ioport   DCB13DR0:   0x03C
BYTE              DCB13DR0;
#pragma  ioport   DCB13DR1:   0x03D
BYTE              DCB13DR1;
#pragma  ioport   DCB13DR2:   0x03E
BYTE              DCB13DR2;
#pragma  ioport   DCB13CR0:   0x03F
BYTE              DCB13CR0;





#pragma  ioport   AMX_IN:     0x060
BYTE              AMX_IN;





#pragma  ioport   ARF_CR:     0x063
BYTE              ARF_CR;







#pragma  ioport   CMP_CR0:     0x064
BYTE              CMP_CR0;









#pragma  ioport   ASY_CR:     0x065
BYTE              ASY_CR;





#pragma  ioport  CMP_CR1:     0x66
BYTE             CMP_CR1;













#pragma  ioport   ACB00CR3:   0x070
BYTE              ACB00CR3;
#pragma  ioport   ACB00CR0:   0x071
BYTE              ACB00CR0;
#pragma  ioport   ACB00CR1:   0x072
BYTE              ACB00CR1;
#pragma  ioport   ACB00CR2:   0x073
BYTE              ACB00CR2;


#pragma  ioport   ACB01CR3:   0x074
BYTE              ACB01CR3;
#pragma  ioport   ACB01CR0:   0x075
BYTE              ACB01CR0;
#pragma  ioport   ACB01CR1:   0x076
BYTE              ACB01CR1;
#pragma  ioport   ACB01CR2:   0x077
BYTE              ACB01CR2;


#pragma  ioport   ACB02CR3:   0x078
BYTE              ACB02CR3;
#pragma  ioport   ACB02CR0:   0x079
BYTE              ACB02CR0;
#pragma  ioport   ACB02CR1:   0x07A
BYTE              ACB02CR1;
#pragma  ioport   ACB02CR2:   0x07B
BYTE              ACB02CR2;


#pragma  ioport   ACB03CR3:   0x07C
BYTE              ACB03CR3;
#pragma  ioport   ACB03CR0:   0x07D
BYTE              ACB03CR0;
#pragma  ioport   ACB03CR1:   0x07E
BYTE              ACB03CR1;
#pragma  ioport   ACB03CR2:   0x07F
BYTE              ACB03CR2;


#pragma  ioport   ASC10CR0:   0x080
BYTE              ASC10CR0;
#pragma  ioport   ASC10CR1:   0x081
BYTE              ASC10CR1;
#pragma  ioport   ASC10CR2:   0x082
BYTE              ASC10CR2;
#pragma  ioport   ASC10CR3:   0x083
BYTE              ASC10CR3;


#pragma  ioport   ASD11CR0:   0x084
BYTE              ASD11CR0;
#pragma  ioport   ASD11CR1:   0x085
BYTE              ASD11CR1;
#pragma  ioport   ASD11CR2:   0x086
BYTE              ASD11CR2;
#pragma  ioport   ASD11CR3:   0x087
BYTE              ASD11CR3;


#pragma  ioport   ASC12CR0:   0x088
BYTE              ASC12CR0;
#pragma  ioport   ASC12CR1:   0x089
BYTE              ASC12CR1;
#pragma  ioport   ASC12CR2:   0x08A
BYTE              ASC12CR2;
#pragma  ioport   ASC12CR3:   0x08B
BYTE              ASC12CR3;


#pragma  ioport   ASD13CR0:   0x08C
BYTE              ASD13CR0;
#pragma  ioport   ASD13CR1:   0x08D
BYTE              ASD13CR1;
#pragma  ioport   ASD13CR2:   0x08E
BYTE              ASD13CR2;
#pragma  ioport   ASD13CR3:   0x08F
BYTE              ASD13CR3;


#pragma  ioport   ASD20CR0:   0x090
BYTE              ASD20CR0;
#pragma  ioport   ASD20CR1:   0x091
BYTE              ASD20CR1;
#pragma  ioport   ASD20CR2:   0x092
BYTE              ASD20CR2;
#pragma  ioport   ASD20CR3:   0x093
BYTE              ASD20CR3;


#pragma  ioport   ASC21CR0:   0x094
BYTE              ASC21CR0;
#pragma  ioport   ASC21CR1:   0x095
BYTE              ASC21CR1;
#pragma  ioport   ASC21CR2:   0x096
BYTE              ASC21CR2;
#pragma  ioport   ASC21CR3:   0x097
BYTE              ASC21CR3;


#pragma  ioport   ASD22CR0:   0x098
BYTE              ASD22CR0;
#pragma  ioport   ASD22CR1:   0x099
BYTE              ASD22CR1;
#pragma  ioport   ASD22CR2:   0x09A
BYTE              ASD22CR2;
#pragma  ioport   ASD22CR3:   0x09B
BYTE              ASD22CR3;


#pragma  ioport   ASC23CR0:   0x09C
BYTE              ASC23CR0;
#pragma  ioport   ASC23CR1:   0x09D
BYTE              ASC23CR1;
#pragma  ioport   ASC23CR2:   0x09E
BYTE              ASC23CR2;
#pragma  ioport   ASC23CR3:   0x09F
BYTE              ASC23CR3;








#pragma  ioport   RDI0RI:     0x0B0
BYTE              RDI0RI;
#pragma  ioport   RDI0SYN:    0x0B1
BYTE              RDI0SYN;
#pragma  ioport   RDI0IS:     0x0B2
BYTE              RDI0IS;
#pragma  ioport   RDI0LT0:    0x0B3
BYTE              RDI0LT0;
#pragma  ioport   RDI0LT1:    0x0B4
BYTE              RDI0LT1;
#pragma  ioport   RDI0RO0:    0x0B5
BYTE              RDI0RO0;
#pragma  ioport   RDI0RO1:    0x0B6
BYTE              RDI0RO1;

#pragma  ioport   RDI1RI:     0x0B8
BYTE              RDI1RI;
#pragma  ioport   RDI1SYN:    0x0B9
BYTE              RDI1SYN;
#pragma  ioport   RDI1IS:     0x0BA
BYTE              RDI1IS;
#pragma  ioport   RDI1LT0:    0x0BB
BYTE              RDI1LT0;
#pragma  ioport   RDI1LT1:    0x0BC
BYTE              RDI1LT1;
#pragma  ioport   RDI1RO0:    0x0BD
BYTE              RDI1RO0;
#pragma  ioport   RDI1RO1:    0x0BE
BYTE              RDI1RO1;




#pragma  ioport   I2C_CFG:    0x0D6
BYTE              I2C_CFG;











#pragma  ioport   I2C_SCR:    0x0D7
BYTE              I2C_SCR;









#pragma  ioport   I2C_DR:     0x0D8
BYTE              I2C_DR;

#pragma  ioport   I2C_MSCR:   0x0D9
BYTE              I2C_MSCR;








#pragma  ioport   INT_CLR0:   0x0DA
BYTE              INT_CLR0;
#pragma  ioport   INT_CLR1:   0x0DB
BYTE              INT_CLR1;
#pragma  ioport   INT_CLR3:   0x0DD
BYTE              INT_CLR3;

#pragma  ioport   INT_MSK3:   0x0DE
BYTE              INT_MSK3;



#pragma  ioport   INT_MSK0:   0x0E0
BYTE              INT_MSK0;









#pragma  ioport   INT_MSK1:   0x0E1
BYTE              INT_MSK1;









#pragma  ioport   INT_VC:     0x0E2
BYTE              INT_VC;
#pragma  ioport   RES_WDT:    0x0E3
BYTE              RES_WDT;


#pragma  ioport   DEC_DH:     0x0E4
CHAR              DEC_DH;
#pragma  ioport   DEC_DL:     0x0E5
CHAR              DEC_DL;
#pragma  ioport   DEC_CR0:    0x0E6
BYTE              DEC_CR0;
#pragma  ioport   DEC_CR1:    0x0E7
BYTE              DEC_CR1;


#pragma  ioport   MUL_X:         0x0E8
CHAR              MUL_X;
#pragma  ioport   MUL_Y:         0x0E9
CHAR              MUL_Y;
#pragma  ioport   MUL_DH:        0x0EA
CHAR              MUL_DH;
#pragma  ioport   MUL_DL:        0x0EB
CHAR              MUL_DL;
#pragma  ioport   MUL_RESULT:    0x0EA
INT               MUL_RESULT;
#pragma  ioport   MAC_X:         0x0EC
CHAR              MAC_X;
#pragma  ioport   MAC_Y:         0x0ED
CHAR              MAC_Y;
#pragma  ioport   MAC_CL0:       0x0EE
BYTE              MAC_CL0;
#pragma  ioport   MAC_CL1:       0x0EF
BYTE              MAC_CL1;
#pragma  ioport   ACC_DR1:       0x0EC
CHAR              ACC_DR1;
#pragma  ioport   ACC_DR0:       0x0ED
CHAR              ACC_DR0;
#pragma  ioport   ACC_LOW_WORD:  0x0EC
INT               ACC_LOW_WORD;
#pragma  ioport   ACC_DR3:       0x0EE
CHAR              ACC_DR3;
#pragma  ioport   ACC_DR2:       0x0EF
CHAR              ACC_DR2;
#pragma  ioport   ACC_HI_WORD:   0x0EE
INT               ACC_HI_WORD;







#pragma  ioport   CPU_SCR1:   0xFE
BYTE              CPU_SCR1;




#pragma  ioport   CPU_SCR0:   0x0FF
BYTE              CPU_SCR0;



















#pragma  ioport   PRT0DM0:    0x100
BYTE              PRT0DM0;
#pragma  ioport   PRT0DM1:    0x101
BYTE              PRT0DM1;
#pragma  ioport   PRT0IC0:    0x102
BYTE              PRT0IC0;
#pragma  ioport   PRT0IC1:    0x103
BYTE              PRT0IC1;

#pragma  ioport   PRT1DM0:    0x104
BYTE              PRT1DM0;
#pragma  ioport   PRT1DM1:    0x105
BYTE              PRT1DM1;
#pragma  ioport   PRT1IC0:    0x106
BYTE              PRT1IC0;
#pragma  ioport   PRT1IC1:    0x107
BYTE              PRT1IC1;

#pragma  ioport   PRT2DM0:    0x108
BYTE              PRT2DM0;
#pragma  ioport   PRT2DM1:    0x109
BYTE              PRT2DM1;
#pragma  ioport   PRT2IC0:    0x10A
BYTE              PRT2IC0;
#pragma  ioport   PRT2IC1:    0x10B
BYTE              PRT2IC1;

#pragma  ioport   PRT3DM0:    0x10C
BYTE              PRT3DM0;
#pragma  ioport   PRT3DM1:    0x10D
BYTE              PRT3DM1;
#pragma  ioport   PRT3IC0:    0x10E
BYTE              PRT3IC0;
#pragma  ioport   PRT3IC1:    0x10F
BYTE              PRT3IC1;

#pragma  ioport   PRT4DM0:    0x110
BYTE              PRT4DM0;
#pragma  ioport   PRT4DM1:    0x111
BYTE              PRT4DM1;
#pragma  ioport   PRT4IC0:    0x112
BYTE              PRT4IC0;
#pragma  ioport   PRT4IC1:    0x113
BYTE              PRT4IC1;

#pragma  ioport   PRT5DM0:    0x114
BYTE              PRT5DM0;
#pragma  ioport   PRT5DM1:    0x115
BYTE              PRT5DM1;
#pragma  ioport   PRT5IC0:    0x116
BYTE              PRT5IC0;
#pragma  ioport   PRT5IC1:    0x117
BYTE              PRT5IC1;







#pragma  ioport   DBB00FN:    0x120
BYTE              DBB00FN;
#pragma  ioport   DBB00IN:    0x121
BYTE              DBB00IN;
#pragma  ioport   DBB00OU:    0x122
BYTE              DBB00OU;


#pragma  ioport   DBB01FN:    0x124
BYTE              DBB01FN;
#pragma  ioport   DBB01IN:    0x125
BYTE              DBB01IN;
#pragma  ioport   DBB01OU:    0x126
BYTE              DBB01OU;


#pragma  ioport   DCB02FN:    0x128
BYTE              DCB02FN;
#pragma  ioport   DCB02IN:    0x129
BYTE              DCB02IN;
#pragma  ioport   DCB02OU:    0x12A
BYTE              DCB02OU;


#pragma  ioport   DCB03FN:    0x12C
BYTE              DCB03FN;
#pragma  ioport   DCB03IN:    0x12D
BYTE              DCB03IN;
#pragma  ioport   DCB03OU:    0x12E
BYTE              DCB03OU;


#pragma  ioport   DBB10FN:    0x130
BYTE              DBB10FN;
#pragma  ioport   DBB10IN:    0x131
BYTE              DBB10IN;
#pragma  ioport   DBB10OU:    0x132
BYTE              DBB10OU;


#pragma  ioport   DBB11FN:    0x134
BYTE              DBB11FN;
#pragma  ioport   DBB11IN:    0x135
BYTE              DBB11IN;
#pragma  ioport   DBB11OU:    0x136
BYTE              DBB11OU;


#pragma  ioport   DCB12FN:    0x138
BYTE              DCB12FN;
#pragma  ioport   DCB12IN:    0x139
BYTE              DCB12IN;
#pragma  ioport   DCB12OU:    0x13A
BYTE              DCB12OU;


#pragma  ioport   DCB13FN:    0x13C
BYTE              DCB13FN;
#pragma  ioport   DCB13IN:    0x13D
BYTE              DCB13IN;
#pragma  ioport   DCB13OU:    0x13E
BYTE              DCB13OU;







#pragma  ioport   CLK_CR0:    0x160
BYTE              CLK_CR0;





#pragma  ioport   CLK_CR1:    0x161
BYTE              CLK_CR1;




#pragma  ioport   ABF_CR0:    0x162
BYTE              ABF_CR0;









#pragma  ioport   AMD_CR0:    0x163
BYTE              AMD_CR0;



#pragma  ioport   AMD_CR1:    0x166
BYTE              AMD_CR1;



#pragma  ioport   ALT_CR0:    0x167
BYTE              ALT_CR0;



#pragma  ioport   ALT_CR1:    0x168
BYTE              ALT_CR1;



#pragma  ioport   CLK_CR2:    0x169
BYTE              CLK_CR2;







#pragma  ioport   GDI_O_IN:   0x1D0
BYTE              GDI_O_IN;
#pragma  ioport   GDI_E_IN:   0x1D1
BYTE              GDI_E_IN;
#pragma  ioport   GDI_O_OU:   0x1D2
BYTE              GDI_O_OU;
#pragma  ioport   GDI_E_OU:   0x1D3
BYTE              GDI_E_OU;





#pragma  ioport   OSC_CR4:    0x1DE
BYTE              OSC_CR4;





#pragma  ioport   OSC_CR3:    0x1DF
BYTE              OSC_CR3;

#pragma  ioport   OSC_CR0:    0x1E0
BYTE              OSC_CR0;


















#pragma  ioport   OSC_CR1:    0x1E1
BYTE              OSC_CR1;



#pragma  ioport   OSC_CR2:    0x1E2
BYTE              OSC_CR2;




#pragma  ioport   VLT_CR:     0x1E3
BYTE              VLT_CR;









#pragma  ioport   VLT_CMP:    0x1E4
BYTE              VLT_CMP;




#pragma  ioport   IMO_TR:     0x1E8
BYTE              IMO_TR;
#pragma  ioport   ILO_TR:     0x1E9
BYTE              ILO_TR;
#pragma  ioport   BDG_TR:     0x1EA
BYTE              BDG_TR;
#pragma  ioport   ECO_TR:     0x1EB
BYTE              ECO_TR;





































































#line 896 "C:/PROGRA~1/CYPRES~2/PSOCDE~1/tools/include/CY8C27~1/m8c.h"
















#line 11 "./system.h"





#line 31 "./system.h"








#line 45 "./system.h"


#line 50 "./system.h"











#line 9 "./irq.c"
#line 1 "./irq_cpustat.h"

#line 7 "./irq_cpustat.h"

extern irq_cpustat_t irq_stat[];


#line 10 "./irq.c"
#line 1 "./interrupt.h"

#line 11 "./interrupt.h"

extern unsigned char softirq_vec;

enum {
	TIMER_BH = 0
};

extern void do_softirq(void);

static void mark_bh(unsigned char nr)
{
	softirq_vec = (0x01 << nr);
}

#line 11 "./irq.c"






void do_IRQ(void)
{
 asm("or  F, 01h");
	if (softirq_pending())
		do_softirq();
}
