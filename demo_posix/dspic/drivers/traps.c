/************************************************************************************************
 * File:            traps.c
 * Description:     interrupt handler when dsPic encounters fatal errors
 ***********************************************************************************************/

#include <define.h>
#include <asm/delay.h>

/**********************************************************************
 * Osciallator Fail Interrupt
 **********************************************************************/
void _ISR _OscillatorFail(void)
{
    _OSCFAIL = 0;        //Clear the trap flag
}

/**********************************************************************
 * Address Interrupt
 **********************************************************************/
void _ISR _AddressError(void)
{
    _ADDRERR = 0;        //Clear the trap flag

    ERR_LED0_EN();
    
    while (1){
        ERR_LED0(1);
        mdelay(100); 
        ERR_LED0(0);
        mdelay(100); 
    }
}

/**********************************************************************
 * Starck Error Interrupt
 **********************************************************************/
void _ISR _StackError(void)
{
    _STKERR = 0;         //Clear the trap flag

    ERR_LED0_EN();

    ERR_LED0(1);
    while (1);
}

/**********************************************************************
 * Maths Interrupt
 **********************************************************************/
void _ISR _MathError(void)
{
    _MATHERR = 0;        //Clear the trap flag
    while (1){
        ERR_LED1(1);
        mdelay(100); 
        ERR_LED1(0);
        mdelay(100);
    }
}

#ifdef MPLAB_DSPIC33_PORT
/**********************************************************************
 * DMA Error Interrupt
 **********************************************************************/
void _ISR _DMACError(void)
{
    INTCON1bits.DMACERR = 0;        //Clear the trap flag

    ERR_LED1_EN();

    ERR_LED1(1);
    while (1);
}
#endif
