/*
 * console.c
 * 
 * utility to print things on console
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
void newline(void)
{
    char buf = 0x0d;
    while(write(fd_uart, &buf, 1) < 0) usleep(0);    
}

//-------------------------------------------------------------
void printHex(unsigned int value, int precision)
{
    char hex[5];
    int2hexString(hex, value, precision);
    int number = sprintf(hex, "%s", hex);
    while(write(fd_uart, hex, number) < 0) usleep(0);
}

//-------------------------------------------------------------
void printStr(char* str)
{
    while(write(fd_uart, str, strlen(str)) < 0) usleep(0);   
}

//-------------------------------------------------------------
void printDec(unsigned int value)
{
    char buf[6];
    int number = sprintf(buf, "%d", value);
    while(write(fd_uart, buf, number) < 0) usleep(0);      
}

//-------------------------------------------------------------
void printMACAdress(void *addr)
{
    int k;
    for(k=0; k<5; k++)
    {
        printHex( ((__u8*)addr)[k], 2);
        printStr("-");
    }
    printHex( ((__u8*)addr)[k], 2);
    newline();   
}

//-------------------------------------------------------------
void printMACHeader(void *pheader)  
{
    printStr("DA: ");
    printMACAdress((__u8*)pheader);

    printStr("SA: ");
    printMACAdress((__u8*)(pheader+6));

    printStr("TL: ");
    __u8* addr = (__u8*)(pheader+12);
    printHex( addr[0], 2);
    printHex( addr[1], 2);
    newline();
}

//-------------------------------------------------------------
void printPayload(void *addr, int len, int* pos)
{
    int k;
    for(k=0; k<len; k++, (*pos)++){
        printHex( ((__u8*)addr)[k], 2);
        switch((*pos)%16){
            case 3: case 7: case 11: 
                printStr(" ");
                break;
            case 15:
                newline();
                break;
        }
    }   
}

//-------------------------------------------------------------
void printIPAdress(void *addr)
{
    int k;
    for(k=0; k<3; k++)
    {
        printDec( ((__u8*)addr)[k]);
        printStr(".");
    }
    printDec( ((__u8*)addr)[k]);
    newline();    
}
