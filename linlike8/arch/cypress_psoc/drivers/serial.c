/*
 * Copyright (c) 24-05-2004 cheng chung yan yan@amonics.com> / <chungyan5@hkem.com for 8-bit system
 *
 * this module is serial in cypress psoc, driver specification. So each arch. has different code here
 * 
 * using cypress psoc serial
 *
 *5-11-2004	Hei		modified for the return number of the serial_complete_write so that the return number would be the number of bytes transmitted
 *
 *			remark: There is error for the transmit of data after sending one byte of data--the start of package(02), to solve the problem, it is needed to disable the global interupt before sending the data and enable the interupt after sending the data
 */

// including, get data linking from others ===============================================
//	linlike8 configuration
#include <linlike8/config.h>
#include "app.h"
#include <asm/system.h>
#if (UART_MOD==1)
#if (SWITCH_CASE==0)
	#include "sched.h"								// linlike8 system
	#include "timer.h"
#endif
#include "cirbuf.h"									// pre_wr_cir254buf()
#include "uart_1.h"
#include "uart_2.h"

unsigned char serial_rx_buf[2][MAX_UART_BUF];		
struct Serial_Rx{
	unsigned char	wr;
	unsigned char	rd;
};
struct Serial_Rx serial_rx[2];

unsigned char serial_tx_buf[2][MAX_UART_TX_BUF];
struct Serial_Tx{
	unsigned char	wr;								// max. of tx buf. is 7
	unsigned char	rd;
	unsigned char	tx_complete_flag;						// true as completed, false is non-completed
};
struct Serial_Tx serial_tx[2];

//	define structure for the uart_write_complete
struct WR_COMPL_VAR{
	unsigned tmp	:7;
	unsigned state	:1;
};

void uart_open(void)
{
	serial_rx[0].wr = 0;
	serial_rx[0].rd = 0;
	serial_tx[0].wr = 0;
	serial_tx[0].rd = 0;
	serial_tx[0].tx_complete_flag = 1; 
	serial_rx[1].wr = 0;
	serial_rx[1].rd = 0;
	serial_tx[1].wr = 0;
	serial_tx[1].rd = 0;
	serial_tx[1].tx_complete_flag = 1;

        UART_1_EnableInt();
        UART_2_EnableInt(); 
        UART_1_Start(UART_PARITY_NONE);
        UART_2_Start(UART_PARITY_NONE);
}
#endif

#pragma	interrupt_handler	serial_1_tx_isr						// should add this isr name at boot.asm from PSoC Designer
#pragma interrupt_handler	serial_2_tx_isr						// should add this isr name at boot.asm from PSoC Designer

void serial_1_tx_isr(void)
{
#if (UART_MOD==1)
	unsigned char k;
	if (bUART_1_ReadTxStatus() & UART_TX_BUFFER_EMPTY) {
		if ((k=pre_rd_cir254buf((unsigned char)serial_tx[0].wr,(unsigned char)serial_tx[0].rd,MAX_UART_TX_BUF))!=255) {
			UART_1_SendData(serial_tx_buf[0][(unsigned char)serial_tx[0].rd]);
			serial_tx[0].rd = (unsigned)k;
		} else serial_tx[0].tx_complete_flag = 1;					// change to empty of tx
	}
#endif
}

void serial_2_tx_isr(void)
{
#if (UART_MOD==1)
	unsigned char k;
	if (bUART_2_ReadTxStatus() & UART_TX_BUFFER_EMPTY) {
		if ((k=pre_rd_cir254buf((unsigned char)serial_tx[1].wr,(unsigned char)serial_tx[1].rd,MAX_UART_TX_BUF))!=255) {
			UART_2_SendData(serial_tx_buf[1][(unsigned char)serial_tx[1].rd]);
			serial_tx[1].rd = (unsigned)k;
		} else serial_tx[1].tx_complete_flag = 1;					// change to empty of tx
	}
#endif
}
#pragma	interrupt_handler	serial_1_rx_isr						// should add this isr name at boot.asm from PSoC Designer
#pragma	interrupt_handler	serial_2_rx_isr						// should add this isr name at boot.asm from PSoC Designer
void serial_1_rx_isr(void)
{
#if (UART_MOD==1)
	unsigned char i;
	unsigned char bRxStatus;
	if ( bRxStatus=bUART_1_ReadRxStatus() & UART_RX_COMPLETE ) {
		if ( ! (bRxStatus & UART_RX_NO_ERROR) ) {
			if ((i=pre_wr_cir254buf(serial_rx[0].wr,serial_rx[0].rd,MAX_UART_BUF))!=255) {
				serial_rx_buf[0][serial_rx[0].wr] = bUART_1_ReadRxData();
				serial_rx[0].wr = i;
			} else bUART_1_ReadRxData();					// any cases, i must need to clear the UART rx register data, then prepare for next interrupt
		}
	}
#endif
}

