#include "ft_ping.h"


void closeRessources(t_ping *ping)
{
    if (ping->sockfd > 0)
        close(ping->sockfd);   
}

int main(int argc, char **argv)
{   
    t_ping ping;

    triggerError(argc != 2, "Usage: ft_ping <destAddress>\n");
    
    printf("pinging %s\n", argv[1]);

    // Setting up the destination address
    memset(&ping.destAddress, 0, sizeof(ping.destAddress));
    ping.destAddress.sin_family = AF_INET;
    triggerError(inet_pton(PF_INET, argv[1], &ping.destAddress.sin_addr) != 1, "inet_pton failed\n");

    // Setting up the raw socket
    ping.sockfd = initSocketFd();
    triggerError(ping.sockfd < 0, "socket() failed\n");

    // Setting up the ICMP header
    memset(&ping.icmpHeader, 0, sizeof(struct icmphdr));

    char buffer[1024] = {0};
    struct timeval start, end;
    long rtt_microseconds = 0;
    
    while(ping.icmpHeader.un.echo.sequence < UINT16_MAX)
    {
        triggerError(defineICMPHeader(&ping) != 0, "defineICMPHeader() failed\n");
        gettimeofday(&start, NULL);
        sendto(ping.sockfd, (void *)&ping.icmpHeader, sizeof(ping.icmpHeader), 0, (struct sockaddr *)&ping.destAddress, sizeof(ping.destAddress));
        recvfrom(ping.sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
        gettimeofday(&end, NULL);
        rtt_microseconds = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
        printf("time=%.3f ms\n", rtt_microseconds / 1000.0);
        sleep(1);
    }
    close(ping.sockfd);
    return 0;
}
// int defineIPHeader()
// {
//     struct ip ipHeader;
//     ipHeader.ip_v = IP_ICMP_VERSION;
//     ipHeader.ip_hl = IP_ICMP_HEADER_LENGTH;
//     ipHeader.ip_tos = IP_ICMP_TOS;
//     ipHeader.ip_len = IP_ICMP_TOTAL_LENGTH;
//     ipHeader.ip_id = IP_ICMP_ID;
//     ipHeader.ip_off = IP_ICMP_FLAGS;
//     ipHeader.ip_ttl = IP_ICMP_TTL;
//     ipHeader.ip_p = IP_ICMP_PROTOCOL;
//     ipHeader.ip_dst = destAddress.sin_addr;
//     ipHeader.ip_src = srcAddress.sin_addr;
//     ipHeader.ip_sum = computeChecksum( (uint8_t *) &ipHeader, IP_ICMP_HEADER_LENGTH * 4);
//     return 0;
// }
    // struct ifaddrs *ifap;
    // triggerError(getifaddrs(&ifap) == -1, "getifaddrs() failed\n");

    // struct ifaddrs *idx = ifap;
    // while (idx)
    // {
    //     //printf("interface name: %s\n", idx->ifa_name);
    //     if (idx->ifa_addr->sa_family == AF_INET && strcmp(idx->ifa_name, "en0") == 0)
    //     {
    //         srcAddress = *(struct sockaddr_in *)idx->ifa_addr;
    //         //printf("ip address: %s\n", inet_ntoa(addr->sin_addr));
    //         break;
    //     }
    //     idx = idx->ifa_next;
    // }
    // freeifaddrs(ifap);
    // defineIPHeader();