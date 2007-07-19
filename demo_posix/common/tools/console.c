/*
 * console.c
 * 
 * utility to print things on console
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    int i=0;
    for(i=0; i<precision; i++){
        __u16 remainder = value%16;
        switch(remainder){
            case 0x00: buf[precision-1-i] = '0'; break;
            case 0x01: buf[precision-1-i] = '1'; break;
            case 0x02: buf[precision-1-i] = '2'; break;
            case 0x03: buf[precision-1-i] = '3'; break;
            case 0x04: buf[precision-1-i] = '4'; break;
            case 0x05: buf[precision-1-i] = '5'; break;
            case 0x06: buf[precision-1-i] = '6'; break;
            case 0x07: buf[precision-1-i] = '7'; break;
            case 0x08: buf[precision-1-i] = '8'; break;
            case 0x09: buf[precision-1-i] = '9'; break;
            case 0x0A: buf[precision-1-i] = 'A'; break;
            case 0x0B: buf[precision-1-i] = 'B'; break;
            case 0x0C: buf[precision-1-i] = 'C'; break;
            case 0x0D: buf[precision-1-i] = 'D'; break;
            case 0x0E: buf[precision-1-i] = 'E'; break;
            case 0x0F: buf[precision-1-i] = 'F'; break;
            default: buf[precision-1-i] = '*';
        }
        value /=16;
    }
    buf[precision] = 0x00;
}

//-------------------------------------------------------------
void newline(void)
{
    char buf = 0x0d;
    write(fd_uart, &buf, 1);    
}

//-------------------------------------------------------------
void printHex(unsigned int value, int precision)
{
    char hex[5];
    int2hexString(hex, value, precision);
    int number = sprintf(hex, "%s", hex);
    write(fd_uart, hex, number);
    mdelay(5);    
}

//-------------------------------------------------------------
void printStr(char* str)
{
    write(fd_uart, str, strlen(str));   
    mdelay(10);       
}

//-------------------------------------------------------------
void printDec(unsigned int value)
{
    char buf[6];
    int number = sprintf(buf, "%d", value);
    write(fd_uart, buf, number);
    mdelay(5);        
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
