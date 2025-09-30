#ifndef PRINT_H
#define PRINT_H
#include <stdio.h>
#include <arpa/inet.h>
// ip header
#include <netinet/ip.h>
// icmp header 
#include <netinet/ip_icmp.h>
#include "booleans.h"
#include "rtt.h"

void printBeginning(char *domain, bool is_verbose, int sockfd, struct sockaddr_in *destAddress);
void printReplyInfo(struct iphdr *ip_header, struct icmphdr *icmp_header, char *ip_address, long rtt_microseconds);
void printReplyInfoVerbose(struct iphdr *ip_header, struct icmphdr *icmp_header, char *ip_address, long rtt_microseconds);
void printStatistics(t_rtt *rtt, char *domain);

#endif