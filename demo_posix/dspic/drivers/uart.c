/************************************************************************************************
 * File: 			uart.c
 * Description:		control U1ART (RS485) and U2ART (RS232)
 *					Primary: U2ART (RS232), device 0
 *					Secondary: U1ART (RS485), device 1
 ***********************************************************************************************
 * SUMMARY:
 * 1)	The uart module consists of 2 channels located at U1RX (34), U1TX, (33), U2RX (31) and
 * 		U1TX (32).
 * 2)	U2ART is referenced by UARTA, U1ART is referenced by UARTB. By default, U2ART is used
 * 		for RS232 communication, and U1ART is used for RS485 communication.
 * 3)	Default communcation speed is 19200bps. Autobaud detection is disabled.
 * 4) 	The driver has a POSIX-compliant interface with open(), read(), and write().
 * 5)   When U2RX detects a break character, it resets the system for bootloader application  
 ***********************************************************************************************/

#include <define.h>
#include <fcntl.h>
#include <errno.h>
#include <asm/system.h>
#include <asm/delay.h>

#if (UART_MOD == 0)
//do not include uart module if disabled

#else 

/************************************************************************************************
 * Buffers for UARTs data
 ************************************************************************************************/
static unsigned char uart_rx_buf[NO_OF_UART][MAX_UART_RX_BUF];
struct UART_Rx{
    unsigned char   wr;
    unsigned char   rd;
};
static struct UART_Rx uart_rx[NO_OF_UART];
static unsigned char uart_tx_buf[NO_OF_UART][MAX_UART_TX_BUF];
struct UART_Tx{
    unsigned char   wr;                             
    unsigned char   rd;
    unsigned char   tx_complete_flag;
};
static struct UART_Tx uart_tx[NO_OF_UART];

static int uart_io_flag[NO_OF_UART];

/************************************************************************************************
 * Local Functions Declaration
 ************************************************************************************************/
static unsigned int uart2_autobaud(void);
static void uart2_init(void);
#if (NO_OF_UART>=2)
static unsigned int uart1_autobaud(void);
static void uart1_init(void);
#endif

/********************************************************************************************
 * Name:        int uart_open(unsigned char device, int flags)
 * 
 * Function:    get uart port ready
 * 
 * Input:       device: uart port id
 *                      +--Primary: U2ART (RS232), device 0
 *                      +--Secondary: U1ART (RS485), device 1
 *              flags: accessing mode
 * 
 * Ouput:       0: uart opened
 * 
 *********************************************************************************************/
int uart_open(unsigned char device, int flags)
{
    uart_io_flag[device] = flags;
    
#   if (NO_OF_UART>=2)
    switch (device) {
        case 0 : 
#   endif
			//==========Primary UART = U2ART: RS232================
			uart_rx[0].wr = 0;
            uart_rx[0].rd = 0;
			//=====================================================
            uart_tx[0].wr = 0;
            uart_tx[0].rd = 0;
            uart_tx[0].tx_complete_flag = 1;
			//=====================================================
			uart2_init();
			//=====================================================
#   if (NO_OF_UART>=2)
            break;
        case 1 :
			//========Secondary UART = U1ART: RS485================
            uart_rx[1].wr = 0;
            uart_rx[1].rd = 0;
			//=====================================================
            uart_tx[1].wr = 0;
            uart_tx[1].rd = 0;
            uart_tx[1].tx_complete_flag = 1;
			//=====================================================
			uart1_init();
			//=====================================================
            break;
    } //end switch
#   endif
    return 0;
}

/************************************************************************************************
 * Initialize U2ART (RS232)
 ************************************************************************************************/
