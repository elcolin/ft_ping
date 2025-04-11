#include "ft_ping.h"

void triggerError(int condition, char *msg)
{
    if (condition)
    {
        perror(msg);
        exit(1);
    }
}

int initSocketFd()
{
    struct protoent *proto = getprotobyname("icmp");
    if (proto == NULL)
        return -1;

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    printf("socket created\n");
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

int defineICMPHeader(t_ping *ping)
{
    memset(&ping->icmpHeader.checksum, 0, sizeof(ping->icmpHeader.checksum));
    ping->icmpHeader.type = ICMP_ECHO;
    ping->icmpHeader.code = ICMP_CODE;
    ping->icmpHeader.un.echo.id = htons(getpid() & 0xFFFF);
    ping->icmpHeader.un.echo.sequence = ping->icmpHeader.un.echo.sequence + 1;
    ping->icmpHeader.checksum = computeChecksum((uint8_t *) &ping->icmpHeader, sizeof(struct icmphdr));
    return 0;
}