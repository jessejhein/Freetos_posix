/*
 * Copyright (c) 24-05-2004 cheng chung yan yan@amonics.com> for 8-bit system
 *
 * this module is uart in avr
 * support : RS232 and RS485
 * 
 * pins define
 * 	RX and TX(since avr contains one UART, so using its default pins) -- see datasheet
 * 	in RS485 mode, tx_rx_ctrl(low active for receive, high active for transmit) pin, this pin defined in app_define module
 */


// including, get data linking from others ===============================================
//	avr platform
#include <avr/io.h>									// io ctrl
#include <avr/signal.h>								// interrupt signal
//#include <avr/interrupt.h>
//	linlike8 configuration
#include <linlike8/config.h>
#include <asm/system.h>
#if (UART_MOD==1)
#include <linlike8/cirbuf.h>						// pre_wr_cir254buf()
//	uart
#include <linlike8/uart.h>


// data =================================================================================

													// set direction of PC.4
													// set direction of PD.2
#define	INIT_485_CTRL()		do {								\
								DDRC |= _BV(PC4);				\
								DDRD |= _BV(PD2);				\
							} while (0)							\
													// rec. state, must PD.2 1st to disable tx.
#define	RX_485_DATA()		do {								\
								PORTD &= ~_BV(PD2);				\
								PORTC &= ~_BV(PC4);				\
							} while (0)							\
													// tx. state, must PC.4 1st, to disable rx.
#define	TX_485_DATA()		do {								\
								PORTC |= _BV(PC4);				\
								PORTD |= _BV(PD2);				\
							} while (0)							\
													// this code will be put to app_define module later, after tested

unsigned char uart_rx_buf[MAX_UART_BUF];
struct {
	unsigned	wr :4;
	unsigned	rd :4;
} uart_rx;
unsigned char uart_tx_buf[MAX_UART_TX_BUF];
struct {
	unsigned	wr :3;								// max. of tx buf. is 7
	unsigned	rd :4;
	unsigned	tx_complete_flag :1;						// true as completed, false is non-completed
} uart_tx;
#if (SWITCH_CASE==1)
struct wr_compl_struct {
	unsigned tmp	:7;
	unsigned state	:1;
} wr_compl_var;
#endif

void uart_open(void)
{
	uart_rx.wr = 0;
	uart_rx.rd = 0;
	uart_tx.wr = 0;
	uart_tx.rd = 0;
	uart_tx.tx_complete_flag = 1;

#if (SWITCH_CASE==1)
	wr_compl_var.state = 0;
#endif
#if (RS485==1)
	INIT_485_CTRL();								
	RX_485_DATA();
#endif
	//UBRRH = 0x00;
	UBRRL = 25;											// ref. fr. datasheet, 8MHz MClk, 19.2K baudrate, (unsigned char) (8000000/(19200*16L) -1)
	UCSRB = ( (1<<RXCIE) | (1<<RXEN) | (1<<TXEN) );		// Enable UART receiver and transmitter, and receive interrupt
	UCSRC = (1 << URSEL) | (3 << UCSZ0);				// asynchronous 8N1
	/*
	DDRC |= _BV(PC3);
	DDRC |= _BV(PC1);
	PORTC &= ~_BV(PC1);
	PORTC &= ~_BV(PC3);*/
}
#endif	// (UART_MOD==1)

SIGNAL(SIG_UART_DATA)								// chk this in include/avr/iom8535.h
{
#if (UART_MOD==1)
	unsigned char k;
	
//if (PORTC & _BV(PC0)) PORTC &= ~_BV(PC0);
//else PORTC |= _BV(PC0);
	if ((k=pre_rd_cir254buf((unsigned char)uart_tx.wr,(unsigned char)uart_tx.rd,MAX_UART_TX_BUF))!=255) {
		UDR = uart_tx_buf[(unsigned char)uart_tx.rd];
		uart_tx.rd = (unsigned)k;
	} else {
		UCSRB &= ~(1<<UDRIE);         					// Disable UDRE interrupt
		uart_tx.tx_complete_flag = 1;					// change to empty of tx
#if (RS485==1)
		UCSRB |= (1<<TXCIE);                   			// Enable UART completed interrupt
#endif
	}
#endif
}

