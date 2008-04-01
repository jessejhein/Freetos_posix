/*
 * inet.c
 */

#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <uip.h>

static char inet_ipaddr[16];

/******************************************************************************
 * Function:        int inet_aton(const char *name, struct in_addr *addr)
 * 
 * Description:     converts the Internet host address name from the standard 
 *                  numbers-and-dots notation into binary data and stores it in 
 *                  the struct in_addr that addr points to.
 * 
 * Input:           *name: ip address in string format
 *                  *addr: ip address in binary format
 * 
 * Output:          0, not valid
 *                  1, valid
 ******************************************************************************/
int inet_aton(const char *name, struct in_addr *addr){
    char* ans;
    int i = 0;
    if(strlen(name)<16){
        //strtok cannot work on const char*
        strcpy(inet_ipaddr, name);
        ans = strtok(inet_ipaddr, ".");
        while (ans != NULL && i<4)
        {
            int val = atoi(ans);
            if(val<0x00 || val>0xFF) return 0;
            addr->s_addr[i++] = (unsigned char) val;
            ans = strtok(NULL, ".");
        }
        return 1;
    }
    return 0;
}


/******************************************************************************
 * Function:        char* inet_ntoa(struct in_addr addr)
 * 
 * Description:     converts the Internet host address addr to a string in the 
 *                  standard numbers-and-dots notation. 
 * 
 * Input:           *addr: ip address in binary format
 * 
 * Output:          a pointer into a statically-allocated buffer.
 ******************************************************************************/
char* inet_ntoa(struct in_addr addr){
    sprintf(inet_ipaddr, "%d.%d.%d.%d", (int)addr.s_addr[0], (int)addr.s_addr[1], (int)addr.s_addr[2], (int)addr.s_addr[3]);
    return inet_ipaddr;
}

