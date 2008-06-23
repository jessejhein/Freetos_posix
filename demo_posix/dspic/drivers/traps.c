/**
 * \file
 * Interrupt handler when dsPic encounters fatal errors
 * \author Dennis Tsang <dennis@amonics.com>
 */

#include <define.h>
#include <asm/delay.h>

/**********************************************************************
 * Osciallator Fail Interrupt
 **********************************************************************/
void _IRQ 
_OscillatorFail(void)
{
  _OSCFAIL = 0;        //Clear the trap flag
}

/**********************************************************************
 * Address Interrupt
 **********************************************************************/
void _IRQ 
_AddressError(void)
{
  _ADDRERR = 0;        //Clear the trap flag

  ERR_LED0_EN();
    
  while (1)
    {
      ERR_LED0(1);
      mdelay(100); 
      ERR_LED0(0);
      mdelay(100); 
    }
}

/**********************************************************************
 * Starck Error Interrupt
 **********************************************************************/
void _IRQ 
_StackError(void)
{
  _STKERR = 0;         //Clear the trap flag

  ERR_LED0_EN();

  ERR_LED0(1);
  while (1);
}

/**********************************************************************
 * Maths Interrupt
 **********************************************************************/
void _IRQ 
_MathError(void)
{
  _MATHERR = 0;        //Clear the trap flag
  while (1)
    {
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
void _IRQ 
_DMACError(void)
{
  INTCON1bits.DMACERR = 0;        //Clear the trap flag

  ERR_LED1_EN();

  ERR_LED1(1);
  while (1);
}
#endif /* MPLAB_DSPIC33_PORT */
