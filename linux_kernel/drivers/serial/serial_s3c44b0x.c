/*
 *  linux/drivers/char/serial_s3c44b0.c
 *
 *  Driver for S3C44B0 serial ports
 *
 *  Based on drivers/char/serial.c, by Linus Torvalds, Theodore Ts'o.
 *
 *  Copyright 1999 ARM Limited
 *  Copyright (C) 2000 Deep Blue Solutions Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  $Id: s3c44b0.c,v 1.9.2.1 2001/11/27 17:35:39 rmk Exp $
 *
 * This is a generic driver for ARM S3C44B0-type serial ports.  They
 * have a lot of 16550-like features, but are not register compatable.
 * Note that although they do have CTS, DCD and DSR inputs, they do
 * not have an RI input, nor do they have DTR or RTS outputs.  If
 * required, these have to be supplied via some other means (eg, GPIO)
 * and hooked into this driver.
 */
#include <linux/config.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/major.h>
#include <linux/string.h>
#include <linux/fcntl.h>
#include <linux/ptrace.h>
#include <linux/ioport.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/circ_buf.h>
#include <linux/serial.h>
#include <linux/console.h>
#include <linux/sysrq.h>

#include <asm/system.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/bitops.h>

#if defined(CONFIG_SERIAL_S3C44B0X_CONSOLE) && defined(CONFIG_MAGIC_SYSRQ)
#define SUPPORT_SYSRQ
#endif

#include <linux/serial_core.h>
#include "hardware.h" //<asm/hardware.h>

#define UART_NR		2

#define SERIAL_S3C44B0_MAJOR	4
#define SERIAL_S3C44B0_MINOR	64
#define SERIAL_S3C44B0_NR	UART_NR

#define CALLOUT_S3C44B0_NAME	"cua"
#define CALLOUT_S3C44B0_MAJOR	5
#define CALLOUT_S3C44B0_MINOR	64
#define CALLOUT_S3C44B0_NR	UART_NR

static struct tty_driver normal, callout;
static struct tty_struct *s3c44b0_table[UART_NR];
static struct termios *s3c44b0_termios[UART_NR], *s3c44b0_termios_locked[UART_NR];
#ifdef SUPPORT_SYSRQ
static struct console s3c44b0_console;
#endif


/*
 * Access macros for the S3C44B0 UARTs
 */
#define UFSTAT		((port)->iobase + (void *)&UFSTAT0 - (void *)&ULCON0)
#define UERSTAT		((port)->iobase + (void *)&UERSTAT0 - (void *)&ULCON0)
#define UMSTAT		((port)->iobase + (void *)&UMSTAT0 - (void *)&ULCON0)
#define URXH		((port)->iobase + (void *)&URXH0 - (void *)&ULCON0)
#define UTXH		((port)->iobase + (void *)&UTXH0 - (void *)&ULCON0)
#define UMCON		((port)->iobase + (void *)&UMCON0 - (void *)&ULCON0)
#define UCON		((port)->iobase + (void *)&UCON0 - (void *)&ULCON0)
#define ULCON		((port)->iobase + (void *)&ULCON0 - (void *)&ULCON0)
#define UFCON		((port)->iobase + (void *)&UFCON0 - (void *)&ULCON0)
#define UBRDIV		((port)->iobase + (void *)&UBRDIV0 - (void *)&ULCON0)

#define RFCNT_MASK	0x0f
#define TFCNT_MASK	0xf0
#define TFULL		0x200

#define UART_OE		1
#define UART_PE		2
#define UART_FE		3
#define UART_BE		4
#define UART_ANY_ERR	(UART_OE|UART_PE|UART_FE)

#define UART_RTS	1
#define UART_CTS	1
#define UCON_BRK	0x10

#define ULCON_CS5	0
#define ULCON_CS6	1
#define ULCON_CS7	2
#define ULCON_CS8	3
#define ULCON_STOPB	4
#define ULCON_PARITY	0x20
#define ULCON_PEVEN	0x08


static void s3c44b0uart_stop_tx(struct uart_port *port, u_int from_tty)
{
	outl(0x85, UCON);
	INT_DISABLE(port->irq);
}

static void s3c44b0uart_start_tx(struct uart_port *port, u_int nonempty, u_int from_tty)
{
	if(nonempty){
		outl(0x285, UCON);
		INT_ENABLE(port->irq);
	}
}

static void s3c44b0uart_stop_rx(struct uart_port *port)
{
	INT_DISABLE(port->irq+4);
}

static void s3c44b0uart_enable_ms(struct uart_port *port)
{
}

