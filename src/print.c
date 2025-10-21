#include "print.h"

void printReplyInfo(const t_packet *packet, const long rtt_microseconds, const t_args *args, char *ipAddr)
{
    if (args->activatedOptions[VERBOSE] == TRUE)
    {
        printf("%d bytes from %s: icmp_seq=%d ident=%d ttl=%d",
        ntohs(packet->ip_hdr->tot_len) - (packet->ip_hdr->ihl * 4), ipAddr,
        ntohs(packet->icmp_hdr->un.echo.sequence), ntohs(packet->icmp_hdr->un.echo.id), packet->ip_hdr->ttl);
    }
    else
    {
        printf("%d bytes from %s: icmp_seq=%d ttl=%d",
        ntohs(packet->ip_hdr->tot_len) - (packet->ip_hdr->ihl * 4), ipAddr,
        ntohs(packet->icmp_hdr->un.echo.sequence), packet->ip_hdr->ttl);
    }
    if (args->optionsValue[PACKET_SIZE] >= 16)
            printf(" time=%.*f ms", getDecimalsToPrint(rtt_microseconds), rtt_microseconds / 1000.0);
    printf("\n");
}
void  printStatistics(t_rtt *rtt, char *domain)
{
    printf("\n--- %s ft_ping statistics ---\n", domain);
    printf("%ld packets transmitted, %ld packets received, %.1f%% packet loss\n",
        rtt->pkg_sent, rtt->pkg_received, rtt->pkg_sent == 0 ? 0.0 : ((rtt->pkg_sent - rtt->pkg_received) * 100.0 / rtt->pkg_sent));
    if (rtt->pkg_received != 0)
        printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
            rtt->min / 1000.0, rtt->mean / 1000.0, rtt->max / 1000.0, rtt->mdev / 1000);
}

void printPacketError(t_packet *reply, const int error, const int sequenceNumber)
{
    if (reply == NULL)
        return;
    uint32_t saddr = reply->ip_hdr->saddr; // in network byte order
    struct in_addr addr;
    addr.s_addr = saddr;

    char *ip_str = inet_ntoa(addr); // returns a pointer to static buffer
    switch (error)
    {
        case ICMP_DEST_UNREACH:
            char *msg_unreach[] = {"Destination net unreachable", "Destination host unreachable", "Destination protocol unreachable", "Destination port unreachable", "Fragmentation needed and DF set", "Source route failed"};
            printf("From %s icmp_seq=%d %s\n", ip_str, sequenceNumber, msg_unreach[reply->icmp_hdr->code]);
        break;
        case ICMP_TIME_EXCEEDED:
            char *msg_ttl[] = {"Time to live exceeded", "Fragment Reass time exceeded"};
            printf("From %s icmp_seq=%d %s\n", ip_str, sequenceNumber, msg_ttl[reply->icmp_hdr->code]);
        break;
        default:
            return;
    }

}
/*      0 = net unreachable;

      1 = host unreachable;

      2 = protocol unreachable;

      3 = port unreachable;

      4 = fragmentation needed and DF set;

      5 = source route failed.
*/

void printBeginning(t_args *args, int sockfd, struct sockaddr_in *destAddr)
{
    if (args->activatedOptions[VERBOSE] == TRUE)
    {
        printf("ping: sock4.fd: %d (socktype: SOCK_RAW), hints.ai_family: AF_INET\n\n", sockfd);
        printf("ai->ai_family: AF_INET, ai->ai_canonname: '%s'\n", args->domain);
        return;
    }
    printf("PING %s (%s) %d(%ld) bytes of data\n", args->domain, inet_ntoa(destAddr->sin_addr), args->optionsValue[PACKET_SIZE], sizeof(struct iphdr) + sizeof(struct icmphdr) + args->optionsValue[PACKET_SIZE]);
}