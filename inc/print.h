#ifndef PRINT_H
#define PRINT_H
#include <stdio.h>
#include <arpa/inet.h>
// ip header
#include <netinet/ip.h>
// icmp header 
#include <netinet/ip_icmp.h>
#include "utils.h"
#include "rtt.h"
#include "args.h"
#include "packet.h"

void    printBeginning(t_args *args, int sockfd, struct sockaddr_in *destAddr);
void    printReplyInfo(const t_packet *packet, const long rtt_microseconds, const t_args *args, char *ipAddr);
void    printStatistics(t_rtt *rtt, char *domain);
int     printPacketError(t_packet *reply, const int sequenceNumber);

#endif