/*
 * arch/cypress_psoc/kernel/setup.c
 *
 * 24th June 2005	yan	create this following linux, init. this arch 
 *
 * init. the cpu core, and all mcu peripheral
 */

// including, get data linking from others ===============================================
//	linlike8 os
#include <linlike8/config.h>
#if (EEPROM_MOD==1)
//	eeprom
#include <linlike8/eeprom.h>
#endif
#if (UART_MOD==1)
//	eeprom
#include <linlike8/uart.h>
#endif

// methods ==============================================================================

void setup_arch(void)
{
#if (EEPROM_MOD==1)
	eeprom_init();
#endif
#if (UART_MOD==1)
	uart_open();
#endif
}

