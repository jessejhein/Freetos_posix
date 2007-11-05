/*
 * autoscale.c
 */

#include <stdio.h>

/*
 * Function:
 * 
 * 
 * Output:      >0 length of string
 *              -1 overflow, let user to decide what to print
 *              -2 underrun, let user to decide what to print
 *              
 * 
 * 
 */
int autoscale(char* buf, float value, char* unit){
    int len = 0;
    if(value > 99999.4)         len = sprintf(buf, "++.++");
    else if(value > 999.94)     len = sprintf(buf, "%5.0f", value);     //XXXXX
    else if(value > 99.994)     len = sprintf(buf, "%5.1f", value);     //XXX.X
    else if(value > 9.9994)     len = sprintf(buf, "%5.2f", value);     //XX.XX
    else if(value >= 0)         len = sprintf(buf, "%5.3f", value);     //X.XXX
    else if(value > -0.005)     len = sprintf(buf, "%s", "0.000");      //0.000
    else if(value > -9.995)     len = sprintf(buf, "%+5.2f", value);    //-X.XX
    else if(value > -99.95)     len = sprintf(buf, "%+5.1f", value);    //-XX.X
    else if(value > -9999.5)    len = sprintf(buf, "%+5.0f", value);    //-XXXX
    else                        len = sprintf(buf, "--.--");             //--
    if(unit != NULL){
        return sprintf(buf+len, "%s", unit) + len;
    }
    return len;
}
