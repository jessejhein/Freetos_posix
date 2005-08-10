/*
 *	arch/cypress_psoc/kernel/setup.c
 *
 *	09-04-2004	yan	create this following linux, init. this arch 
 *
 */

#include <linlike8/config.h>
#include "app.h"// may be remove later 

extern void gpio_open(void);
#if (UART_MOD==1)
extern void uart_open(void);
#endif
#if (I2C_MOD==1)
extern void i2c_open(void);
#endif
//#if (KB_MOD==1)
//extern void kb_open(void);
//#endif

void setup_arch(void)
{
	gpio_open();
//#if (KB_MOD==1)
//	kb_open();
//#endif
#if (UART_MOD==1)
	uart_open();
#endif
#if (I2C_MOD==1)
	i2c_open();
#endif

}

