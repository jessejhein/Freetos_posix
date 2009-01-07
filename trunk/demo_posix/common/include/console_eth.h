/*
 * console_eth.h
 * 
 * utility to print ethernet related things on console
 * 
 */

#ifndef CONSOLE_ETH_H_
#define CONSOLE_ETH_H_

/*
 * Print an mac address 
 * e.g. 00-06-0E-00-00-01
 */
void printMACAdress(void *addr);


/*
 * Print an ethernet address 
 * e.g. DA: FF-FF-FF-FF-FF-FF
 *      SA: 00-06-0E-00-00-01
 *      TL: 0800
 */
void printMACHeader(void *pheader); 


/*
 * Print the payload of ethernet packet
 * e.g. 00112233 44556677 8899AABB CCDDEEFF
 *      00112233 44556677 8899AABB CCDDEEFF
 *      00112233 4455
 */
void printPayload(void *addr, int len, int* pos);


/*
 * Print an ip address 
 * e.g. 192.168.1.63
 */
void printIPAdress(void *addr);

#endif /* CONSOLE_ETH_H_ */
