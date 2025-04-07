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
    triggerError(!proto, "getprotobyname() failed");

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    triggerError(sockfd < 0, "socket() failed");
    printf("socket created\n");
    return sockfd;
}

uint16_t computeChecksum(uint8_t *addr, int count)
{
/* Compute Internet Checksum for "count" bytes
* beginning at location "addr".
*/
    uint16_t checksum;
    register uint32_t sum = 0;
    while( count > 1 ) {
    /* This is the inner loop */
    sum += *(addr++);
    count -= 2;
    }
    /* Add left-over byte, if any */
    if( count > 0 )
    sum += * (uint8_t *) addr;
    /* Fold 32-bit sum to 16 bits */
    while (sum>>16)
    sum = (sum & 0xffff) + (sum >> 16);
    checksum = ~sum;
    return checksum;
}

int defineICMPHeader()
{
    struct icmp icmpHeader;
    icmpHeader.icmp_type = ICMP_ECHO;
    icmpHeader.icmp_code = ICMP_CODE;
    icmpHeader.icmp_id = getpid() & 0xFFFF;
    icmpHeader.icmp_seq = ICMP_SEQUENCE_NUMBER;
    icmpHeader.icmp_cksum = computeChecksum((uint8_t *) &icmpHeader, sizeof(icmpHeader));
    return 0;
}