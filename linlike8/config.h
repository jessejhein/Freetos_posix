/*
 * header of configuration for linlike8
 * i.e. Parameters
 *
 */

// scheduling ===========================================================
#define	SWITCH_CASE		1					// 1 as turn ON switch case mode, otherwise 0

// uart module ===========================================================
#define	SERIAL_MOD					1
#if (SERIAL_MOD>0)
//		uart buffer size
	#define	MAX_SERIAL_BUF			12		// must >= 9 in laser control application
	#define	MAX_SERIAL_TX_BUF		5		// >= 2
#endif
