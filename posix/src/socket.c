/**
 * \file
 * Socket
 * \author Dennis Tsang <dennis@amonics.com>
 */

#include <sys/socket.h>
#include <netinet/in.h>

/**
 * Holds information on ethernet application
 */
typedef struct
{
  /** ethernet application rountine */
  eth_appcall_t appcall;
  /** address family */
  int domain;
  int type;
  /** protocol family */
  int protocol;
  /** local port (in network byte order) */
  int lport;
  int rport;
  int isclient;
  union
    {
      struct uip_udp_conn* udp_conn;
      struct uip_conn *tcp_conn;
    };
} eth_info_t;


/** List of ethernet applications */
static eth_info_t ethApp[ETH_MAX_APP];


//------------------------------------------------------------------------------
int 
socket(int domain, int type, int protocol)
{
  //check for socket availability
  int i;
  int fd_sock = -1;
  for (i=0; i<ETH_MAX_APP; i++)
    {
      if (ethApp[i].appcall == NULL)
        {
          fd_sock = i;
          break;
        }
    }
    
  //socket available
  if(fd_sock != -1)
    {
      if(domain == AF_INET)
        {
          if(type == SOCK_STREAM || type == SOCK_DGRAM)
            {
              ethApp[fd_sock].domain = domain;
              ethApp[fd_sock].type = type;
              //TCP
              if(protocol == IPPROTO_IP || protocol == IPPROTO_TCP)
                {
                  ethApp[fd_sock].protocol = IPPROTO_TCP;
                  return fd_sock;
                }
              //UPD
              else if(protocol == IPPROTO_UDP)
                {
                  ethApp[fd_sock].protocol = IPPROTO_UDP;
                  return fd_sock;
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


//------------------------------------------------------------------------------
int 
shutdown(int sockfd, int how)
{
  if(sockfd>=0 && sockfd<ETH_MAX_APP) 
    {
      if(ethApp[sockfd].protocol == IPPROTO_UDP)
        {
          uip_udp_remove(ethApp[sockfd].udp_conn);
        }
      ethApp[sockfd].appcall = NULL;
      ethApp[sockfd].domain = 0;
      ethApp[sockfd].type = 0;
      ethApp[sockfd].protocol = 0;
      ethApp[sockfd].lport = 0;
      ethApp[sockfd].rport = 0;
      ethApp[sockfd].isclient = 0;
      return 0;
    }
  //invalid socket descriptor
  return -1;
}


//------------------------------------------------------------------------------
int 
bind(int sockfd, struct sockaddr *my_addr, int addrlen)
{
  if(sockfd>=0 && sockfd<ETH_MAX_APP) 
    {
      if(my_addr == NULL) return -1;
      //save appcall and local port
      ethApp[sockfd].appcall = ((struct sockaddr_in *)my_addr)->appcall;
      ethApp[sockfd].lport = ((struct sockaddr_in *)my_addr)->sin_port;
      return 0;
    }
  //invalid socket descriptor
  return -1;
}


//------------------------------------------------------------------------------
int 
connect(int sockfd, struct sockaddr *serv_addr, int addrlen)
{
  if(sockfd>=0 && sockfd<ETH_MAX_APP) 
    {
      //UPD: create udp connection
      if(ethApp[sockfd].protocol == IPPROTO_UDP)
        {
          //set udp remote address and remote port
          ethApp[sockfd].udp_conn = uip_udp_new(
                                        (uip_ipaddr_t*)(((struct sockaddr_in *)serv_addr)->sin_addr.s_addr), 
                                        ((struct sockaddr_in *)serv_addr)->sin_port
                                      );
          if(ethApp[sockfd].udp_conn != NULL) 
            {
              //bind the local uip socket
              uip_udp_bind(ethApp[sockfd].udp_conn, ethApp[sockfd].lport);
              return 0;
            }
          //all udp sockets are used
          else return -1;
        }
      else if(ethApp[sockfd].protocol == IPPROTO_TCP)
        {
          //set tcp remote address and remote port
          ethApp[sockfd].tcp_conn = uip_connect( 
                                        (uip_ipaddr_t*)(((struct sockaddr_in *)serv_addr)->sin_addr.s_addr), 
                                        ((struct sockaddr_in *)serv_addr)->sin_port
                                      );
          if(ethApp[sockfd].tcp_conn != NULL) 
            {
              ethApp[sockfd].rport = ((struct sockaddr_in *)serv_addr)->sin_port;
              ethApp[sockfd].isclient = 1;
            	return 0;
            }
          //all tcp sockets are used
          else return -1;
        }
    }
  //invalid socket descriptor
  return -1;
}


//------------------------------------------------------------------------------
int 
listen(int sockfd, int backlog)
{
  if(sockfd>=0 && sockfd<ETH_MAX_APP) 
    {
      uip_listen( ethApp[sockfd].lport );
      ethApp[sockfd].isclient = 0;
      return 0;
    }
  //invalid socket descriptor
  return -1;
}


//------------------------------------------------------------------------------
int 
send(int sockfd, void *msg, int len, int flags)
{
  if(sockfd>=0 && sockfd<ETH_MAX_APP) 
    {
      uip_send(msg, len);
      return len;
    }
  //invalid socket descriptor
  return -1;
}

//------------------------------------------------------------------------------
/* 
 * BUG: 
 * when len < data_left, and application fails to read the remaining bytes stream, the
 * rd_ptr cannot be reset to zero.
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
*/

//--------------------------------------------------------------------------------------
// Non-standard api
//--------------------------------------------------------------------------------------
/**
 * \brief initialize sockets
 */
void 
socket_init(void)
{
  int i;
  for(i=0; i<ETH_MAX_APP; i++)
    {
      ethApp[i].appcall = NULL;
      ethApp[i].domain = 0;
      ethApp[i].type = 0;
      ethApp[i].protocol = 0;
      ethApp[i].lport = 0;
      ethApp[i].rport = 0;
      ethApp[i].isclient = 0;
    }
}


/**
 * \brief rountine to handle tcp packets
 */
void 
tcp_appcall(void)
{
  int i;
  for (i=0; i<ETH_MAX_APP; i++)
    {
      //application is TCP
      if(ethApp[i].type == SOCK_STREAM)
        {
          //check for local port number, for server applications (i.e. using listen()) 
          if(ethApp[i].isclient == 0)
            {
              if(uip_conn->lport == ethApp[i].lport)
                {
                  ethApp[i].appcall();
                  break;
                }
            }
          //check for remote port number, for client applications (i.e. using connect())
          else
            {
              if(uip_conn->rport == ethApp[i].rport)
                {
                  ethApp[i].appcall();
                  break;
                }
            }
        }
    }
}


/**
 * \brief rountine to handle udp packets
 */
void 
udp_appcall(void)
{
  int i;
  for (i=0; i<ETH_MAX_APP; i++)
    {
      //application is UDP
      if(ethApp[i].type == SOCK_DGRAM)
        {
          //check for local port number
          if(uip_udp_conn->lport == ethApp[i].lport)
            {
              ethApp[i].appcall();
              break;
            }
        }
    }
}
