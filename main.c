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

int sendRequest(int sockfd, struct sockaddr_in *destAddress, struct icmphdr *icmpHeader)
{
    return sendto(sockfd, (void *)icmpHeader, sizeof(struct icmphdr), 0, (struct sockaddr *)destAddress, sizeof(struct sockaddr_in));
}

int receiveResponse(char *buffer, int sockfd, u_int16_t buffer_size)
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    return recvfrom(sockfd, buffer, buffer_size, 0, (struct sockaddr *)&addr, &addr_len);
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
    bool                is_verbose;
    
    is_verbose = checkVerboseArguments(argc, argv);
    isrc = is_verbose == TRUE ? 2 : 1;
    memset(&rtt, 0, sizeof(rtt));
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
    printBeginning(argv[isrc], is_verbose, sockfd, destAddress);
    FD_ZERO(&readfds);
    while(sequenceNumber < UINT16_MAX && g_exit == FALSE)
    {
        memset(&timeout, 0, sizeof(timeout));
        memset(buffer, 0, sizeof(buffer));
        // Define the ICMP header
        defineICMPHeader(&icmph_request, ++sequenceNumber);
        // Timestamp the start time
        gettimeofday(&start, NULL);
        // Send the ICMP request
        triggerError(sendRequest(sockfd, &destAddress, &icmph_request) < 0, "sendto failed", sockfd);
        ++pkg_sent;
        // Set the timeout for the select function
        if (pkg_received == 0)
            timeout.tv_sec = 1;
        else
            timeout.tv_usec = JITTER + rtt.rtt_avg;
        while(g_exit == FALSE)
        {
            FD_SET(sockfd, &readfds);
            // Waiting for file descriptor to be ready or timeout
            triggerError(select(sockfd + 1, &readfds, NULL, NULL, &timeout) < 0, "select failed", sockfd);
            if (timeout.tv_sec == 0 && timeout.tv_usec == 0)
                break;
            FD_CLR(sockfd, &readfds);
            // Receives data from the socket, stores in buffer
            triggerError(receiveResponse(buffer, sockfd, sizeof(buffer)) < 0, "recvfrom failed", sockfd);
            gettimeofday(&end, NULL);
            // If no valid packet is received, continue to next iteration
            if (handlePackets(&icmph_reply, &icmph_request, buffer, &iph_reply) == FAILURE)
                continue;
            // Calculate the round-trip time
            rtt_microseconds = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
            // Print the reply information
            is_verbose == TRUE ? printReplyInfoVerbose(iph_reply, icmph_reply, argv[isrc], rtt_microseconds) :
                printReplyInfo(iph_reply, icmph_reply, argv[isrc], rtt_microseconds);
            ++pkg_received;
            // Update the RTT statistics
            rttUpdate(&rtt, rtt_microseconds, pkg_received);
            break;
        }
        sleep(1);
    }
    printStatistics(&rtt, pkg_sent, pkg_received, argv[isrc]);
    close(sockfd);
    return 0;
}