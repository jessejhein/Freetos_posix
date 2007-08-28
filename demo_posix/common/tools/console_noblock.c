/*
 * console_noblock.c
 * 
 * utility to print things on console in non-block mode
 */

#include <define.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <asm/delay.h>
#include <asm/types.h>

extern int fd_uart;

/*
 * convert integer value to hex string
 * value: a value
 * precision: number of digits 
 */
static void int2hexString(char* buf, __u16 value, int precision)
{
    char ascii[] = "0123456789ABCDEF";
    int i=0;
    for(i=0; i<precision; i++){
        __u16 remainder = value%16;
        buf[precision-1-i] = ascii[remainder];
        value /=16;
    }
    buf[precision] = 0x00;
}

//-------------------------------------------------------------
int newline(void)
{
    char buf = 0x0d;
    return write(fd_uart, &buf, 1);    
}

//-------------------------------------------------------------
int printHex(unsigned int value, int precision)
{
    char hex[5];
    int2hexString(hex, value, precision);
    int number = sprintf(hex, "%s", hex);
    return write(fd_uart, hex, number);
}

//-------------------------------------------------------------
int printStr(char* str)
{
    return write(fd_uart, str, strlen(str));   
}

//-------------------------------------------------------------
int printDec(unsigned int value)
{
    char buf[6];
    int number = sprintf(buf, "%d", value);
    return write(fd_uart, buf, number);      
}
