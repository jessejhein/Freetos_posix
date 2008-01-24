/************************************************************************************************
 * File:            app_http.c
 * Description:     http server task employing posix thread and file API
 ***********************************************************************************************/

#include <define.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <console.h>

#include "uip.h"
#include "uip_arp.h"
#include "dhcpc.h"
#include "timer.h"

/************************************************************************************************
 * DEBUG
 ***********************************************************************************************/
#define DEBUG       0
#define DEBUG_DHCP  1
#define DEBUG_RX    0       //Enabling this will slow down communication significantly

/************************************************************************************************
 * Global Variables
 ***********************************************************************************************/
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])
int fd_eth;

/************************************************************************************************
 * Local Function
 ***********************************************************************************************/
#if(DEBUG_RX == 1)
static void print_pkt(void);
#endif
#if(DEBUG_DHCP == 1)
static void print_IPConfig(const struct dhcpc_state *s);
#endif
#define SEND_PKT(fd)     {    while( \
                                           (write(fd, uip_buf, sizeof(uip_buf)) < 0) \
                                        && (errno == EAGAIN) \
                                     ){ usleep(0); } \
                          }
static void eth_app_init(void);

/************************************************************************************************
 * Webserver Task
 ***********************************************************************************************/
void* tskHTTPServer(void* ptr)
{
    int i;
    int wr_status;
    uip_ipaddr_t ipaddr;
    struct timer periodic_timer, arp_timer;
    
    //----------------
    // Initialization
    //----------------
    while(1){
        fd_eth = open(ETHERNET, O_RDWR);
        if(fd_eth < 0){
            sleep(5);
        }
        else{
            timer_set(&periodic_timer, CLOCK_SECOND / 2);
            timer_set(&arp_timer, CLOCK_SECOND * 10);
            eth_app_init();
            break;
        }
    }
    
    //------------------
    // Tasks loops here
    //------------------
    start_process();
    
    while(1){
        //-------------------------------
        // Read and test for link status
        //-------------------------------
        int len = read(fd_eth, uip_buf, sizeof(uip_buf));
        if(len < 0)
        {
#if(DEBUG == 1)
            while(printStr("WARNING: Ethernet Link Broken!") < 0) usleep(0);
            while(newline() < 0) usleep(0);
#endif            
            while( (fd_eth = open(ETHERNET, O_RDWR)) < 0 ){
                uip_ipaddr(ipaddr, 0,0,0,0);
                uip_sethostaddr(ipaddr);
                uip_setnetmask(ipaddr);
                uip_setdraddr(ipaddr);                
                sleep(5);
            }
            eth_app_init();
#if(DEBUG == 1)
            while(printStr("Ethernet Link Re-established.") < 0) usleep(0);
            while(newline() < 0) usleep(0);
#endif            
            len = read(fd_eth, uip_buf, sizeof(uip_buf));
        }                                
        uip_len = (u16_t)len;

        //-----------------
        // Packet received
        //-----------------
        if(uip_len > 0)
        {
#if(DEBUG_RX == 1)
            print_pkt();
#endif
            if(BUF->type == htons(UIP_ETHTYPE_IP)){
                uip_arp_ipin();
                uip_input();
                if(uip_len > 0) {
                    uip_arp_out();
                    SEND_PKT(fd_eth);
                }
            } 
            else if(BUF->type == htons(UIP_ETHTYPE_ARP)) {
                uip_arp_arpin();
                if(uip_len > 0) {
                    SEND_PKT(fd_eth);
                }
            }
        }
        
        //----------------------------------------------
        // Periodic timer has expired, every 0.5 seconds
        //----------------------------------------------
        else if(timer_expired(&periodic_timer)) 
        {
            timer_reset(&periodic_timer);
            for(i = 0; i < UIP_CONNS; i++) {
                uip_periodic(i);
                if(uip_len > 0) {
                    uip_arp_out();
                    SEND_PKT(fd_eth);
                }
            }
    
#if UIP_UDP
            for(i = 0; i < UIP_UDP_CONNS; i++) {
                uip_udp_periodic(i);
                if(uip_len > 0) {
                    uip_arp_out();
                    SEND_PKT(fd_eth);
                }
            }
#endif /* UIP_UDP */
          
            // Call the ARP timer function every 10 seconds.
            if(timer_expired(&arp_timer)) {
                timer_reset(&arp_timer);
                uip_arp_timer();
#if(DEBUG == 1)
                while(printStr("ARP Timer reset.") < 0) usleep(0);
                while(newline() < 0) usleep(0);
#endif
            }
        }
        
        usleep(0);
    }
    
    end_process();   
}

static void eth_app_init(void)
{
    uip_init();
    dhcpc_init(&uip_ethaddr, 6);
    httpd_init();
}

/*************************************************************************
 * DHCP client will call this function after getting a valid ip address 
 *************************************************************************/
void dhcpc_configured(const struct dhcpc_state *s)
{
    uip_sethostaddr(s->ipaddr);
    uip_setnetmask(s->netmask);
    uip_setdraddr(s->default_router);
#if(DEBUG_DHCP == 1)
    print_IPConfig(s);
#endif    
}

/*
 * Print a packet stored in uip_buf on console
 */
#if(DEBUG_RX == 1)
static void print_pkt(void)
{
    int i, row;
    //Print packet content on console  
    if(uip_len > 0){
        while(printStr("Rx Packet: ") < 0) usleep(0);
        while(printDec((unsigned int) uip_len) < 0) usleep(0);
        while(newline() < 0) usleep(0);
        while(printStr("======================") < 0) usleep(0);
        while(newline() < 0) usleep(0); 
        printMACHeader((struct uip_eth_hdr*)uip_buf);
        printPayload(&uip_buf[UIP_LLH_LEN], uip_len-UIP_LLH_LEN, &row);
        while(newline() < 0) usleep(0);
        while(newline() < 0) usleep(0); 
    }    
}
#endif

/*
 * Print ip configuration
 */
#if(DEBUG_DHCP == 1)
void print_IPConfig(const struct dhcpc_state *s)
{
    while(printStr("DHCP Configuration") < 0) usleep(0);
    while(newline() < 0) usleep(0);
    while(printStr("==================") < 0) usleep(0);
    while(newline() < 0) usleep(0);
    
    while(printStr("   MAC Address....... : ") < 0) usleep(0);
    printMACAdress((u8_t*) s->mac_addr);

    while(printStr("   IP Address........ : ") < 0) usleep(0);
    printIPAdress((u8_t*) s->ipaddr);

    while(printStr("   Subnet Mask....... : ") < 0) usleep(0);
    printIPAdress((u8_t*) s->netmask);
    
    while(printStr("   Default Gateway... : ") < 0) usleep(0);
    printIPAdress((u8_t*) s->default_router);

    while(printStr("   Default DNS....... : ") < 0) usleep(0);
    printIPAdress((u8_t*) s->dnsaddr);
    
    while(newline() < 0) usleep(0);
}
#endif
