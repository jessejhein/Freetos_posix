/**
 * \addtogroup ethernet ETHERNET
 * @{
 * 
 * Implementation of Ethernet API
 */

/**
 * \defgroup socket Linux Socket
 * @{
 * 
 * Implementation of Socket API
 */

/**
 * \file
 * Socket
 * \author Dennis Tsang <dennis@amonics.com>
 */

/*
 * Example 1: SERVER
 */
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
    
#define MAX_CONNECTION      2       //ignored for uip
    
int fd_app = -1;
/*
 * foo_init(): create socket and initialized connection
 */
void 
foo_init(void)
{
  fd_app = socket(AF_INET, SOCK_STREAM, PF_UNSPEC);
  //acquired socket descriptor
  if(fd_app != -1)
    {
      struct sockaddr_in localSocket;
      localSocket.sin_family = AF_INET;
      localSocket.sin_port = htons(80);
      inet_aton("192.168.1.38", &(localSocket.sin_addr)); 
      memset(&(localSocket.sin_zero), 0, 8); 
#ifdef FREERTOS_SCHED 
      localSocket.appcall = foo_appcall;
#endif            
      //bind local socket
      if(bind(fd_app, (struct sockaddr*) &localSocket, sizeof(struct sockaddr)) != -1)
        {
          //listen for the port
          if(listen(fd_app, MAX_CONNECTION) < 0)
            {
#ifndef FREERTOS_SCHED
              int fd_app2 = -1; 
              struct sockaddr_in remoteSocket;
              fd_app2 = accept(fd_app, (struct sockaddr*) &remoteSocket, sizeof(struct sockaddr))
              if(fd_app2 != -1)
                {
                  foo_appcall();
                }
#endif
            }
        }
    }
}
    
/*
 * foo_appcall(): handle the communications
 */
void 
foo_appcall(void)
{
  //handle the call
}


/*
 * Example 2: CLIENT
 */
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
    
int fd_app = -1;
/*
 * foo_init(): create socket and initialized connection
 */
void 
foo_init(void)
{
  fd_app = socket(AF_INET, SOCK_STREAM, IPPROTO_UDP);
  //acquired socket descriptor
  if(fd_app != -1)
    {
      struct sockaddr_in localSocket;
      localSocket.sin_family = AF_INET;
      localSocket.sin_port = htons(CLIENT_PORT);
      inet_aton("192.168.1.52", &(localSocket.sin_addr));
      memset(&(localSocket.sin_zero), 0, 8); 
#ifdef FREERTOS_SCHED 
      localSocket.appcall = foo_appcall;
#endif            
      //bind local socket
      if(bind(fd_app, (struct sockaddr*) &localSocket, sizeof(struct sockaddr)) != -1)
        {
          struct sockaddr_in remoteSocket;
          remoteSocket.sin_family = AF_INET;
          remoteSocket.sin_port = htons(SERVER_PORT);
          inet_aton("192.168.1.5", &(remoteSocket.sin_addr)); 
          memset(&(remoteSocket.sin_zero), 0, 8); 
          //connect to remote port
          if(connect(fd_app, (struct sockaddr*) &remoteSocket, sizeof(struct sockaddr)) != -1)
            {
#ifndef FREERTOS_SCHED
              foo_appcall();
#endif
            }
        }
    }
}
    
/*
 * foo_appcall(): handle the communications
 */
void 
foo_appcall(void)
{
  //perform read/write
}

 
/** @} */
/** @} */