static void uart2_init(void){
	#if(UARTA_RS485>0)
		RS485_CTL_EN();		//Enable control pin for output
	#endif
	//=============================================================================
	// Configure Baud rate
	//  +-- Default Baud rate = 19.2 kbps
	//	+-- U2BRG = 30e6 / (16 * 19200) - 1 = 97
	unsigned int u2brg = UART_DEFAULT_BRG; 
	#if(AUTO_BAUD_DECT>0)
	u2brg = uart2_autobaud();
	#endif
	U2BRG  = u2brg;					
	//==============================================================================
    // Disable U2ART
    U2MODEbits.UARTEN = 0;			//Disable U2ART module
	//==============================================================================
	// Configure Interrupt Priority
    _U2RXIF = 0;	//Clear Rx interrupt flags
    _U2TXIF = 0;	//Clear Tx interrupt flags
    _U2RXIE = 1;	//Receive interrupt: 0 disable, 1 enable 
    _U2TXIE = 1;	//Transmit interrupt: 0 disable, 1 enable
	//==============================================================================
	// Configure Mode
	//  +--Default: 8N1, no loopback, no wake in sleep mode, continue in idle mode
	//	+--Autobaud detect from IC2
	//	+--Enable U2ART module
	U2MODEbits.ABAUD = 0;	//Disable Autobaud detect from U2RX 
    U2MODEbits.UARTEN = 1;	//U2ART enable
	//==============================================================================
	// Configure Status
	//	+--Default: TxInt when a char is transmitted, no break char
	//	+--Default: RxInt when a char is received, no address detect, clear overflow
	//	+--Enable Transmit
	U2STAbits.UTXEN = 1;	//Tx enable
}

/************************************************************************************************
 * Initialize U1ART (RS485)
 ************************************************************************************************/
#if (NO_OF_UART>=2)
static void uart1_init(void){
	#if(UARTB_RS485>0)
		RS485_CTL_EN();		//Enable control pin for output
	#endif
	//=============================================================================
	// Configure Baud rate
	//  +-- Default Baud rate = 19.2 kbps
	//	+-- U1BRG = 30e6 / (16 * 19200) - 1 = 97
	unsigned int u1brg = UART_DEFAULT_BRG; 
	#if(AUTO_BAUD_DECT>0)
	u1brg = uart1_autobaud();
	#endif
	U1BRG  = u1brg;					
	//==============================================================================
    // Disable U1ART
    U1MODEbits.UARTEN = 0;			//Disable U1ART module
	//==============================================================================
	// Configure Interrupt Priority
    _U1RXIF = 0;	//Clear Rx interrupt flags
    _U1TXIF = 0;	//Clear Tx interrupt flags
    _U1RXIE = 1;	//Receive interrupt: 0 disable, 1 enable 
    _U1TXIE = 1;	//Transmit interrupt: 0 disable, 1 enable
	//==============================================================================
	// Configure Mode
	//  +--Default: 8N1, no loopback, no wake in sleep mode, continue in idle mode
	//	+--Autobaud detect from IC1
	//	+--Enable U1ART module
	U1MODEbits.ABAUD = 0;	//Disable Autobaud detect from U1RX 
    U1MODEbits.UARTEN = 1;	//U1ART enable
	//==============================================================================
	// Configure Status
	//	+--Default: TxInt when a char is transmitted, no break char
	//	+--Default: RxInt when a char is received, no address detect, clear overflow
	//	+--Enable Transmit
	U1STAbits.UTXEN = 1;	//Tx enable
}
#endif

/*******************************************************************************
 * UART2 Auto baud detect
 *******************************************************************************/
