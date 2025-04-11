#include "ft_ping.h"

int initSocketFd()
{
    struct protoent *proto = getprotobyname("icmp");
    if (proto == NULL)
        return -1;

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 30000; // 30ms timeout
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
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

void defineICMPHeader(t_ping *ping)
{
    // Setting up the ICMP header
    memset(&ping->icmpHeader.checksum, 0, sizeof(ping->icmpHeader.checksum));
    ping->icmpHeader.type = ICMP_ECHO;
    ping->icmpHeader.code = ICMP_CODE;
    ping->icmpHeader.un.echo.id = htons(getpid() & 0xFFFF);
    ping->icmpHeader.un.echo.sequence = htons(++ping->sequenceNumber);
    ping->icmpHeader.checksum = computeChecksum((uint8_t *) &ping->icmpHeader, sizeof(struct icmphdr));
}