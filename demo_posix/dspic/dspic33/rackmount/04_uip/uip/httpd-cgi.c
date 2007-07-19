/**
 * \addtogroup httpd
 * @{
 */

/**
 * \file
 *         Web server script interface
 * \author
 *         Adam Dunkels <adam@sics.se>
 *
 */

/*
 * Copyright (c) 2001-2006, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: httpd-cgi.c,v 1.2 2006/06/11 21:46:37 adam Exp $
 *
 */

#include "uip.h"
#include "uip_arp.h"
#include "psock.h"
#include "httpd.h"
#include "httpd-cgi.h"
#include "httpd-fs.h"

#include <laser_ctrl.h>

#include <stdio.h>
#include <string.h>

/*****************************************************************************
 * CGI Functions Declaration
 *****************************************************************************/
HTTPD_CGI_CALL(laser_html, "laser-cgi", laser_cgi);
HTTPD_CGI_CALL(about_html, "about-cgi", about_cgi);

static const struct httpd_cgi_call *calls[] = { &laser_html, &about_html, NULL };

/*****************************************************************************
 * CGI Key Maps
 * +--Laser Control cgi coding is represented by 2 digits
 *  +-----------+----------+---------------------------+
 *  | 1st Digit | Action   | 2nd Digit                 |
 *  +-----------+----------+---------------------------+
 *  |    '0'    | status   | {0, 1} = {laser0, laser1} |
 *  |    '1'    | cur set  | {0, 1} = {laser0, laser1} |
 *  |    '2'    | tec      | {0, 1} = {laser0, laser1} |
 *  |    '3'    | pwr      | {0, 1} = {output, input}  |
 *  +-----------+----------+---------------------------+
 *  
 * +--About cgi coding is represented by 1
 *  +-----------+----------+
 *  | 1st Digit | Action   |
 *  +-----------+----------+
 *  |    '0'    | MAC      |
 *  |    '1'    | IP       |
 *  |    '2'    | SUBNET   |
 *  |    '3'    | GATEWAY  |
 *  +-----------+----------+
 *****************************************************************************/
//laser-cgi
#define STATUS_CGI          0
#define CUR_SET_CGI         1         
#define TEC_CGI             2
#define POWER_CGI           3
//about-cgi
#define MAC_ADDR_CGI        0
#define IP_ADDR_CGI         1
#define SUBNET_MASK_CGI     2
#define GATEWAY_CGI         3

/*****************************************************************************
 * Keeps the following two functions
 *****************************************************************************/
static
PT_THREAD(nullfunction(struct httpd_state *s, char *ptr))
{
  PSOCK_BEGIN(&s->sout);
  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
httpd_cgifunction
httpd_cgi(char *name)
{
  const struct httpd_cgi_call **f;
  
  /* Find the matching name in the table, return the function. */
  for(f = calls; *f != NULL; ++f) {
    if(strncmp((*f)->name, name, strlen((*f)->name)) == 0) {
      return (*f)->function;
    }
  }
  return nullfunction;
}

/*****************************************************************************
 * Functions to handle laser-cgi
 *****************************************************************************/
static unsigned short
generate_laser_cgi(void *arg)
{
  char *f = (char *)arg;
  unsigned short ans;
  unsigned char offset = f[1] - '0';

  switch(f[0] - '0'){
    case STATUS_CGI:
        ans = snprintf((char *)uip_appdata, UIP_APPDATA_SIZE, "%s", 
                (laser_ctrl.enable[offset] == 0)? "OFF" : "ON" );
        break;
    case CUR_SET_CGI:
        ans = snprintf((char *)uip_appdata, UIP_APPDATA_SIZE, "%5.1f", laser_set.current_set[offset]);        
        break;
    case TEC_CGI:
        ans = snprintf((char *)uip_appdata, UIP_APPDATA_SIZE, "%5.1f", laser_get.tec[offset]); 
        break;
    case POWER_CGI:
        ans = snprintf((char *)uip_appdata, UIP_APPDATA_SIZE, "%5.1f", laser_get.power[offset]);
        break;
    default:
        ans = 0;
  }
  return ans;
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(laser_cgi(struct httpd_state *s, char *ptr))
{
  PSOCK_BEGIN(&s->sout);

  PSOCK_GENERATOR_SEND(&s->sout, generate_laser_cgi, strchr(ptr, ' ') + 1);

  PSOCK_END(&s->sout);
}

/*****************************************************************************
 * Functions to handle about-cgi
 *****************************************************************************/
static unsigned short
generate_about_cgi(void *arg)
{
  char *f = (char *)arg;  
  unsigned short ans;

  switch(f[0] - '0'){
    case MAC_ADDR_CGI:
        ans = snprintf((char *)uip_appdata, UIP_APPDATA_SIZE, 
                        "%2.2X-%2.2X-%2.2X-%2.2X-%2.2X-%2.2X",
                        ((unsigned char *)&uip_ethaddr)[0], 
                        ((unsigned char *)&uip_ethaddr)[1],
                        ((unsigned char *)&uip_ethaddr)[2],
                        ((unsigned char *)&uip_ethaddr)[3],
                        ((unsigned char *)&uip_ethaddr)[4],
                        ((unsigned char *)&uip_ethaddr)[5] ); 
        break;
    case IP_ADDR_CGI:
        ans = snprintf((char *)uip_appdata, UIP_APPDATA_SIZE, 
                        "%d.%d.%d.%d",
                        ((unsigned char *)&uip_hostaddr)[0], 
                        ((unsigned char *)&uip_hostaddr)[1],
                        ((unsigned char *)&uip_hostaddr)[2], 
                        ((unsigned char *)&uip_hostaddr)[3] );
        break;
    case SUBNET_MASK_CGI:
        ans = snprintf((char *)uip_appdata, UIP_APPDATA_SIZE, 
                        "%d.%d.%d.%d", 
                        ((unsigned char *)&uip_netmask)[0], 
                        ((unsigned char *)&uip_netmask)[1],
                        ((unsigned char *)&uip_netmask)[2], 
                        ((unsigned char *)&uip_netmask)[3] );
        break;
    case GATEWAY_CGI:
        ans = snprintf((char *)uip_appdata, UIP_APPDATA_SIZE, 
                        "%d.%d.%d.%d", 
                        ((unsigned char *)&uip_draddr)[0], 
                        ((unsigned char *)&uip_draddr)[1],
                        ((unsigned char *)&uip_draddr)[2], 
                        ((unsigned char *)&uip_draddr)[3] );
        break;
    default:
        ans = 0; 
  }
  return ans;

}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(about_cgi(struct httpd_state *s, char *ptr))
{
  PSOCK_BEGIN(&s->sout);

  PSOCK_GENERATOR_SEND(&s->sout, generate_about_cgi, strchr(ptr, ' ') + 1);

  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/

/** @} */