static void
s3c44b0uart_rx_int(int irq, void *dev_id, struct pt_regs *regs)
{
	struct uart_info *info = dev_id;
	struct tty_struct *tty = info->tty;
	unsigned int status, uer, ch, flg, ignored = 0;
	struct uart_port *port = info->port;
	
	status = inl(UFSTAT);
	while(status&RFCNT_MASK){
		uer = inl(UERSTAT);
		ch = inb(URXH);

		if(tty->flip.count >= TTY_FLIPBUF_SIZE)
			goto ignore_char;
		port->icount.rx++;

		flg = TTY_NORMAL;

		if(uer&UART_ANY_ERR)
			goto handle_error;
		if(uart_handle_sysrq_char(info, ch, regs))
			goto ignore_char;

	error_return:
		*tty->flip.flag_buf_ptr++ = flg;
		*tty->flip.char_buf_ptr++ = ch;
		tty->flip.count++;
	ignore_char:
		status = inl(UFSTAT);
	}
out:
	tty_flip_buffer_push(tty);
	return;

handle_error:
	if(uer&UART_PE)
		port->icount.parity++;
	else if(uer&UART_FE)
		port->icount.frame++;
	if(uer&UART_OE)
		port->icount.overrun++;

	if(uer&port->ignore_status_mask){
		if( ++ignored > 100)
			goto out;
		goto ignore_char;
	}
	uer &= port->read_status_mask;

	if(uer&UART_PE)
		flg = TTY_PARITY;
	else if(uer&UART_FE)
		flg = TTY_FRAME;

        if (uer&UART_OE) {
		/*
		 * CHECK: does overrun affect the current character?
		 * ASSUMPTION: it does not.
		 */
                *tty->flip.flag_buf_ptr++ = flg;
                *tty->flip.char_buf_ptr++ = ch;
                tty->flip.count++;
                if (tty->flip.count >= TTY_FLIPBUF_SIZE)
                        goto ignore_char;
                ch = 0;
                flg = TTY_OVERRUN;
        }
#ifdef SUPPORT_SYSRQ
        info->sysrq = 0;
#endif
        goto error_return;

}

static void s3c44b0uart_tx_int(int irq, void *dev_id, struct pt_regs *regs)
{
	struct uart_info *info = dev_id;
	struct uart_port *port = info->port;

	if (port->x_char) {
		outb(port->x_char, UTXH);
		port->icount.tx++;
		port->x_char = 0;
		return;
	}
	if (info->xmit.head == info->xmit.tail
	    || info->tty->stopped
	    || info->tty->hw_stopped) {
		s3c44b0uart_stop_tx(port, 0);
		return;
	}

	while(!(inl(UFSTAT)&TFULL)){
		outb(info->xmit.buf[info->xmit.tail], UTXH);
		info->xmit.tail = (info->xmit.tail + 1) & (UART_XMIT_SIZE - 1);
		port->icount.tx++;
		if (info->xmit.head == info->xmit.tail)
			break;
	};

	if (CIRC_CNT(info->xmit.head,
		     info->xmit.tail,
		     UART_XMIT_SIZE) < WAKEUP_CHARS)
		uart_event(info, EVT_WRITE_WAKEUP);

	if (info->xmit.head == info->xmit.tail)
		s3c44b0uart_stop_tx(info->port, 0);

}

static u_int s3c44b0uart_tx_empty(struct uart_port *port)
{
	return (inl(UFSTAT)&TFCNT_MASK) ? 0 : TIOCSER_TEMT; 
}

static u_int s3c44b0uart_get_mctrl(struct uart_port *port)
{
	return (inl(UMSTAT)&UART_CTS) ? TIOCM_CTS : 0;
}

static void s3c44b0uart_set_mctrl(struct uart_port *port, u_int mctrl)
{
	unsigned int status;

	status = inl(UMCON);

	if(mctrl & TIOCM_RTS)
		status |= UART_RTS;
	else
		status &= ~UART_RTS;

	outl(status, UMCON);
}

static void s3c44b0uart_break_ctl(struct uart_port *port, int break_state)
{
	int status;

	status = inl(UCON);
	if(break_state)
		status |= UCON_BRK;
	else
		status &= ~UCON_BRK;
	outl(status, UCON);
}

static int s3c44b0uart_startup(struct uart_port *port, struct uart_info *info)
{
	int retv;

	retv = request_irq(port->irq, s3c44b0uart_tx_int, 0,
		"s3c44b0_uart_tx", info);
	if(retv)
		return retv;
	retv = request_irq(port->irq+4, s3c44b0uart_rx_int, 0,
		"s3c44b0_uart_rx", info);
	if(retv){
		free_irq(port->irq, info);
		return retv;
	}

	outl(0x17, UFCON);
	outl(0x85, UCON);
	INT_ENABLE(port->irq+4);

	return 0;
}

