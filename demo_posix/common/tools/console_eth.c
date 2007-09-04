/*
 * console_eth.c
 * 
 * utility to print ethernet related things on console
 */

#include <define.h>
#include <unistd.h>
#include <console.h>

//-------------------------------------------------------------
void printMACAdress(void *addr)
{
    int k;
    for(k=0; k<5; k++)
    {
        while(printHex( ((__u8*)addr)[k], 2) < 0) usleep(0);
        while(printStr("-") < 0) usleep(0);
    }
    while(printHex( ((__u8*)addr)[k], 2) < 0) usleep(0);
    while(newline() < 0) usleep(0);   
}

//-------------------------------------------------------------
void printMACHeader(void *pheader)  
{
    while(printStr("DA: ") < 0) usleep(0);
    printMACAdress((__u8*)pheader) < 0);

    while(printStr("SA: ") < 0) usleep(0);
    printMACAdress((__u8*)(pheader+6));

    while(printStr("TL: ") < 0) usleep(0);
    __u8* addr = (__u8*)(pheader+12);
    while(printHex( addr[0], 2) < 0) usleep(0);
    while(printHex( addr[1], 2) < 0) usleep(0);
    while(newline() < 0) usleep(0);
}

//-------------------------------------------------------------
void printPayload(void *addr, int len, int* pos)
{
    int k;
    for(k=0; k<len; k++, (*pos)++){
        while(printHex( ((__u8*)addr)[k], 2) < 0) usleep(0);
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

//-------------------------------------------------------------
void printIPAdress(void *addr)
{
    int k;
    for(k=0; k<3; k++)
    {
        while(printDec( ((__u8*)addr)[k]) < 0) usleep(0);
        printStr(".");
    }
    while(printDec( ((__u8*)addr)[k]) < 0) usleep(0);
    while)(newline() < 0) usleep(0);    
}
