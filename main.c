#include "ft_ping.h"

volatile sig_atomic_t g_exit = 0;

void closeRessources(t_ping *ping)
{
    if (ping->sockfd > 0)
        close(ping->sockfd);   
}

void triggerError(int condition, char *msg, int sockfd)
{
    if (condition)
    {
        perror(msg);
        close(sockfd);
        exit(EXIT_FAILURE);
    }
}
void handlesigint(int sig)
{
    g_exit = 1;
    (void)sig;
}

int resolveFQDN(char *fqdn, struct sockaddr_in *addr)
{
    struct addrinfo hints;
    struct addrinfo *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    if (getaddrinfo(fqdn, NULL, &hints, &result) != 0)
    {
       perror("getaddrinfo failed");
       return -1;
    }
    memcpy(addr, result->ai_addr, sizeof(struct sockaddr_in));
    freeaddrinfo(result);
    return 0;
}

int send_request(int sockfd, struct sockaddr_in *destAddress, struct icmphdr *icmpHeader)
{
    return sendto(sockfd, (void *)icmpHeader, sizeof(struct icmphdr), 0, (struct sockaddr *)destAddress, sizeof(struct sockaddr_in));
}

int recv_response(char *buffer, int sockfd, u_int16_t buffer_size)
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    return recvfrom(sockfd, buffer, buffer_size, 0, (struct sockaddr *)&addr, &addr_len);
}

int parsePacket(char *buffer, struct iphdr **ip_header, struct icmphdr **icmp_header)
{
    *ip_header = (struct iphdr *)buffer;   
    *icmp_header = (struct icmphdr *)(buffer + ((*ip_header)->ihl * 4));
    if (ntohs((*ip_header)->tot_len) < sizeof(struct iphdr))
        return -1;
    return ntohs((*ip_header)->tot_len);
}

int comparePackets(struct icmphdr *icmp_reply, struct icmphdr *icmp_request)
{
    if (icmp_reply == NULL || icmp_request == NULL)
    {
        //printf("icmp_reply or icmp_request is NULL\n");
        return -1;
    }
    if (icmp_reply->un.echo.id != icmp_request->un.echo.id)
    {
        //printf("icmp_reply->id: %d, icmp_request->id: %d\n", icmp_reply->un.echo.id, icmp_request->un.echo.id);
        return -1;
    }
    if (ntohs(icmp_reply->un.echo.sequence) != ntohs(icmp_request->un.echo.sequence))
    {
        //printf("icmp_reply->sequence: %d, icmp_request->sequence: %d\n", ntohs(icmp_reply->un.echo.sequence), ntohs(icmp_request->un.echo.sequence));
        return -1;
    }
    return 0;
}

void printReplyInfo(struct iphdr *ip_header, struct icmphdr *icmp_header, char *ip_address, long rtt_microseconds)
{
    printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n",
        ntohs(ip_header->tot_len) - (ip_header->ihl * 4), ip_address,
        ntohs(icmp_header->un.echo.sequence), ip_header->ttl, rtt_microseconds / 1000.0);
}

int main(int argc, char **argv)
{
    size_t              pkg_sent = 0;
    size_t              pkg_received = 0;
    fd_set              readfds;
    int                 sockfd;
    char                buffer[1024] = {0};
    long                rtt_microseconds = 0;
    u_int16_t           sequenceNumber = 0;
    struct icmphdr      icmph_request;
    struct iphdr        *iph_reply = NULL;
    struct icmphdr      *icmph_reply = NULL;
    struct timeval      start, end, timeout;
    struct sockaddr_in  destAddress;
    
    FD_ZERO(&readfds);
    timeout.tv_sec = 0;
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
    // Setting up the destination address
    memset(&destAddress, 0, sizeof(destAddress));
    destAddress.sin_family = AF_INET;
    // Check if the argument is a valid IP address
    if(inet_pton(PF_INET, argv[1], &destAddress.sin_addr) != 1 && resolveFQDN(argv[1], &destAddress) < 0)
    {
        fprintf(stderr, "Failed to resolve %s\n", argv[1]);
        return 1;
    }
    // Setting up the raw socket
    sockfd = initSocketFd();    
    while(sequenceNumber < UINT16_MAX && !g_exit)
    {
        FD_SET(sockfd, &readfds);
        // Define the ICMP header
        defineICMPHeader(&icmph_request, ++sequenceNumber);
        memset(buffer, 0, sizeof(buffer));
        // Timestamp the start time
        gettimeofday(&start, NULL);
        // Send the ICMP request
        triggerError(send_request(sockfd, &destAddress, &icmph_request) < 0, "sendto failed", sockfd);
        pkg_sent++;
         // 30ms timeout
        timeout.tv_usec = 30000;
        triggerError(select(sockfd + 1, &readfds, NULL, NULL, &timeout) < 0, "select failed", sockfd);
        // Timeout occurred, no response received
        if (timeout.tv_sec == 0 && timeout.tv_usec == 0)
        {
            sleep(1);
            continue;
        }
        FD_CLR(sockfd, &readfds);
        while(!g_exit)
        {
            // Receive the ICMP reply
            triggerError(recv_response(buffer, sockfd, sizeof(buffer)) < 0, "recvfrom failed", sockfd);
            // Timestamp the end time
            gettimeofday(&end, NULL);
            int pkg_idx = 0;
            while (comparePackets(icmph_reply, &icmph_request) != 0 && !g_exit && pkg_idx >= 0)
                // Loop until we find a valid packet
                pkg_idx = parsePacket(&(buffer[pkg_idx]), &iph_reply, &icmph_reply);
            // If pkg_idx < 0, it means we didn't find a valid packet
            if (pkg_idx < 0)
                continue;
            // Calculate the round-trip time
            rtt_microseconds = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
            // Print the reply information
            printReplyInfo(iph_reply, icmph_reply, argv[1], rtt_microseconds);
            pkg_received++;
            break;
        }
        sleep(1);
    }
    printf("\n--- ft_ping statistics ---\n");
    printf("%ld packets transmitted, %ld packets received, %.1f%% packet loss\n",
        pkg_sent, pkg_received, pkg_sent == 0 ? 0.0 : ((pkg_sent - pkg_received) * 100.0 / pkg_sent));
    close(sockfd);
    return 0;
}