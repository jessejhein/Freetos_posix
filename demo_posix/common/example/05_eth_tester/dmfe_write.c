/*
 * not used in testing 
 */

#include <define.h>
 
/********************************************************************************************
 * Name:        int dmfe_write(unsigned char device, unsigned char *buf, int count)
 * 
 * Function:    copy count bytes of the data from buf and transmit
 * 
 * Input:       None       
 * 
 * Ouput:       the number of bytes written (>= 0)
 *              -1: error, uart is not opened for writing (errno = EBADF)
 * 
 *********************************************************************************************/
int dmfe_write(void)
{
    return 0;
}




/*****************************************************************************
 * void dmfe_interrupt(void)
 *****************************************************************************
 * Process Tx Interrupt
 *****************************************************************************/
void dmfe_interrupt(void)
{
    ETH_ISR_IF = 0;       //Clear Interrupt Flag
}
