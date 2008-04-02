/*
 * socket.c
 */

#include <sys/socket.h>
#include <netinet/in.h>

/*
 * Structure:       eth_info_t
 * 
 * Member:          appcall:    ethernet application rountine
 *                  domain:     address family
 *                  protocol:   protocol family
 *                  port:       local port (in network byte order)
 */
typedef struct{
    eth_appcall_t appcall;
    int domain;
    int type;
    int protocol;
    int port;
    union{
        struct uip_udp_conn* udp_conn;
    };
} eth_info_t;

static eth_info_t ethApp[ETH_MAX_APP];

/**************************************************************************************
 * Function:        int socket(int domain, int type, int protocol)
 * 
 * Description:     create an endpoint for communication
 * 
 * Input:           domain:     address family (supports only AF_INET)
 *                  type:       socket type (supports only SOCK_STREAM or SOCK_DGRAM)
 *                  protocol:   protocol family (supports only IPPROTO_IP/IPPROTO_TCP/IPPROTO_UDP) 
 * 
 * Output:          the socket descriptor
 *                  -1 on error.
 **************************************************************************************/
int socket(int domain, int type, int protocol)
{
    //check for socket availability
    int i;
    int fd_sock = -1;
    for (i=0; i<ETH_MAX_APP; i++){
        if (ethApp[i].appcall == NULL){
            fd_sock = i;
            break;
        } 
    }
    
    //socket available
    if(fd_sock != -1){
        if(domain == AF_INET){
            if(type == SOCK_STREAM || type == SOCK_DGRAM){
                ethApp[fd_sock].domain = domain;
                ethApp[fd_sock].type = type;
                //TCP
                if(protocol == IPPROTO_IP || protocol == IPPROTO_TCP){
                    ethApp[fd_sock].protocol = IPPROTO_TCP;
                    return 0;
                }
                //UPD
                else if(protocol == IPPROTO_UDP){
                    ethApp[fd_sock].protocol = IPPROTO_UDP;
                    return 0;
                }
                //unsupported protocol
                else return -1;
            }
            //unsupported type
            else return -1;
        }
        //unsupported domain
        else return -1;
    }
    //all sockets used
    return -1;
}

/**************************************************************************************
 * Function:        int shutdown(int sockfd, int how)
 * 
 * Description:     create an endpoint for communication
 * 
 * Input:           sockfd:     socket descriptor
 *                  how:        SHUT_RD     Further receives are disallowed
 *                              SHUT_WR     Further sends are disallowed
 *                              SHUT_RDWR   Further sends and receives are disallowed
 * 
 * Output:          0 on success
 *                  -1 on error.
 **************************************************************************************/
int shutdown(int sockfd, int how)
{
    if(sockfd>=0 && sockfd<ETH_MAX_APP) {
        if(ethApp[sockfd].protocol == IPPROTO_UDP){
            uip_udp_remove(ethApp[sockfd].udp_conn);
        }
        ethApp[sockfd].appcall = NULL;
        ethApp[sockfd].domain = 0;
        ethApp[sockfd].type = 0;
        ethApp[sockfd].protocol = 0;
        ethApp[sockfd].port = 0;
        return 0;
    }
    //invalid socket descriptor
    return -1;
}

/**************************************************************************************
 * Function:        int bind(int sockfd, struct sockaddr *my_addr, int addrlen)
 * 
 * Description:     attaches a local endpoint to a socket.
 * 
 * Input:           sockfd:     socket descriptor
 *                  my_addr:    information about local port and IP address
 *                  addrlen:    set to sizeof(struct sockaddr).
 * 
 * Output:          0 on success
 *                  -1 on error
 **************************************************************************************/
int bind(int sockfd, struct sockaddr *my_addr, int addrlen)
{
    if(sockfd>=0 && sockfd<ETH_MAX_APP) {
        if(my_addr == NULL) return -1;
        //save appcall and local port
        ethApp[sockfd].appcall = ((struct sockaddr_in *)my_addr)->appcall;
        ethApp[sockfd].port = ((struct sockaddr_in *)my_addr)->sin_port;
        return 0;
    }
    //invalid socket descriptor
    return -1;
}


/**************************************************************************************
 * Function:        int connect(int sockfd, struct sockaddr *serv_addr, int addrlen)
 * 
 * Description:     initiate a connection on a socket
 * 
 * Input:           sockfd:     socket descriptor
 *                  serv_addr:  information about the destination port and IP address
 *                  addrlen:    set to sizeof(struct sockaddr).
 * 
 * Output:          0 on success
 *                  -1 on error
 **************************************************************************************/