static void s3c44b0uart_shutdown(struct uart_port *port, struct uart_info *info)
{
	free_irq(port->irq, info);
	free_irq(port->irq+4, info);
	outl(0x00, UCON);
}

static void s3c44b0uart_change_speed(struct uart_port *port, u_int cflag, u_int iflag, u_int quot)
{
	unsigned int flags;
	unsigned int ulcon = 0;

	switch(cflag&CSIZE){
	case CS5: ulcon = ULCON_CS5; break;
	case CS6: ulcon = ULCON_CS6; break;
	case CS7: ulcon = ULCON_CS7; break;
	default:  ulcon = ULCON_CS8; break;
	}
	if(cflag&CSTOPB)
		ulcon |= ULCON_STOPB;
	if(cflag&PARENB){
		ulcon |= ULCON_PARITY;
		if(!(cflag&PARODD))
			ulcon |= ULCON_PEVEN;
	}

	port->read_status_mask = UART_OE;
	if(iflag&INPCK)
		port->read_status_mask |= UART_PE|UART_FE;

	port->ignore_status_mask = 0;
	if(iflag&IGNPAR)
		port->ignore_status_mask |= UART_PE|UART_FE;
	if(iflag&IGNBRK){
		port->ignore_status_mask |= UART_BE;
		if(iflag&IGNPAR)
			port->ignore_status_mask |= UART_OE;
	}

	save_flags_cli(flags);
	outl(ulcon, ULCON);
	outl(quot-1, UBRDIV);
	restore_flags(flags);

}

static const char *s3c44b0uart_type(struct uart_port *port)
{
	return port->type == PORT_S3C44B0 ? "S3C44B0" : NULL;
}

/*
 * Release the memory region(s) being used by 'port'
 */
static void s3c44b0uart_release_port(struct uart_port *port)
{
}

/*
 * Request the memory region(s) being used by 'port'
 */
static int s3c44b0uart_request_port(struct uart_port *port)
{
	return 0;
}

/*
 * Configure/autoconfigure the port.
 */
static void s3c44b0uart_config_port(struct uart_port *port, int flags)
{
	if(flags & UART_CONFIG_TYPE)
		port->type = PORT_S3C44B0;
}

/*
 * verify the new serial_struct (for TIOCSSERIAL).
 */
static int s3c44b0uart_verify_port(struct uart_port *port, struct serial_struct *ser)
{
	return 0;
}

static struct uart_ops s3c44b0_pops = {
	tx_empty:	s3c44b0uart_tx_empty,
	set_mctrl:	s3c44b0uart_set_mctrl,
	get_mctrl:	s3c44b0uart_get_mctrl,
	stop_tx:	s3c44b0uart_stop_tx,
	start_tx:	s3c44b0uart_start_tx,
	stop_rx:	s3c44b0uart_stop_rx,
	enable_ms:	s3c44b0uart_enable_ms,
	break_ctl:	s3c44b0uart_break_ctl,
	startup:	s3c44b0uart_startup,
	shutdown:	s3c44b0uart_shutdown,
	change_speed:	s3c44b0uart_change_speed,
	type:		s3c44b0uart_type,
	release_port:	s3c44b0uart_release_port,
	request_port:	s3c44b0uart_request_port,
	config_port:	s3c44b0uart_config_port,
	verify_port:	s3c44b0uart_verify_port,
};

static struct uart_port s3c44b0_ports[UART_NR] = {
	{
		iobase:		(void *)&ULCON0,
		irq:		INT_UTXD0,
		uartclk:	CONFIG_ARM_CLK,//60000000,
		fifosize:	16,
		ops:		&s3c44b0_pops,
		flags:		ASYNC_BOOT_AUTOCONF,
	},
	{
		iobase:		(void *)&ULCON1,
		irq:		INT_UTXD1,
		uartclk:	CONFIG_ARM_CLK,//60000000,
		fifosize:	16,
		ops:		&s3c44b0_pops,
		flags:		ASYNC_BOOT_AUTOCONF,
	}
};

#ifdef CONFIG_SERIAL_S3C44B0X_CONSOLE
static void s3c44b0uart_console_write(struct console *co, const char *s, u_int count)
{
	struct uart_port *port = s3c44b0_ports + co->index;
	unsigned int status, intmask;
	int i;

	intmask = INTMSK;
	INT_DISABLE(port->irq);
	INT_DISABLE(port->irq+4);

	for (i = 0; i < count; i++) {
		do {
			status = inl(UFSTAT);
		} while (status&TFULL);
		outb(s[i], UTXH);
		if (s[i] == '\n') {
			do {
				status = inl(UFSTAT);
			} while (status&TFULL);
			outb('\r', UTXH);
		}
	}

	do {
		status = inl(UFSTAT);
	} while (status&TFCNT_MASK);
	INTMSK = intmask;
}

