#include "ft_ping.h"

volatile sig_atomic_t g_exit = 0;

void closeRessources(t_ping *ping)
{
    if (ping->sockfd > 0)
        close(ping->sockfd);   
}

void triggerError(int condition, char *msg)
{
    if (condition)
    {
        perror(msg);
        g_exit = 1;
    }
}

void handlesigint(int sig)
{
    g_exit = 1;
    (void)sig;
    //printf("\n--- ft_ping statistics ---\n");
    //printf("%d packets transmitted, %d packets received, %.1f%% packet loss\n",
    //    g_sent, g_received, g_sent == 0 ? 0.0 : ((g_sent - g_received) * 100.0 / g_sent));
    //exit(0);
}

int main(int argc, char **argv)
{
    t_ping ping;
    size_t pkg_sent = 0;
    size_t pkg_received = 0;
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <IP address>\n", argv[0]);
        return 1;
    }
    if(signal(SIGINT, handlesigint) == SIG_ERR)
    {
        perror("signal failed");
        return 1;
    }
    
    printf("pinging %s\n", argv[1]);

    // Setting up the destination address
    memset(&ping.destAddress, 0, sizeof(ping.destAddress));
    ping.destAddress.sin_family = AF_INET;
    if(inet_pton(PF_INET, argv[1], &ping.destAddress.sin_addr) != 1)
    {
        perror("inet_pton failed");
        return 1;
    }

    // Setting up the raw socket
    ping.sockfd = initSocketFd();
    if (ping.sockfd < 0)
    {
        perror("socket failed");
        return 1;
    }

    // Setting up the ICMP header
    memset(&ping.icmpHeader, 0, sizeof(struct icmphdr));

    char buffer[1024] = {0};
    struct timeval start, end;
    long rtt_microseconds = 0;
    ping.sequenceNumber = 0;

    while(ping.sequenceNumber < UINT16_MAX && !g_exit)
    {
        defineICMPHeader(&ping);
        sleep(1);
        gettimeofday(&start, NULL);
        if (sendto(ping.sockfd, (void *)&ping.icmpHeader, sizeof(ping.icmpHeader), 0, (struct sockaddr *)&ping.destAddress, sizeof(ping.destAddress)) == -1)
        {
            perror("sendto failed");
            closeRessources(&ping);
            g_exit = 1;
            break;
        }
        pkg_sent++;
        if (recvfrom(ping.sockfd, buffer, sizeof(buffer), 0, NULL, NULL) == -1)
            continue;
        gettimeofday(&end, NULL);
        
        rtt_microseconds = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
        struct iphdr *ip_header = (struct iphdr *)buffer;
        struct icmphdr *icmp_header = (struct icmphdr *)(buffer + (ip_header->ihl * 4));
        if (icmp_header->un.echo.id != ping.icmpHeader.un.echo.id)
            continue;
        pkg_received++;

        printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n",
               ntohs(ip_header->tot_len) - (ip_header->ihl * 4), argv[1], 
               ntohs(icmp_header->un.echo.sequence), ip_header->ttl, rtt_microseconds / 1000.0);
    }
    printf("\n--- ft_ping statistics ---\n");
    printf("%ld packets transmitted, %ld packets received, %.1f%% packet loss\n",
        pkg_sent, pkg_received, pkg_sent == 0 ? 0.0 : ((pkg_sent - pkg_received) * 100.0 / pkg_sent));
    close(ping.sockfd);
    return 0;
}