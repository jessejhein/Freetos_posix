#ifndef _TERMIOS_H
# error "Never use <bits/termios.h> directly; include <termios.h> instead."
#endif

typedef __u32 speed_t;

/** speed of serial port */
struct termios
{
  speed_t c_ispeed;                     /* input speed */
  speed_t c_ospeed;                     /* output speed */
#define _HAVE_STRUCT_TERMIOS_C_ISPEED 1
#define _HAVE_STRUCT_TERMIOS_C_OSPEED 1
};

/* c_cflag bit meaning */
#define B0                      0000000 /* hang up */
#define B600                    0000010
#define B1200                   0000011
#define B2400                   0000013
#define B4800                   0000014
#define B9600                   0000015
#define B19200                  0000016
#define B38400                  0000017
#define CSIZE                   0000060
#define   CS5                   0000000
#define   CS6                   0000020
#define   CS7                   0000040
#define   CS8                   0000060
#define CSTOPB                  0000100
#define CREAD                   0000200
#define PARENB                  0000400
#define PARODD                  0001000
#define HUPCL                   0002000
#define CLOCAL                  0004000
#define B57600                  0010001
#define B115200                 0010002
#define B230400                 0010003
#define B460800                 0010004
#define B576000                 0010006
#define B921600                 0010007


/* c_iflag bits */
#define IGNBRK                  0000001
#define BRKINT                  0000002
#define IGNPAR                  0000004
#define PARMRK                  0000010
#define INPCK                   0000020
#define ISTRIP                  0000040
#define INLCR                   0000100
#define IGNCR                   0000200
#define ICRNL                   0000400
#define IUCLC                   0001000
#define IXON                    0002000
#define IXANY                   0004000
#define IXOFF                   0010000
#define IMAXBEL                 0020000
#define IUTF8                   0040000


/* c_oflag bits */
#define OPOST                   0000001
#define OLCUC                   0000002
#define ONLCR                   0000004
#define OCRNL                   0000010
#define ONOCR                   0000020
#define ONLRET                  0000040
#define OFILL                   0000100
#define OFDEL                   0000200


/* c_lflag bits */
#define ISIG                    0000001
#define ICANON                  0000002
#define ECHO                    0000010
#define ECHOE                   0000020
#define ECHOK                   0000040
#define ECHONL                  0000100
#define NOFLSH                  0000200
#define TOSTOP                  0000400
#define IEXTEN                  0100000


/* tcsetattr uses these */
#define TCSANOW                 0
#define TCSADRAIN               1
#define TCSAFLUSH               2
