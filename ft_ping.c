#include "ft_ping.h"

#define ICMP_CODE 0

int initSocketFd()
{
    struct protoent *proto = getprotobyname("icmp");
    if (proto == NULL)
    {
        perror("getprotobyname");
        exit(EXIT_FAILURE);
    }
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

uint16_t computeChecksum(uint8_t *addr, int count)
{
    uint32_t sum = 0;
    uint16_t *ptr = (uint16_t *) addr;

    while (count > 1) {
        sum += *ptr++;
        count -= 2;
    }
    if (count > 0) {
        sum += *((uint8_t *)ptr);
    }
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    return (uint16_t)(~sum);
}

void defineICMPHeader(struct icmphdr *icmpHeader, u_int16_t sequenceNumber)
{
    // Setting up the ICMP header
    memset(icmpHeader, 0, sizeof(struct icmphdr));
    icmpHeader->type = ICMP_ECHO;
    icmpHeader->code = ICMP_CODE;
    icmpHeader->un.echo.id = htons(getpid() & 0xFFFF);
    icmpHeader->un.echo.sequence = htons(sequenceNumber);
    icmpHeader->checksum = computeChecksum((uint8_t *)icmpHeader, sizeof(struct icmphdr));
}