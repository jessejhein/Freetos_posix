/**
 * \file
 * Interrupt handler when dsPic encounters fatal errors
 * \author Dennis Tsang <dennis@amonics.com>
 */

#include <define.h>
#include <asm/delay.h>
#include <asm/types.h>
#include <unistd.h>

/** variables to store program counter: order matters */
__u16 StkAddrLo;
__u16 StkAddrHi;


/**
 * \brief handle trap error rountine
 */
static void
handle_trap_error(__u8 err)
{
  //save error type
  if( lseek( fd_nvm, TRAP_ERR_SA, SEEK_SET ) == TRAP_ERR_SA )
    {
      while( write(fd_nvm, &err, TRAP_ERR_LEN) != TRAP_ERR_LEN );
    }
  //save address
  if( lseek( fd_nvm, TRAP_ADDR_LO_SA, SEEK_SET ) == TRAP_ADDR_LO_SA )
    {
      while( write(fd_nvm, &StkAddrLo, TRAP_ADDR_LO_LEN) != TRAP_ADDR_LO_LEN );
    }
  if( lseek( fd_nvm, TRAP_ADDR_HI_SA, SEEK_SET ) == TRAP_ADDR_HI_SA )
    {
      while( write(fd_nvm, &StkAddrHi, TRAP_ADDR_HI_LEN) != TRAP_ADDR_HI_LEN );
    }
}


/**
 * \brief Osciallator Fail Interrupt
 */
void _TRAP_IRQ 
_OscillatorFail(void)
{
  _OSCFAIL = 0;
  handle_trap_error('O');
}


/**
 * \brief Address Error Interrupt
 */
void _TRAP_IRQ 
_AddressError(void)
{
  _ADDRERR = 0;
  handle_trap_error('A');
  
  ERR_LED_CONFIG();
    
  while (1)
    {
      int i;
      //error indication
      i=0;
      while(1)
        {
          ERR_LED0(1);
          mdelay(100); 
          ERR_LED0(0);
          mdelay(100);
          if(i++ > 50) break;
        } 

      //address indication
      for(i=0; i<16; i++)
        {
          //clk
          if(i%2 == 0) ERR_LED1(1);
          else ERR_LED1(0);
          
          //data
          if( (StkAddrLo >> i) & 0x01 ) ERR_LED0(1);
          else ERR_LED0(0);
          mdelay(2000);
        }

      //address indication
      for(i=0; i<16; i++)
        {
          //clk
          if(i%2 == 0) ERR_LED1(1);
          else ERR_LED1(0);
          
          //data
          if( (StkAddrHi >> i) & 0x01 ) ERR_LED0(1);
          else ERR_LED0(0);
          mdelay(2000);
        }
    }
}


/**
 * \brief Starck Error Interrupt
 */
void _TRAP_IRQ 
_StackError(void)
{
  _STKERR = 0;
  handle_trap_error('S');
  
  ERR_LED_CONFIG();

  ERR_LED0(1);
  while (1);
}


/**
 * \brief Maths Interrupt
 */
void _TRAP_IRQ 
_MathError(void)
{
  _MATHERR = 0;
  handle_trap_error('M');

  ERR_LED_CONFIG();

  while (1)
    {
      ERR_LED1(1);
      mdelay(100); 
      ERR_LED1(0);
      mdelay(100);
    }
}

#ifdef MPLAB_DSPIC33_PORT
/**
 * \brief DMA Error Interrupt
 */
void _TRAP_IRQ 
_DMACError(void)
{
  INTCON1bits.DMACERR = 0;
  handle_trap_error('D');

  ERR_LED_CONFIG();

  ERR_LED1(1);
  while (1);
}
#endif /* MPLAB_DSPIC33_PORT */
