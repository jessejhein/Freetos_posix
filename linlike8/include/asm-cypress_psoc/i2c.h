/*
 * Copyright (c) 10-06-2004 cheng chung yan <yan@amonics.com> for 8-bit system
 *
 * header of i2c
 *
 *	3-11-2004	hei modified for the i2c.c
 */

// return 
//	the length of data to be sent, 0 as empty data can be sent
extern unsigned char i2c_write(unsigned char *__buf, unsigned char __n
#if (I2C_MASTER==1||I2C_MULTI_MASTER==1)
, unsigned char slave_addr
#endif
);
// return 
//	the length of data received, 0 as empty data received
extern unsigned char i2c_read(unsigned char *__buf, unsigned char __n
#if (I2C_MASTER==1||I2C_MULTI_MASTER==1)
, unsigned char slave_addr
#endif
);
//return
//If the return value is 1 it is not complete; if the return value is 0, it is complete 
extern unsigned char i2c_write_complete(unsigned char *__buf, unsigned char __n
#if (I2C_MASTER==1||I2C_MULTI_MASTER==1)
, unsigned char slave_addr
#endif
);
//return
//If the return value is 1 it is not complete; if the return value is 0, it is complete 
extern unsigned char i2c_read_complete(unsigned char *__buf, unsigned char __n
#if (I2C_MASTER==1||I2C_MULTI_MASTER==1)
, unsigned char slave_addr
#endif
);