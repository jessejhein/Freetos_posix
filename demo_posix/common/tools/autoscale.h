/*
 * autoscale.h
 */


#ifndef _AUTOSCALE_H_
#define _AUTOSCALE_H_

/**************************************************************************************
 * Function:    int autoscale(char* buf, float value, char* unit);
 * 
 * Description: auto-shifting of decimal point for 5 character space (exc. unit)
 *              when        value> 99999  ->  ++.++
 *              when 99.99< value<=999.9  ->  XXX.X
 *              when 9.999< value<=99.99  ->  XX.XX
 *              when     0<=value<=9.999  ->  X.XXX
 *              when -9.99<=value< 0      ->  -X.XX
 *              when -99.9<=value< -9.99  ->  -XX.X
 *              when -9999<=value< -99.9  ->  -XXXX
 *              when -9999< value         ->  --.--
 *
 * Input:       buf:    buffer to print to
 *              value:  value to print
 *              unit:   units to print (NULL if no unit)
 * 
 * Output:      length of characters in buf (5 characters + units)
 *  
 **************************************************************************************/
extern int autoscale(char* buf, float value, char* unit);

#endif /* AUTOSCALE */