void serial_2_rx_isr(void)
{
#if (UART_MOD==1)
	unsigned char i;
	unsigned char bRxStatus;
	if ( bRxStatus=bUART_2_ReadRxStatus() & UART_RX_COMPLETE ) {
		if ( ! (bRxStatus & UART_RX_NO_ERROR) ) {
			if ((i=pre_wr_cir254buf(serial_rx[1].wr,serial_rx[1].rd,MAX_UART_BUF))!=255) {
				serial_rx_buf[1][serial_rx[1].wr] = bUART_2_ReadRxData();
				serial_rx[1].wr = i;
			} else bUART_2_ReadRxData();					// any cases, i must need to clear the UART rx register data, then prepare for next interrupt
		}
	}
#endif
}

#if (UART_MOD==1)
unsigned char uart_write(unsigned char *__buf, unsigned char __n,unsigned char which,unsigned char uart)		//which -- to decide which package set would use
{
	unsigned char i;
//	struct {
//		unsigned j : 7;
		//unsigned k : 1;
//	} var;
	unsigned char var;
	cli();													//diable the global interupt
	if (serial_tx[which].tx_complete_flag) {						// empty of tx
		//var.k = 1;
		var = 1;
	} else {
		//var.k = 0;
		var = 0;
	}
	for (;var<__n;var++) {
		if ((i=pre_wr_cir254buf((unsigned char)serial_tx[which].wr,(unsigned char)serial_tx[which].rd,MAX_UART_TX_BUF))!=255) {	
			serial_tx_buf[which][serial_tx[which].wr] = *(__buf+var);
			serial_tx[which].wr = i;
		} else break;
	}
//	if (var.k==1) UART_1_SendData(*(__buf));
	if (serial_tx[which].tx_complete_flag) {
		if (uart == 0)UART_1_SendData(*(__buf));	// send 1st byte to active the Tx
		else UART_2_SendData(*(__buf));
		serial_tx[which].tx_complete_flag = 0;						// change to non-empty of tx
	}
	sti();													//enable the global interupt
	return (unsigned char) var;
}

//<rec 1 byte,remove>unsigned char read(unsigned char *__buf, unsigned char __n)	// reduce RAM usage
unsigned char uart_read(unsigned char *__buf, unsigned char which)					//<rec 1 byte,add>
{
	unsigned char k;
	//<rec 1 byte,remove>unsigned char j;
	//<rec 1 byte,remove>for (j=0;j<__n;j++) {
		if ((k=pre_rd_cir254buf(serial_rx[which].wr,serial_rx[which].rd,MAX_UART_BUF))!=255) {
			//<rec 1 byte,remove>*(__buf+j) = serial_rx_buf[which][serial_rx[which].rd];
			*(__buf) = serial_rx_buf[which][serial_rx[which].rd];//<rec 1 byte,add>
			serial_rx[which].rd = k;
			return 1;//<rec 1 byte,add>
		}//<rec 1 byte,add>
		return 0;//<rec 1 byte,add>
		//<rec 1 byte,remove>} else break;
	//<rec 1 byte,remove>}
	//<rec 1 byte,remove>return j;
}

#if (SWITCH_CASE==0)
void uart_write_complete(unsigned char *__buf, unsigned char __n, unsigned char which,unsigned char uart)			//which -- to decide which package set would use
{
	// non-tested
	unsigned char i = 0;
	do {
		i += serial_write(__buf + i, __n - i,which,uart);
		if (i==__n) break;
		else msleep(10);
	} while (1);
}
#else
unsigned char uart_write_complete(unsigned char *__buf, unsigned char __n,struct WR_COMPL_VAR *pa,unsigned char which,unsigned char uart)
{
	switch (pa->state) {
		case 0 :
			 pa->tmp = 0;
			 pa->state = 1;
			break;
		case 1 :
			 pa->tmp += uart_write(__buf + pa->tmp, __n - pa->tmp,which,uart);
			 if (pa->tmp>=__n)
			 { 
			 pa->state = 0;
			 return __n;
			 }
			break;
	}
	return 0;
}
#endif

#endif