static unsigned int uart2_autobaud(void){
	U2MODEbits.ABAUD = 1;	//Enable Autobaud detect from U2RX (from IC2 if 0)
    U2MODEbits.UARTEN = 1;	//U2ART enable
    //Timer 3 Config==========================================================
	PR3 = 0xFFFF;			// Set counter to maximum
	_T3IF = 0;				// Clear interrupt flag
	_T3IE = 0;				// Disable interrupt
	T3CONbits.TON = 1;		// Start the timer, TMR3 count up
    //Input Capture Config====================================================
	IC2CONbits.ICM = 3;		//Detect rising	
	_IC2IF = 0;				//Clear interrupt flag
	_IC2IE = 0; 			//Disable interrupt
    //Start Auto baud detection===============================================
    //Expected character = 'U' (0x55)
    //			  _   _   _   _   _
    //			_|S|_|1|_|1|_|1|_|1|_
    //           <--------------->
    //========================================================================
    cli();					//Disable Global Interrupt
	while(!_IC2IF);			//1st rising edge detected
    TMR3 = 0;				//Clear TMR3 to start count up
    _IC2IF = 0;				//Clear interrupt flag
	while(!_IC2IF);			//2nd rising edge detected
	_IC2IF = 0;				//Clear interrupt flag
	while(!_IC2IF);			//3rd rising edge detected
	_IC2IF = 0;				//Clear interrupt flag
	while(!_IC2IF);			//4th rising edge detected
	_IC2IF = 0;				//Clear interrupt flag
	while(!_IC2IF);			//5th rising edge detected
	_IC2IF = 0;				//Clear interrupt flag
	T3CONbits.TON = 0;		//Stop the timer
	sti(); 					//Enable Global Interrupt
    //Compute value for BRG register==========================================
    // BRG 	= (FCY/(16*baudrate))-1
    //		= (FCY/(16*FCY/(TMR3/8)))-1
    //		= (TMR3/128) - 1
    // add 0x04 for rounding
    unsigned int time;
	time = ((TMR3+0x40)>>7)-1;			
    //========================================================================
    return time;   
}

/*******************************************************************************
 * UART1 Auto baud detect
 *******************************************************************************/
#if (NO_OF_UART>=2)
static unsigned int uart1_autobaud(void){
	U1MODEbits.ABAUD = 1;	//Enable Autobaud detect from U1RX (from IC1 if 0)
    U1MODEbits.UARTEN = 1;	//U1ART enable
    //Timer 3 Config==========================================================
	PR3 = 0xFFFF;			// Set counter to maximum
	_T3IF = 0;				// Clear interrupt flag
	_T3IE = 0;				// Disable interrupt
	T3CONbits.TON = 1;		// Start the timer, TMR3 count up
    //Input Capture Config====================================================
	IC1CONbits.ICM = 3;		//Detect rising	
	_IC1IF = 0;				//Clear interrupt flag
	_IC1IE = 0; 			//Disable interrupt
    //Start Auto baud detection===============================================
    //Expected character = 'U' (0x55)
    //			  _   _   _   _   _
    //			_|S|_|1|_|1|_|1|_|1|_
    //           <--------------->
    //========================================================================
    cli();					//Disable Global Interrupt
	while(!_IC1IF);			//1st rising edge detected
    TMR3 = 0;				//Clear TMR3 to start count up
    _IC1IF = 0;				//Clear interrupt flag
	while(!_IC1IF);			//2nd rising edge detected
	_IC1IF = 0;				//Clear interrupt flag
	while(!_IC1IF);			//3rd rising edge detected
	_IC1IF = 0;				//Clear interrupt flag
	while(!_IC1IF);			//4th rising edge detected
	_IC1IF = 0;				//Clear interrupt flag
	while(!_IC1IF);			//5th rising edge detected
	_IC1IF = 0;				//Clear interrupt flag
	T3CONbits.TON = 0;		//Stop the timer
	sti(); 					//Enable Global Interrupt
    //Compute value for BRG register==========================================
    // BRG 	= (FCY/(16*baudrate))-1
    //		= (FCY/(16*FCY/(TMR3/8)))-1
    //		= (TMR3/128) - 1
    // add 0x04 for rounding
    unsigned int time;
	time = ((TMR3+0x40)>>7)-1;			
    //========================================================================
    return time;   
}
#endif

/*******************************************************************************
 * Transmit Interrupt Handler for U2ART (RS232)
 *  +-- An interrupt will occur whenever tx buffer becomes empty
 * 	+-- First interrupt is set manually in uart_write
 *******************************************************************************/
