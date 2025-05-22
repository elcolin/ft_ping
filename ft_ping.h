#ifndef FT_PING_H
#define FT_PING_H
#include <arpa/inet.h>
// socket
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
// ip header
#include <netinet/ip.h>
// icmp header 
#include <netinet/ip_icmp.h>

#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

// getaddrinfo
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include <netdb.h>

#include <string.h>
// getpid
#include <unistd.h>
// CTRL-C handling
#include <signal.h>
// errno
#include <errno.h>

// typedef struct s_ping
// {
//    struct sockaddr_in   destAddress;
//    int                  sockfd;
//    struct icmphdr       icmpHeader;
//    u_int16_t            sequenceNumber;
//    char                 buffer[1024];
// } t_ping;

typedef struct s_rtt
{
   struct timeval start;
   struct timeval end;
   double         rtt_min;
   double         rtt_max;
   double         rtt_sum;
   double         rtt_avg;
   double         rtt_mdev;
} t_rtt;

typedef enum
{
   FALSE,
   TRUE
}  bool;

typedef enum
{
   SUCCESS,
   FAILURE
}  status;
/*
                    -- Internet Protocol Header --

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |Version|  IHL  |Type of Service|          Total Length         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |         Identification        |Flags|      Fragment Offset    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |  Time to Live |    Protocol   |         Header Checksum       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                       Source Address                          |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Destination Address                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Options                    |    Padding    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

https://datatracker.ietf.org/doc/html/rfc791
*/
/*
                        -- ICMP Header --
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Type      |     Code      |          Checksum             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |           Identifier          |        Sequence Number        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Data ...
   +-+-+-+-+-
                    Echo or Echo Reply Message

   https://datatracker.ietf.org/doc/html/rfc792

*/

// Values of the different fields in the ICMP header for echo request
#define ICMP_CODE 0

void     triggerError(int condition, char *msg, int sockfd);
void     defineICMPHeader(struct icmphdr *icmpHeader, u_int16_t sequenceNumber);

uint16_t computeChecksum(uint8_t *addr, int count);
int      initSocketFd();


#endif