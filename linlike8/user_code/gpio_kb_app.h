/*
 *	linlike8/test_apps/gpio_kb_app.h
 *
 *	14-04-2004	yan	gpio module for keyboard application part
 *
 */

#define	APP_UP_KEY_DOWN_CASE	do {							\
		PRT1DR |= 0x04;\
	} while (0)
#define	APP_DOWN_KEY_DOWN_CASE	do {							\
		PRT1DR |= 0x08;\
	} while (0)
#define	APP_ENTER_KEY_DOWN_CASE	do {							\
		PRT1DR |= 0x10;\
	} while (0)
#define	APP_UP_KEY_UP_CASE	do {							\
		PRT1DR |= 0x20;\
	} while (0)
#define	APP_DOWN_KEY_UP_CASE	do {							\
		PRT1DR |= 0x40;\
	} while (0)
#define	APP_ENTER_KEY_UP_CASE	do {							\
		if (RDI1RO0&0x10) {							\
                        RDI1RO0 &= ~0x10;						\
                        GDI_E_IN |= 0x02;       					\
		} else {								\
                        GDI_E_IN &= ~0x02;      					\
			RDI1RO0 |= 0x10;        					\
		}									\
	} while (0)									// internal source disconnect to gpio pin
											// gpio input to gpio output pin
											// gpio input disconnect to gpio output pin
											// internal source to gpio pin
//		PRT1DR |= 0x80;\

