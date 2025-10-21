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

void printBeginning(t_args *args, int sockfd, struct sockaddr_in *destAddr)
{
    if (args->activatedOptions[VERBOSE] == TRUE)
    {
        printf("ping: sock4.fd: %d (socktype: SOCK_RAW), hints.ai_family: AF_INET\n\n", sockfd);
        printf("ai->ai_family: AF_INET, ai->ai_canonname: '%s'\n", args->domain);
        printf("PING %s (%s) %d(%ld) bytes of data\n", args->domain, inet_ntoa(destAddr->sin_addr), args->optionsValue[PACKET_SIZE], sizeof(struct iphdr) + sizeof(struct icmphdr) + args->optionsValue[PACKET_SIZE]);
        return;
    }
    printf("PING %s (%s) %ld bytes of data\n", args->domain, inet_ntoa(destAddr->sin_addr), sizeof(struct icmphdr));
}