#if (RS485==1)
SIGNAL(SIG_UART_TRANS)								// chk this in include/avr/iom8535.h
{
	UCSRB &= ~(1<<TXCIE);							// after complete tx all, disable rs485 tx ctrl
	RX_485_DATA();									// set back to rec. mode
}
#endif

SIGNAL(SIG_UART_RECV)									// chk this in include/avr/iom8535.h
{
#if (UART_MOD==1)
	unsigned char i;
	unsigned char data;
	data = UDR;                 						// Read the received data, any cases, i must need to clear the UART rx register data, then prepare for next interrupt
	if ((i=pre_wr_cir254buf(uart_rx.wr,uart_rx.rd,MAX_UART_BUF))!=255) {
		uart_rx_buf[uart_rx.wr] = data;			
		uart_rx.wr = i;
	}
#endif
}

#if (UART_MOD==1)
unsigned char uart_write(unsigned char *__buf, unsigned char __n,unsigned char which,unsigned char uart)
{
	unsigned char i;
//	struct {
//		unsigned j : 7;
		//unsigned k : 1;
//	} var;
	unsigned char var;
//	if (uart_tx.tx_complete_flag) {						// empty of tx
//		var = 1;
//	} else {
		var = 0;
//	}
	cli();													//diable the global interupt
	for (;var<__n;var++) {
		if ((i=pre_wr_cir254buf((unsigned char)uart_tx.wr,(unsigned char)uart_tx.rd,MAX_UART_TX_BUF))!=255) {
			uart_tx_buf[uart_tx.wr] = *(__buf+var);
			uart_tx.wr = i;
		} else break;
	}
	sti();// should be remove after testing					//enable the global interupt
	
//	if (uart_tx.tx_complete_flag) {
#if (RS485==1)
		TX_485_DATA();										// set to tx mode
#endif
		UCSRB |= (1<<UDRIE);                    			// Enable UDRE interrupt
//		UDR = *(__buf);										//	and send 1st byte to active the Tx
//		uart_tx.tx_complete_flag = 0;						// change to non-empty of tx
//	}

	return (unsigned char) var;
}

//<rec 1 byte,remove>unsigned char read(unsigned char *__buf, unsigned char __n)	// reduce RAM usage
unsigned char uart_read(unsigned char *__buf, unsigned char which) //<rec 1 byte,add>
{
	unsigned char k;
	//<rec 1 byte,remove>unsigned char j;
	//<rec 1 byte,remove>for (j=0;j<__n;j++) {
		if ((k=pre_rd_cir254buf(uart_rx.wr,uart_rx.rd,MAX_UART_BUF))!=255) {
			//<rec 1 byte,remove>*(__buf+j) = uart_rx_buf[uart_rx.rd];
			*(__buf) = uart_rx_buf[uart_rx.rd];//<rec 1 byte,add>
			uart_rx.rd = k;
			return 1;//<rec 1 byte,add>
		}//<rec 1 byte,add>
		return 0;//<rec 1 byte,add>
		//<rec 1 byte,remove>} else break;
	//<rec 1 byte,remove>}
	//<rec 1 byte,remove>return j;
}

#if (SWITCH_CASE==0)
void uart_write_complete(unsigned char *__buf, unsigned char __n)
{
	// non-tested
	unsigned char i = 0;
	do {
		i += uart_write(__buf + i, __n - i);
		if (i==__n) break;
		else msleep(10);
	} while (1);
}
#else
unsigned char uart_write_complete(unsigned char *__buf, unsigned char __n,struct WR_COMPL_VAR *pa,unsigned char which,unsigned char uart)
{
	switch (wr_compl_var.state) {
		case 0 :
			 wr_compl_var.tmp = 0;
			 wr_compl_var.state = 1;
			break;
		case 1 :
			 wr_compl_var.tmp += uart_write(__buf + wr_compl_var.tmp, __n - wr_compl_var.tmp,0,0);
			 if (wr_compl_var.tmp>=__n)
			 { 
			 wr_compl_var.state = 0;
			 return __n;
			 }
			break;
	}
	return 0;
}
#endif	// #if (SWITCH_CASE==0)

#endif	// #if (UART_MOD==1)
