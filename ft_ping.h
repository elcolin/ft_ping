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

// getifaddrs
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>

#include <string.h>
// getpid
#include <unistd.h>

typedef uint8_t byte;

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
// Values of the different fields in the IP header for ICMP packets
// Version
#define IP_ICMP_VERSION 4
// Type of Service
#define IP_ICMP_TOS 0
// Protocol
#define IP_ICMP_PROTOCOL 1
// Header length
#define IP_ICMP_HEADER_LENGTH 5
// Total length
#define IP_ICMP_TOTAL_LENGTH 28
// Identification
#define IP_ICMP_ID 0
// Flags
#define IP_ICMP_FLAGS 0
// Time to live
#define IP_ICMP_TTL 64

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
#define ICMP_SEQUENCE_NUMBER 1

void    triggerError(int condition, char *msg);
int defineICMPHeader();
uint16_t computeChecksum(uint8_t *addr, int count);
int initSocketFd();


#endif