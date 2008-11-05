#if !defined _SYS_SOCKET_H && !defined _NETINET_IN_H
# error "Never include <bits/socket.h> directly; use <sys/socket.h> instead."
#endif


/* Types of sockets.  */
#define SOCK_STREAM         1
#define SOCK_DGRAM          2
#define SOCK_RAW            3
#define SOCK_RDM            4
#define SOCK_SEQPACKET      5
#define SOCK_PACKET         10

/* Protocol families.  */
#define PF_UNSPEC           0   /* Unspecified.  */
#define PF_INET             2   /* IP protocol family.  */

/* Address families.  */
#define AF_INET             PF_INET

/** socket address */
struct sockaddr
{
  //unsigned char sa_len;
  /** address family (e.g. AF_INET) */         
  unsigned short sa_family;
  /** 14 bytes that are reserved to hold the address itself */
  char sa_data[14];
};

