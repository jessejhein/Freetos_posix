/*
 * sys/socket.h
 */

/**************************************************************************************
 * Architecture
 **************************************************************************************/

#ifndef _SYS_SOCKET_H
#define _SYS_SOCKET_H   1

/**************************************************************************************
 * Interfaces:
 * None
 **************************************************************************************/

/**************************************************************************************
 * Compile Time Configurations:
 *  $ <.config_freertos_posix>                                                        */
#if 0
    CONFIG_ETHERNET             =   y 
#endif
/**************************************************************************************/

/**************************************************************************************
 * API Usage Examples                                                                 */
#if 0
//Example 1: WEB SERVER
    #include <string.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    
    #define MAX_CONNECTION      2       //ignored for uip
    
    int fd_app = -1;
    /*
     * foo_init(): create socket and initialized connection
     */
    void foo_init(void){
        fd_app = socket(AF_INET, SOCK_STREAM, PF_UNSPEC);
        //acquired socket descriptor
        if(fd_app != -1){
            struct sockaddr_in localSocket;
            localSocket.sin_family = AF_INET;
            localSocket.sin_port = htons(80);
            //localSocket.sin_addr =
            //inet_aton("10.12.110.57", &(my_addr.sin_addr)); 
            memset(&(localSocket.sin_zero), 0, 8); 
    #ifdef FREERTOS_SCHED 
            localSocket.appcall = foo_appcall;
    #endif            
            //bind local socket
            if(bind(fd_app, (struct sockaddr*) &localSocket, sizeof(sockaddr)) != -1){
                 //listen for the port
                if(listen(fd_app, MAX_CONNECTION) < 0){
    #ifdef FREERTOS_SCHED
                    int fd_app2 = -1; 
                    struct sockaddr_in remoteSocket;
                    fd_app2 = accept(fd_app, (struct sockaddr*) &remoteSocket, sizeof(sockaddr))
                    if(fd_app2 != -1){
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
    void foo_appcall(void){
        //handle the call
    }

//Example 2: WEB CLIENT
    #include <string.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    
    int fd_app = -1;
    /*
     * foo_init(): create socket and initialized connection
     */
    void foo_init(void){
        fd_app = socket(AF_INET, SOCK_STREAM, PF_UNSPEC);
        //acquired socket descriptor
        if(fd_app != -1){
            struct sockaddr_in localSocket;
            localSocket.sin_family = AF_INET;
            //localSocket.sin_addr =
            //inet_aton("10.12.110.57", &(my_addr.sin_addr)); 
            memset(&(localSocket.sin_zero), 0, 8); 
    #ifdef FREERTOS_SCHED 
            localSocket.appcall = foo_appcall;
    #endif            
            //bind local socket
            if(bind(fd_app, (struct sockaddr*) &localSocket, sizeof(sockaddr)) != -1){
                struct sockaddr_in remoteSocket;
                remoteSocket.sin_family = AF_INET;
                //remoteSocket.sin_addr =
                //inet_aton("10.12.110.57", &(remoteSocket.sin_addr)); 
                memset(&(remoteSocket.sin_zero), 0, 8); 
                 //connect to remote port
                if(connected(fd_app, MAX_CONNECTION) != -1){
    #ifdef FREERTOS_SCHED
                    foo_appcall();
    #endif
                }
            }
        }
    }
    
    /*
     * foo_appcall(): handle the communications
     */
    void foo_appcall(void){
        //handle the call
    }
    
#endif
/**************************************************************************************/

#include <bits/socket.h>

/**************************************************************************************
 * Function:        int socket(int domain, int type, int protocol)
 * 
 * Description:     create an endpoint for communication
 * 
 * Input:           domain:     address family (supports only AF_INET)
 *                  type:       socket type (supports only SOCK_STREAM (TCP) or SOCK_DGRAM (UDP))
 *                  protocol:   protocol family (supports only PF_INET (IP) or PF_UNSPEC for auto-detection) 
 * 
 * Output:          the socket descriptor
 *                  -1 on error.
 **************************************************************************************/
extern int socket(int domain, int type, int protocol);


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
extern int bind(int sockfd, struct sockaddr *my_addr, int addrlen);


/**************************************************************************************
 * Function:        int connect(int sockfd, struct sockaddr *serv_addr, int addrlen)
 * 
 * Description:     initiate a connection on a socket.
 * 
 * Input:           sockfd:     socket descriptor
 *                  serv_addr:  information about the destination port and IP address
 *                  addrlen:    set to sizeof(struct sockaddr).
 * 
 * Output:          0 on success
 *                  -1 on error
 **************************************************************************************/
extern int connect(int sockfd, struct sockaddr *serv_addr, int addrlen);


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
extern int listen(int sockfd, int backlog);
       

/**************************************************************************************
 * Function:        int accept(int sockfd, struct sockaddr *addr, int *addrlen)
 * 
 * Description:     accept a connection on a socket
 * 
 * Input:           sockfd:     socket descriptor
 *                  addr:       ptr to remote socket address
 *                  addrlen:    set to sizeof(struct sockaddr).
 * 
 * Output:          socket descriptor for the connection
 *                  -1 on error
 * 
 * Note:            call listen() before calling accept()
 **************************************************************************************/
extern int accept(int sockfd, struct sockaddr *addr, int *addrlen);


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
extern int send(int sockfd, void *msg, int len, int flags);

 
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
extern int recv(int sockfd, void *buf, int len, unsigned int flags);


/**************************************************************************************
 * Function:        int sendto(int sockfd, void *msg, int len, unsigned int flags, struct sockaddr *to, int tolen);
 * 
 * Description:     accept a connection on a socket
 * 
 * Input:           sockfd:     socket descriptor
 *                  msg:        data to be sent
 *                  len:        length of data to be sent (in bytes)
 *                  flags:      normally 0
 *                  to:         remote IP address and port
 *                  tolen:      sizeof(struct sockaddr
 * 
 * Output:          number of bytes acutally sent
 *                  -1 on error
 **************************************************************************************/
extern int sendto(int sockfd, void *msg, int len, unsigned int flags, struct sockaddr *to, int tolen);


/**************************************************************************************
 * Function:        int recvfrom(int sockfd, void *buf, int len, unsigned int flags, struct sockaddr *from, int *fromlen)
 * 
 * Description:     accept a connection on a socket
 * 
 * Input:           sockfd:     socket descriptor
 *                  buf:        buffer to read the information into
 *                  len:        maximum length of the buffer (in bytes)
 *                  flags:      normally 0
 *                  from:       local IP address and port
 *                  fromlen:    sizeof(struct sockaddr)
 * 
 * Output:          number of bytes acutally read
 *                  -1 on error
 **************************************************************************************/
extern int recvfrom(int sockfd, void *buf, int len, unsigned int flags, struct sockaddr *from, int *fromlen);


#endif /* sys/socket.h */
