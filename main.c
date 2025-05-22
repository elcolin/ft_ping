#include "ft_ping.h"

volatile sig_atomic_t g_exit = FALSE;

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
    g_exit = TRUE;
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

int receive_response(char *buffer, int sockfd, u_int16_t buffer_size)
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

status comparePackets(struct icmphdr *icmp_reply, struct icmphdr *icmp_request)
{
    if (icmp_reply == NULL || icmp_request == NULL)
    {
        //printf("icmp_reply or icmp_request is NULL\n");
        return FAILURE;
    }
    if (icmp_reply->un.echo.id != icmp_request->un.echo.id)
    {
        //printf("icmp_reply->id: %d, icmp_request->id: %d\n", icmp_reply->un.echo.id, icmp_request->un.echo.id);
        return FAILURE;
    }
    if (ntohs(icmp_reply->un.echo.sequence) != ntohs(icmp_request->un.echo.sequence))
    {
        //printf("icmp_reply->sequence: %d, icmp_request->sequence: %d\n", ntohs(icmp_reply->un.echo.sequence), ntohs(icmp_request->un.echo.sequence));
        return FAILURE;
    }
    return SUCCESS;
}



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
    printf("rtt min/avg/max = %.2f/%.2f/%.2f ms\n",
        rtt->rtt_min / 1000.0, rtt->rtt_avg / 1000.0, rtt->rtt_max / 1000.0);
}

void rttUpdate(t_rtt *rtt, long rtt_microseconds, size_t rtt_count)
{
    rtt->rtt_sum += rtt_microseconds;
    if (rtt->rtt_min == 0 || rtt_microseconds < rtt->rtt_min)
        rtt->rtt_min = rtt_microseconds;
    if (rtt->rtt_max == 0 || rtt_microseconds > rtt->rtt_max)
        rtt->rtt_max = rtt_microseconds;
    rtt->rtt_avg = rtt->rtt_sum / (double) (rtt_count);
}

bool checkVerboseArguments(int argc, char **argv)
{
    if(signal(SIGINT, handlesigint) == SIG_ERR)
    {
        perror("signal failed");
        EXIT_FAILURE;
    }
    if (argc == 3 && strcmp(argv[1], "-v") == 0)
        return TRUE;
    else if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <IP address>\n", argv[0]);
        EXIT_FAILURE;
    }
    return FALSE;
}

status handlePackets(struct icmphdr **icmph_reply, struct icmphdr *icmph_request, char *buffer, struct iphdr **iph_reply)
{
    int pkg_idx = 0;
    while (comparePackets(*icmph_reply, icmph_request) != SUCCESS && g_exit == FALSE && pkg_idx >= 0)
        // Loop until we find a valid packet
        pkg_idx = parsePacket(&(buffer[pkg_idx]), iph_reply, icmph_reply);
    // If pkg_idx < 0, it means we didn't find a valid packet
    if (pkg_idx < 0)
        return FAILURE;
    return SUCCESS;
}

int main(int argc, char **argv)
{
    size_t              pkg_sent = 0, pkg_received = 0;
    size_t              isrc = 1;
    int                 sockfd;
    fd_set              readfds;
    char                buffer[1024] = {0};
    long                rtt_microseconds = 0;
    u_int16_t           sequenceNumber = 0;
    struct icmphdr      icmph_request;
    struct iphdr        *iph_reply = NULL;
    struct icmphdr      *icmph_reply = NULL;
    struct timeval      start, end, timeout;
    struct sockaddr_in  destAddress;
    t_rtt               rtt;
    bool                is_verbose = FALSE;
    
    is_verbose = checkVerboseArguments(argc, argv);
    isrc = is_verbose == TRUE ? 2 : 1;
    memset(&rtt, 0, sizeof(rtt));
    memset(&timeout, 0, sizeof(timeout));
    memset(&destAddress, 0, sizeof(destAddress));
    destAddress.sin_family = AF_INET;
    // Check if the argument is a valid IP address
    if(inet_pton(PF_INET, argv[isrc], &destAddress.sin_addr) != 1 && resolveFQDN(argv[isrc], &destAddress) < 0)
    {
        fprintf(stderr, "Failed to resolve %s\n", argv[isrc]);
        return 1;
    }
    // Setting up the raw socket
    sockfd = initSocketFd();    
    if (is_verbose == TRUE)
    {
        printf("ping: sock4.fd: %d (socktype: SOCK_RAW), hints.ai_family: AF_INET\n\n", sockfd);
        printf("ai->ai_family: AF_INET, ai->ai_canonname: '%s'\n", argv[isrc]);
    }
    printf("PING %s (%s) %ld bytes of data\n", argv[isrc], inet_ntoa(destAddress.sin_addr), sizeof(struct icmphdr));
    FD_ZERO(&readfds);
    while(sequenceNumber < UINT16_MAX && g_exit == FALSE)
    {
        FD_SET(sockfd, &readfds);
        // Define the ICMP header
        defineICMPHeader(&icmph_request, ++sequenceNumber);
        memset(buffer, 0, sizeof(buffer));
        // Timestamp the start time
        gettimeofday(&start, NULL);
        // Send the ICMP request
        triggerError(send_request(sockfd, &destAddress, &icmph_request) < 0, "sendto failed", sockfd);
        ++pkg_sent;
         // 30ms timeout
        if (pkg_received == 0)
            timeout.tv_sec = 1;
        else
            timeout.tv_usec = 30000 + rtt.rtt_avg;
        while(g_exit == FALSE)
        {
            FD_SET(sockfd, &readfds);
            // Waiting for file descriptor to be ready or timeout
            triggerError(select(sockfd + 1, &readfds, NULL, NULL, &timeout) < 0, "select failed", sockfd);
            if (timeout.tv_sec == 0 && timeout.tv_usec == 0)
                break;
            FD_CLR(sockfd, &readfds);
            // Receive the ICMP reply
            triggerError(receive_response(buffer, sockfd, sizeof(buffer)) < 0, "recvfrom failed", sockfd);
            // Timestamp the end time
            gettimeofday(&end, NULL);
            if (handlePackets(&icmph_reply, &icmph_request, buffer, &iph_reply) == FAILURE)
                continue;
            // Calculate the round-trip time
            rtt_microseconds = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
            // Print the reply information
            is_verbose == TRUE ? printReplyInfoVerbose(iph_reply, icmph_reply, argv[isrc], rtt_microseconds) :
                printReplyInfo(iph_reply, icmph_reply, argv[isrc], rtt_microseconds);
            ++pkg_received;
            rttUpdate(&rtt, rtt_microseconds, pkg_received);
            break;
        }
        sleep(1);
    }
    printStatistics(&rtt, pkg_sent, pkg_received, argv[isrc]);
    close(sockfd);
    return 0;
}