static kdev_t s3c44b0uart_console_device(struct console *co)
{
	return MKDEV(SERIAL_S3C44B0_MAJOR, SERIAL_S3C44B0_MINOR+co->index);
}

static int s3c44b0uart_console_wait_key(struct console *co)
{
	struct uart_port *port = s3c44b0_ports + co->index;
	unsigned int status, ch;

	do{
		status = inl(UFSTAT);
	}while(!(status&RFCNT_MASK));
	ch = inb(URXH);

	return ch;
}

static void __init
s3c44b0uart_console_get_options(struct uart_port *port, int *baud, int *parity, int *bits)
{
	unsigned int status, quot;

	status = inl(UCON);
	if(!status)
		return;

	status = inl(ULCON);
	*bits = (status&3)+5;

	*parity = 'n';
	if(status&ULCON_PARITY){
		if(status&ULCON_PEVEN)
			*parity = 'e';
		else
			*parity = 'o';
	}

	quot = inl(UBRDIV);
	*baud = port->uartclk/(16*(quot+1));

	if(*baud>100000 && *baud<130000){
		*baud = 115200;
		return;
	}
	if(*baud>200000 && *baud<250000){
		*baud = 230400;
		return;
	}
	if(*baud>51000 && *baud<66000){
		*baud = 57600;
		return;
	}
	if(*baud>34000 && *baud<42000){
		*baud = 38400;
		return;
	}
	if(*baud>17000 && *baud<22000){
		*baud = 19200;
		return;
	}
	if(*baud>8400 && *baud<11000){
		*baud = 9600;
		return;
	}
	if(*baud>4300 && *baud<5300){
		*baud = 4800;
		return;
	}
	if(*baud>2100 && *baud<2700){
		*baud = 2400;
		return;
	}
	if(*baud>1000 && *baud<1300){
		*baud = 1200;
		return;
	}
}

static int __init s3c44b0uart_console_setup(struct console *co, char *options)
{
	struct uart_port *port;
	int baud = CONFIG_S3C44B0_DEFAULT_BAUDRATE;
	int bits = 8;
	int parity = 'n';
	int flow = 'n';

	/*
	 * Check whether an invalid uart number has been specified, and
	 * if so, search for the first available port that does have
	 * console support.
	*/
	port = uart_get_console(s3c44b0_ports, UART_NR, co);


	if (options)
		uart_parse_options(options, &baud, &parity, &bits, &flow);
	else
		s3c44b0uart_console_get_options(port, &baud, &parity, &bits);

	outl(0x97, UFCON);
	outl(0x05, UCON);
	return uart_set_options(port, co, baud, parity, bits, flow);
/*
	outl(0x97, UFCON);
	outl(0x03, ULCON);
	outl(0x20, UBRDIV);
	outl(0x05, UCON);
*/
        return 0;
}

static struct console s3c44b0_console = {
	name:		"ttyS",
	write:		s3c44b0uart_console_write,
	device:		s3c44b0uart_console_device,
//	wait_key:	s3c44b0uart_console_wait_key,
	setup:		s3c44b0uart_console_setup,
	flags:		CON_PRINTBUFFER,
	index:		-1,
};

void __init s3c44b0x_console_init(void)
{
	register_console(&s3c44b0_console);
}

#define S3C44B0_CONSOLE	&s3c44b0_console
#else
#define S3C44B0_CONSOLE	NULL
#endif

static struct uart_driver s3c44b0_reg = {
	owner:			THIS_MODULE,
	normal_major:		SERIAL_S3C44B0_MAJOR,
#ifdef CONFIG_DEVFS_FS
	normal_name:		"ttyS%d",
	callout_name:		"cua%d",
#else
	normal_name:		"ttyS",
	callout_name:		"cua",
#endif
	normal_driver:		&normal,
	callout_major:		CALLOUT_S3C44B0_MAJOR,
	callout_driver:		&callout,
	table:			s3c44b0_table,
	termios:		s3c44b0_termios,
	termios_locked:		s3c44b0_termios_locked,
	minor:			SERIAL_S3C44B0_MINOR,
	nr:			UART_NR,
	port:			s3c44b0_ports,
	cons:			S3C44B0_CONSOLE,
};

static int __init s3c44b0uart_init(void)
{
	return uart_register_driver(&s3c44b0_reg);
}

static void __exit s3c44b0uart_exit(void)
{
	uart_unregister_driver(&s3c44b0_reg);
}

module_init(s3c44b0uart_init);
module_exit(s3c44b0uart_exit);

EXPORT_NO_SYMBOLS;

MODULE_AUTHOR("TPU");
MODULE_DESCRIPTION("ARM S3C44B0 serial port driver");
MODULE_LICENSE("GPL");
