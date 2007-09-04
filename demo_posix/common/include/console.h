/*
 * console.h
 * 
 * utility to print things on console
 * 
 * "int fd_uart" must be defined in application 
 */

#ifndef CONSOLE_H_
#define CONSOLE_H_

/*
 * Display a newline
 */
int newline(void);


/*
 * Display a string of characters
 */
int printStr(char* str);


/*
 * Display a hex value upto 16-bit
 */
int printHex(unsigned int value, int precision);


/*
 * Display a decimal value
 */
int printDec(unsigned int value);


/*
 * Display a floating point value
 */
int printFloat(float value);

#endif /* CONSOLE_H_ */
