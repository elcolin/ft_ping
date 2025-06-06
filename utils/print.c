#include "inc/print.h"

void printReplyInfo(struct iphdr *ip_header, struct icmphdr *icmp_header, char *ip_address, long rtt_microseconds)
{
    printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n",
        ntohs(ip_header->tot_len) - (ip_header->ihl * 4), ip_address,
        ntohs(icmp_header->un.echo.sequence), ip_header->ttl, rtt_microseconds / 1000.0);
}

void printReplyInfoVerbose(struct iphdr *ip_header, struct icmphdr *icmp_header, char *ip_address, long rtt_microseconds)
{
    printf("%d bytes from %s: icmp_seq=%d ident=%d ttl=%d time=%.2f ms\n",
        ntohs(ip_header->tot_len) - (ip_header->ihl * 4), ip_address,
        ntohs(icmp_header->un.echo.sequence), ntohs(icmp_header->un.echo.id), ip_header->ttl, rtt_microseconds / 1000.0);
}

void printStatistics(t_rtt *rtt, size_t pkg_sent, size_t pkg_received, char *domain)
{
    printf("\n--- %s ft_ping statistics ---\n", domain);
    printf("%ld packets transmitted, %ld packets received, %.1f%% packet loss\n",
        pkg_sent, pkg_received, pkg_sent == 0 ? 0.0 : ((pkg_sent - pkg_received) * 100.0 / pkg_sent));
    if (pkg_received != 0)
        printf("rtt min/avg/max/mdev = %.2f/%.2f/%.2f/%.2f ms\n",
            rtt->rtt_min / 1000.0, rtt->rtt_avg / 1000.0, rtt->rtt_max / 1000.0, rtt->rtt_mdev / 1000.0);
}

void printBeginning(char *domain, bool is_verbose, int sockfd, struct sockaddr_in *destAddress)
{
    if (is_verbose == TRUE)
    {
        printf("ping: sock4.fd: %d (socktype: SOCK_RAW), hints.ai_family: AF_INET\n\n", sockfd);
        printf("ai->ai_family: AF_INET, ai->ai_canonname: '%s'\n", domain);
    }
    printf("PING %s (%s) %ld bytes of data\n", domain, inet_ntoa(destAddress->sin_addr), sizeof(struct icmphdr));
}