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

void printBeginning(t_args *args, int sockfd);
void printReplyInfo(struct iphdr *ip_header, struct icmphdr *icmp_header, char *ip_address, long rtt_microseconds);
void printReplyInfoVerbose(struct iphdr *ip_header, struct icmphdr *icmp_header, char *ip_address, long rtt_microseconds);
void printStatistics(t_rtt *rtt, char *domain);

#endif