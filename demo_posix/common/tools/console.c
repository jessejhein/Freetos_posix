/*
 * console_noblock.c
 * 
 * utility to print things on console in non-block mode
 */

#include <define.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <asm/delay.h>
#include <asm/types.h>
#include <stdio.h>

//#define fd_uart fd_hc

extern int fd_uart;

/*
#include <stdarg.h>
void tprint(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    while (*fmt)
    {
        switch (*fmt)
        {
            case '%':
                fmt++;
                if (*fmt == 'd')
                {
                    int d = va_arg(ap, int);
                    sprintf("<%d> is an integer\n",d);
                }
                else if (*fmt == 's')
                {
                    char *s = va_arg(ap, char*);
                    write(fd_uart, str, strlen(str));
                }
                else
                {
                    printf("%%%c is an unknown format\n", *fmt);
                }
                fmt++;
                break;
            default:
                printf("%c is unknown\n", *fmt);
                fmt++;
                break;
        }
    }
    va_end(ap);
}
*/

/*
 * convert integer value to hex string
 * value: a value
 * precision: number of digits 
 */
static void int2hexString(char* buf, u16 value, int precision)
{
    char ascii[] = "0123456789ABCDEF";
    int i=0;
    for(i=0; i<precision; i++){
        u16 remainder = value%16;
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
//-------------------------------------------------------------
int printFloat(float value)
{
    char buf[20];
    int number = sprintf(buf, "%f", value);
    return write(fd_uart, buf, number);      
}

//-------------------------------------------------------------
void printMACAdress(u8 *addr)
{
    int k;
    for(k=0; k<5; k++)
    {
        while(printHex( addr[k], 2) < 0) usleep(0);
        while(printStr("-") < 0) usleep(0);
    }
    while(printHex( addr[k], 2) < 0) usleep(0);
    while(newline() < 0) usleep(0);   
}

//-------------------------------------------------------------
void printMACHeader(u8 *pheader)  
{
    while(printStr("DA: ") < 0) usleep(0);
    printMACAdress(pheader);

    while(printStr("SA: ") < 0) usleep(0);
    printMACAdress(pheader+6);

    while(printStr("TL: ") < 0) usleep(0);
    u8* addr = pheader+12;
    while(printHex( addr[0], 2) < 0) usleep(0);
    while(printHex( addr[1], 2) < 0) usleep(0);
    while(newline() < 0) usleep(0);
}

//-------------------------------------------------------------
void printPayload(void *addr, int len, int* pos)
{
    int k;
    for(k=0; k<len; k++, (*pos)++){
        while(printHex( ((u8*)addr)[k], 2) < 0) usleep(0);
        switch((*pos)%16){
            case 3: case 7: case 11: 
                while(printStr(" ") < 0) usleep(0);
                break;
            case 15:
                while(newline() < 0) usleep(0);
                break;
        }
    }   
}
