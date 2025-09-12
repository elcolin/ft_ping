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

int main(int argc, char **argv)
{
    size_t              pkg_sent = 0, pkg_received = 0;
    int                 sockfd;
    u_int16_t           sequenceNumber = 0;
    fd_set              readfds;
    char                buffer[1024];
    char                *domain = NULL;
    long                rtt_microseconds = 0;
    struct sockaddr_in  destAddress;
    struct icmphdr      icmph_request;
    struct icmphdr      *icmph_reply = NULL;
    struct iphdr        *iph_reply = NULL;
    struct timeval      start, end, timeout;
    t_rtt               rtt;
    bool                is_verbose;
    void                (*ptrPrintReply)(struct iphdr *, struct icmphdr *, char *, long);
    
    memset(&rtt, 0, sizeof(rtt));
    if(signal(SIGINT, handlesigint) == SIG_ERR)
        exit(EXIT_FAILURE);

    // Verbose mode check
    is_verbose = checkVerboseArguments(argc, argv);
    ptrPrintReply = is_verbose == TRUE ? printReplyInfoVerbose : printReplyInfo;
    domain = is_verbose == TRUE ? argv[2] : argv[1];
    //
    // Check if the domain is valid
    setDestinationAddress(&destAddress, domain);
    sockfd = initSocketFd();

    printBeginning(domain, is_verbose, sockfd, &destAddress);
    FD_ZERO(&readfds);

    // Loop until the maximum sequence number is reached or CTRL-C is pressed
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
        timeout.tv_sec = (pkg_received == 0) ? 1 : (timeout.tv_usec = JITTER + rtt.rtt_avg, 0);
        while(g_exit == FALSE)
        {
            // If timeout is reached, breaks the loop and sends the next request
            if (socketIsReady(sockfd, &readfds, &timeout) == FAILURE)
                break;
            // Receives data from the socket, stores in buffer
            triggerError(receiveResponse(buffer, sockfd, sizeof(buffer)) < 0, "recvfrom failed", sockfd);
            // Timestamp the end time
            gettimeofday(&end, NULL);
            // If no valid packet is received, continue to next iteration
            if (getValidPacket(&icmph_reply, &icmph_request, buffer, &iph_reply) == FAILURE)
                continue;
            // Calculate the round-trip time
            rtt_microseconds = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
            // Print the reply information
            ptrPrintReply(iph_reply, icmph_reply, domain, rtt_microseconds);
            ++pkg_received;
            // Update the RTT statistics
            printf("mdev: %f", calculateMeanDeviation(rtt_microseconds));
            rttUpdate(&rtt, rtt_microseconds, pkg_received);
            break;
        }
        sleep(1);
    }
    printStatistics(&rtt, pkg_sent, pkg_received, domain);
    close(sockfd);
    return 0;
}