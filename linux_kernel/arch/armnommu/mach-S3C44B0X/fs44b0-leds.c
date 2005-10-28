/*
 * LED driver for the Atmel AT91RM9200 Development Kit.
 *
 * (c) SAN People (Pty) Ltd
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
*/

//added by antiscle

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

#include <asm/mach-types.h>
#include <asm/leds.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/arch/hardware.h>

//#ifdef CONFIG_LEDS
//void (*leds_event)(led_event_t);
//#endif

static inline void leds_off(void)
{
//	outl(inl(S3C44B0X_PDATE)&~(3<<2), S3C44B0X_PDATE);
	outl(inl(S3C44B0X_PDATC)&~(1<<1), S3C44B0X_PDATC);
}

static inline void leds_on(void)
{
//	outl(inl(S3C44B0X_PDATE)|(3<<2), S3C44B0X_PDATE);
	outl(inl(S3C44B0X_PDATC)|(1<<1), S3C44B0X_PDATC);
}

static inline void timer_led_toggle(void)
{
//	outl(inl(S3C44B0X_PDATE)^(1<<2), S3C44B0X_PDATE);
	outl(inl(S3C44B0X_PDATC)^(1<<1), S3C44B0X_PDATC);
}

static inline void busy_led_off(void)
{
//	outl(inl(S3C44B0X_PDATE)&~(1<<3), S3C44B0X_PDATE);
	outl(inl(S3C44B0X_PDATC)&~(1<<2), S3C44B0X_PDATC);
}

static inline void busy_led_on(void)
{
//	outl(inl(S3C44B0X_PDATE)|(1<<3), S3C44B0X_PDATE);
	outl(inl(S3C44B0X_PDATC)|(1<<2), S3C44B0X_PDATC);
}

/*
 * Handle LED events.
 */
static void fs44b0_leds_event(led_event_t evt)
{
	unsigned long flags;
		
	local_irq_save(flags);

	switch(evt) {
	case led_start:		/* System startup */
		leds_on();
		break;

	case led_stop:		/* System stop / suspend */
		leds_off();
		break;

#ifdef CONFIG_LEDS_TIMER
	case led_timer:		/* Every 50 timer ticks */
		timer_led_toggle();
		break;
#endif

#ifdef CONFIG_LEDS_CPU
	case led_idle_start:	/* Entering idle state */
		busy_led_off();
		break;

	case led_idle_end:	/* Exit idle state */
		busy_led_on();
		break;
#endif

	default:
		break;
	}

	local_irq_restore(flags);
}


static int __init leds_init(void)
{
	/* Enable PIO to access the LEDs
	*outl((inl(S3C44B0X_PCONE)&~(0xf0))|0x50, S3C44B0X_PCONE);
	*/
//       outl(0x15569, S3C44B0X_PCONE);
	outl((inl(S3C44B0X_PCONC)&~(0x3c))|0x14, S3C44B0X_PCONC);
 
#ifdef CONFIG_LEDS_TIMER
//	printk("LEDS_TIMER init\n");
#endif
#ifdef CONFIG_LEDS_CPU
//	printk("LEDS_CPU init\n");
#endif
	////leds_event = fs44b0_leds_event;

	////leds_event(led_start);
	leds_off();
	busy_led_off();
	return 0;
}

__initcall(leds_init);
