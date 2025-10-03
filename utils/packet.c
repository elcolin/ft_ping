#include "inc/packet.h"

// TODO Remove
#include <stdio.h>

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
    icmpHeader->checksum = computeChecksum((uint8_t *)icmpHeader, sizeof(*icmpHeader) + 16);
}

status comparePackets(struct icmphdr *icmp_reply, struct icmphdr *icmp_request)
{
    if (icmp_reply == NULL || icmp_request == NULL)
        return FAILURE;
    if (icmp_reply->un.echo.id != icmp_request->un.echo.id)
        return FAILURE;
    if (ntohs(icmp_reply->un.echo.sequence) != ntohs(icmp_request->un.echo.sequence))
        return FAILURE;
    return SUCCESS;
}

int parsePacket(char *buffer, struct iphdr **ip_header, struct icmphdr **icmp_header)
{
    *ip_header = (struct iphdr *)buffer;   
    *icmp_header = (struct icmphdr *)(buffer + ((*ip_header)->ihl * 4));
    if (ntohs((*ip_header)->tot_len) < sizeof(struct iphdr))
        return -1;
    return ntohs((*ip_header)->tot_len);
}

status getValidPacket(struct icmphdr **icmph_reply, struct icmphdr *icmph_request, char *buffer, struct iphdr **iph_reply)
{
    int pkg_idx = 0;
    while (comparePackets(*icmph_reply, icmph_request) != SUCCESS && pkg_idx >= 0 && pkg_idx < BUFFER_SIZE)
    {
        // Loop until we find a valid packet
        pkg_idx = parsePacket(&(buffer[pkg_idx]), iph_reply, icmph_reply);
    }
    // If pkg_idx < 0, it means we didn't find a valid packet
    if (pkg_idx < 0 || pkg_idx >= BUFFER_SIZE)
        return FAILURE;
    return SUCCESS;
}