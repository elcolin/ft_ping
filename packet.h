#ifndef PACKET_H
#define PACKET_H
// ip header
#include <netinet/ip.h>
// icmp header 
#include <netinet/ip_icmp.h>
#include <stddef.h>
//memset
#include <string.h>
//getpid
#include <unistd.h>

#include "booleans.h"

#define ICMP_CODE 0

uint16_t computeChecksum(uint8_t *addr, int count);
void     defineICMPHeader(struct icmphdr *icmpHeader, u_int16_t sequenceNumber);
status comparePackets(struct icmphdr *icmp_reply, struct icmphdr *icmp_request);
int parsePacket(char *buffer, struct iphdr **ip_header, struct icmphdr **icmp_header);
status getValidPacket(struct icmphdr **icmph_reply, struct icmphdr *icmph_request, char *buffer, struct iphdr **iph_reply);

#endif