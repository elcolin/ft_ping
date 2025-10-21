#include "packet.h"

// TODO Remove
#include <stdio.h>

void initPacket(t_packet *packet)
{
    memset(packet->buffer, 0, sizeof(packet->buffer));
    packet->ip_hdr = (void *) packet->buffer;
    packet->icmp_hdr = IPHDR_SHIFT(packet->buffer);
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

void defineRequestIPHeader(struct iphdr *ipHeader, u_int16_t sequenceNumber)
{
    ipHeader->version = IPVERSION;
    ipHeader->ihl = IPHDR_LEN;
    ipHeader->id = sequenceNumber; // Number of fragments;
    ipHeader->frag_off = 0x2;
    ipHeader->ttl = 64;
    ipHeader->protocol = IPPROTO_ICMP;
} 

void defineRequestICMPHeader(struct icmphdr *icmpHeader, u_int16_t sequenceNumber)
{
    // Setting up the ICMP header
    icmpHeader->type = ICMP_ECHO;
    icmpHeader->code = ICMP_CODE;
    icmpHeader->un.echo.id = htons(getpid() & 0xFFFF);
    icmpHeader->un.echo.sequence = htons(sequenceNumber);
    icmpHeader->checksum = computeChecksum((uint8_t *)icmpHeader, sizeof(*icmpHeader) + 16);
}

void defineRequestPacket(t_packet *packet, t_args *args)
{
    
}

status comparePackets(struct icmphdr *icmp_reply, struct icmphdr *icmp_request)
{
    if (icmp_reply == NULL || icmp_request == NULL)
        return FAILURE;
    if (icmp_reply->un.echo.id != icmp_request->un.echo.id)
    // Not the same process
        return FAILURE;
    if (ntohs(icmp_reply->un.echo.sequence) != ntohs(icmp_request->un.echo.sequence))
    // Not the same sequence id as request
        return FAILURE;
    return SUCCESS;
}

int parsePacket(void *buffer, struct iphdr **ip_header, struct icmphdr **icmp_header)
{
    *ip_header = (struct iphdr *)buffer;   
    *icmp_header = (struct icmphdr *)(buffer + ((*ip_header)->ihl * 4));
    if (ntohs((*ip_header)->tot_len) < sizeof(struct iphdr))
        return -1;
    return ntohs((*ip_header)->tot_len);
}

status getValidPacket(t_packet *reply, t_packet *request)
{
    int pkg_idx = 0;
    while (comparePackets(reply->icmp_hdr, request->icmp_hdr) != SUCCESS && pkg_idx >= 0 && pkg_idx < BUFFER_SIZE)
    {
        // Loop until we find a valid packet
        pkg_idx = parsePacket(&(reply->buffer[pkg_idx]), &reply->ip_hdr, &reply->icmp_hdr);
    }
    // If pkg_idx < 0, it means we didn't find a valid packet
    if (pkg_idx < 0 || pkg_idx >= BUFFER_SIZE)
        return FAILURE;
    return SUCCESS;
}