void _ISR _U2TXInterrupt(void){
    
#ifdef MPLAB_DSPIC33_PORT
    _U2TXIF = 0;
#else
    DISI_PROTECT(_U2TXIF = 0);          //Clear Interrupt Flag
#endif

    unsigned char next_data_pos;
    next_data_pos = pre_rd_cir254buf(   (unsigned char)uart_tx[0].wr, 
                                        (unsigned char)uart_tx[0].rd, 
                                        MAX_UART_TX_BUF);
    if (next_data_pos!= 255) {
        //Valid Data is available to transmit
        U2TXREG = (uart_tx_buf[0][(unsigned char)uart_tx[0].rd] & 0xFF); 	//send next byte...
        uart_tx[0].rd = (unsigned char) next_data_pos;						//update rd pointer
    } else {
        //Transimission has completed
        uart_tx[0].tx_complete_flag = 1;							// change to empty of tx
        #if(UARTA_RS485>0)
            while(!U2STAbits.TRMT);			//wait for transmit register to empty, i.e. the byte has been sent
            RS485_TX_OFF();					//Set transmit pin to Low, i.e. back to recieve mode
        #endif		
    }   
}

/*******************************************************************************
 * Transmit Interrupt Handler for U1ART (RS485)
 *   +-- An interrupt will occur when tx buffer becomes empty
 *******************************************************************************/
#if (NO_OF_UART>=2)
void _ISR _U1TXInterrupt(void){
    
#ifdef MPLAB_DSPIC33_PORT
    _U1TXIF = 0;
#else
    DISI_PROTECT(_U1TXIF = 0);          //Clear Interrupt Flag
#endif

    unsigned char next_data_pos;
    next_data_pos = pre_rd_cir254buf(   (unsigned char)uart_tx[1].wr, 
                                        (unsigned char)uart_tx[1].rd, 
                                        MAX_UART_TX_BUF);
    if (next_data_pos!=255) {
        //Transimission has not finished
        U1TXREG = (uart_tx_buf[1][(unsigned char)uart_tx[1].rd] & 0xFF);	//send next byte...
        uart_tx[1].rd = (unsigned char) next_data_pos;						//update rd pointer
    } else {
        //Transimission has completed
        uart_tx[1].tx_complete_flag = 1;							// change to empty of tx
        #if(UARTB_RS485>0)
            while(!U1STAbits.TRMT);			//wait for transmit register to empty, i.e. the byte has been sent
            RS485_TX_OFF();					//Set transmit pin to Low, i.e. back to recieve mode
        #endif		
    } 
}
#endif

/*******************************************************************************
 * Receive Interrupt Handler for U2ART (RS232)
 *  +-- An interrupt will occur when a byte is received
 *	+-- read the receive buffer till at least one or more character can read
 *******************************************************************************/
void _ISR _U2RXInterrupt(void)
{
    //No Framming error
    if( U2STAbits.FERR == 0)
    {
        unsigned char next_data_pos;
        if ( U2STAbits.URXDA ){
            //Data is received======================================================
            next_data_pos = pre_wr_cir254buf( uart_rx[0].wr, uart_rx[0].rd, MAX_UART_RX_BUF);
            if (next_data_pos!=255) {
                //If buffer is not full
                uart_rx_buf[0][uart_rx[0].wr] = (unsigned char) U2RXREG; //Read the data from buffer
                uart_rx[0].wr = next_data_pos;
            } 
            else{
                //When buffer is full, still remove data from register, butthe incoming data is lost
                next_data_pos = (unsigned char) U2RXREG; 			//Read the data from buffer
            }		
        }
    }
    //Framming error
    else
    {
        if ( U2STAbits.URXDA ){
            unsigned char data;
            data = (unsigned char) U2RXREG;
            if(data == 0x00){
                //A break char has been received: 
                //  U2RX has been pulled to zero for more than 13 bits
                //  This is used to reboot the pic
                mdelay(800);    //wait for break character to clear
                asm("reset");   //software reset
            }
        }        
    }

#ifdef MPLAB_DSPIC33_PORT
    _U2RXIF = 0;
#else
    DISI_PROTECT(_U2RXIF = 0);          //Clear Interrupt Flag
#endif
}

/*******************************************************************************
 * Receive Interrupt Handler for U1ART (RS485)
 *  +-- An interrupt will occur when a byte is received
 *	+-- read the receive buffer till at least one or more character can read
 *******************************************************************************/
