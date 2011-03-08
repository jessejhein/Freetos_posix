/**
 * \file
 * Defines prototypes for those network library routines that convert Internet address and 
 * dotted-decimal notation, for example, inet_makeaddr().
 * \author Dennis Tsang <dennis@amonics.com>
 */

#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <uip.h>
#include <stdio.h>

//---------------------------------------------------------------------------
int 
inet_aton (const char *name, struct in_addr *addr)
{
  char* ans;
  char ipaddr[16];
  int i = 0;
  if (strlen (name) < 16)
    {
      //strtok cannot work on const char*
      strcpy (ipaddr, name);
      ans = strtok (ipaddr, ".");
      while ((ans != NULL) && (i < 4))
        {
          int val = atoi (ans);
          if ((val < 0x00) || (val > 0xFF)) return 0;
          addr->s_addr[i++] = (unsigned char) val;
          ans = strtok (NULL, ".");
        }
      return 1;
    }
  return 0;
}


//---------------------------------------------------------------------------
static char inet_ipaddr[16];
char* 
inet_ntoa (struct in_addr addr)
{
  sprintf (inet_ipaddr, "%d.%d.%d.%d", (int)addr.s_addr[0], (int)addr.s_addr[1], (int)addr.s_addr[2], (int)addr.s_addr[3]);
  return inet_ipaddr;
}