int connect(int sockfd, struct sockaddr *serv_addr, int addrlen)
{
    if(sockfd>=0 && sockfd<ETH_MAX_APP) {
        //UPD: create udp connection
        if(ethApp[sockfd].protocol == IPPROTO_UDP){
            //set udp remote address and remote port
            ethApp[sockfd].udp_conn = uip_udp_new(
                                        (uip_ipaddr_t*)(((struct sockaddr_in *)serv_addr)->sin_addr.s_addr), 
                                        ((struct sockaddr_in *)serv_addr)->sin_port
                                      );
            if(ethApp[sockfd].udp_conn != NULL) {
                //bind the local uip socket
                uip_udp_bind(ethApp[sockfd].udp_conn, ethApp[sockfd].port);
                return 0;
            }
            //all udp sockets are used
            else return -1;
        }
        else if(ethApp[sockfd].protocol == IPPROTO_TCP){
            //todo: tcp
            return 0;
        }
    }
    //invalid socket descriptor
    return -1;
}


/**************************************************************************************
 * Function:        int listen(int sockfd, int backlog)
 * 
 * Description:     listen for connections on a socket
 * 
 * Input:           sockfd:     socket descriptor
 *                  backlog:    number of connections allowed on the incoming queue (not used)
 * 
 * Output:          0 on success
 *                  -1 on error
 * 
 * Note:            call bind() before calling listen()
 **************************************************************************************/
int listen(int sockfd, int backlog)
{
    if(sockfd>=0 && sockfd<ETH_MAX_APP) {
        uip_listen( ethApp[sockfd].port );
        return 0;
    }
    //invalid socket descriptor
    return -1;
}


/**************************************************************************************
 * Function:        int send(int sockfd, void *msg, int len, int flags)
 * 
 * Description:     accept a connection on a socket
 * 
 * Input:           sockfd:     socket descriptor
 *                  msg:        data to be sent
 *                  len:        length of data to be sent (in bytes)
 *                  flags:      normally 0
 * 
 * Output:          number of bytes acutally sent
 *                  -1 on error
 **************************************************************************************/
int send(int sockfd, void *msg, int len, int flags){
    if(sockfd>=0 && sockfd<ETH_MAX_APP) {
        uip_send(msg, len);
        return len;
    }
    //invalid socket descriptor
    return -1;
}

/**************************************************************************************
 * Function:        int recv(int sockfd, void *buf, int len, unsigned int flags)
 * 
 * Description:     accept a connection on a socket
 * 
 * Input:           sockfd:     socket descriptor
 *                  buf:        buffer to read the information into
 *                  len:        maximum length of the buffer (in bytes)
 *                  flags:      normally 0
 * 
 * Output:          number of bytes acutally read
 *                  -1 on error
 **************************************************************************************/
static int rd_ptr = 0;
int recv(int sockfd, void *buf, int len, unsigned int flags)
{
    if(sockfd>=0 && sockfd<ETH_MAX_APP) {
        if(uip_newdata() > 0){
            //data available to read
            int data_left = uip_datalen() - rd_ptr;
            if( data_left > 0 ){
                int data_len = (data_left > len) ? len : data_left;
                rd_ptr+=data_len;
                while(data_left--){
                    *(char*)buf++ = *(char*)uip_appdata++; 
                }
                if(rd_ptr >= uip_datalen()) rd_ptr = 0;
                return data_len;
            }
        }
        rd_ptr = 0;
        return 0; 
    }
    //invalid socket descriptor
    return -1;
}

//--------------------------------------------------------------------------------------
// Non-standard api
//--------------------------------------------------------------------------------------
/**************************************************************************************
 * Function:        int shutdown(int sockfd, int how)
 * 
 * Description:     create an endpoint for communication
 * 
 * Input:           sockfd:     socket descriptor
 *                  how:        SHUT_RD     Further receives are disallowed
 *                              SHUT_WR     Further sends are disallowed
 *                              SHUT_RDWR   Further sends and receives are disallowed
 * 
 * Output:          0 on success
 *                  -1 on error.
 **************************************************************************************/
void socket_init(void){
    int i;
    for(i=0; i<ETH_MAX_APP; i++){
        ethApp[i].appcall = NULL;
        ethApp[i].domain = 0;
        ethApp[i].type = 0;
        ethApp[i].protocol = 0;
        ethApp[i].port = 0;
    }
}

/**************************************************************************************
 * Function:        void tcp_appcall(void)
 * 
 * Description:     rountine to handle tcp packets
 * 
 * Input:           None
 * 
 * Output:          None
 **************************************************************************************/
/*
void tcp_appcall(void){
    switch(uip_conn->lport) {

#ifdef WEBSERVER
        case HTONS(80):
        {
            httpd_appcall();
            break;
        }
#endif

#ifdef SMTPCLIENT
        case HTONS(25):
        {
            smtp_appcall();
            break;
        }
#endif
    }
}
*/

/**************************************************************************************
 * Function:        void udp_appcall(void)
 * 
 * Description:     rountine to handle udp packets
 * 
 * Input:           None
 * 
 * Output:          None
 **************************************************************************************/
void udp_appcall(void){
    int i;
    for (i=0; i<ETH_MAX_APP; i++){
        //application is UDP
        if(ethApp[i].type == SOCK_DGRAM){
            //check for local port number
            if(uip_udp_conn->lport == ethApp[i].port){
                ethApp[i].appcall();
                break;
            }
        }
    }
}