#if (NO_OF_UART>=2)
void _ISR _U1RXInterrupt(void){
    unsigned char next_data_pos;
    //Data is received======================================================
    next_data_pos = pre_wr_cir254buf( uart_rx[1].wr, uart_rx[1].rd, MAX_UART_RX_BUF);
    if ( U1STAbits.URXDA  ){
        if (next_data_pos!=255) {
            //If buffer is not full
            uart_rx_buf[1][uart_rx[1].wr] = (unsigned char) U1RXREG;
            uart_rx[1].wr = next_data_pos;
        } 
        else{
            //When buffer is full, still remove data from register, butthe incoming data is lost
            next_data_pos = (unsigned char) U1RXREG; 			//Read the data from buffer
        }
    }
    
#ifdef MPLAB_DSPIC33_PORT
    _U1RXIF = 0;
#else
    DISI_PROTECT(_U1RXIF = 0);          //Clear Interrupt Flag
#endif
}
#endif

/********************************************************************************************
 * Name:        int uart_write(unsigned char device, unsigned char *buf, int count)
 * 
 * Function:    copy count bytes of the data from buf and transmit
 * 
 * Input:       device: uart port id
 *              buf: pointer to buffer for writing
 *              count: number of bytes to write
 * 
 * Ouput:       the number of bytes written (>= 0)
 *              -1: error, uart is not opened for writing (errno = EBADF)
 * 
 *********************************************************************************************/
int uart_write(unsigned char device, unsigned char *buf, int count)
{
    //Perform write if write operation is enabled
    if(uart_io_flag[device] & O_RDWR || uart_io_flag[device] & O_WRONLY)
    {
        int next_data_pos;
        int byte = 0;
        for (; byte<count; byte++) {
            next_data_pos = pre_wr_cir254buf(   (unsigned char)uart_tx[device].wr, 
                                                (unsigned char)uart_tx[device].rd, 
                                                MAX_UART_TX_BUF);   
            if (next_data_pos!=255) {
                //Valid data is available
                uart_tx_buf[device][uart_tx[device].wr] = (unsigned char) buf[byte];    //copy the char to tx_buf
                uart_tx[device].wr = next_data_pos;                                     //increment the ptr
            } else break;
        }
        //Set transmit complete flag to false
        if (uart_tx[device].tx_complete_flag) {
            uart_tx[device].tx_complete_flag = 0;
        }
        //Raise Interrupt flag to initiate transmission
        if(device==0){
            #if(UARTA_RS485>0)
                RS485_TX_ON();  //Set transmit pin to High
            #endif  
            _U2TXIF = 1;    //Start interrupt
        } else{
            #if(UARTB_RS485>0)
                RS485_TX_ON();  //Set transmit pin to High
            #endif      
            _U1TXIF = 1;    //Start interrupt
        }
        return byte;        
    }
    //Error, raise error flag
    else
    {
        errno = EBADF;  //io not opened for writing
        return -1;
    }  
}

/********************************************************************************************
 * Name:        int uart_read(unsigned char device, unsigned char *buf)
 * 
 * Function:    copy the received data to buf
 * 
 * Input:       device: uart port id
 *              buf: pointer to buffer for reading
 * 
 * Ouput:       1, indicating 1 byte has been read
 *              0, indicating no data is available
 *              -1: error, uart is not opened for read operation (errno = EBADF)
 * 
 *********************************************************************************************/
int uart_read(unsigned char device, unsigned char *buf)
{
    //Perform read if read operation is enabled
    if(uart_io_flag[device] & O_RDWR || uart_io_flag[device] & O_RDONLY)
    {
        int next_data_pos;
        next_data_pos = pre_rd_cir254buf( uart_rx[device].wr, uart_rx[device].rd, MAX_UART_RX_BUF);
        //Copy 1 byte when data is available
        if (next_data_pos!=255) 
        {
            *buf = uart_rx_buf[device][uart_rx[device].rd];     //copy the stored data to buf
            uart_rx[device].rd = next_data_pos;                 //update the ptr
            return 1;
        }
        //No data can be copied
        else
        {
            return 0;
        }      
    }
    //Error, raise error flag
    else
    {
        errno = EBADF;  //io not opened for reading
        return -1;
    }    
}

#endif //UART_MOD
