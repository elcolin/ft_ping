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
    int                 sockfd;
    u_int16_t           sequenceNumber = 0;
    fd_set              readfds;
    long                rtt_microseconds = 0;

    t_packet            packets[PACKET_NUMBER] = {0};
    struct sockaddr_in  addrs[PACKET_NUMBER] = {0};

    struct timeval      start, end, timeout;
    t_rtt               rtt = {};
    t_args              args = {};
    void                (*ptrPrintReply)(struct iphdr *, struct icmphdr *, char *, long);

    if(signal(SIGINT, handlesigint) == SIG_ERR)
        exit(EXIT_FAILURE);

    // Verbose mode check
    checkArguments(argc, argv, &args);
    ptrPrintReply = args.activatedOptions[VERBOSE] == TRUE ? printReplyInfoVerbose : printReplyInfo;

    // Check if the domain is valid
    setDestinationAddress(&addrs[REPLY], args.domain);
    setSourceAddress(&addrs[REQUEST], &addrs[REPLY]);

    sockfd = initSocketFd();
    int opt = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt)) < 0) {
        perror("setsockopt(IP_HDRINCL)");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // TO DO Remove
    args.destAddress = addrs[REPLY];
    printBeginning(&args, sockfd);
    FD_ZERO(&readfds);
    
    // Loop until the maximum sequence number is reached or CTRL-C is pressed
    while(sequenceNumber < UINT16_MAX && g_exit == FALSE)
    {
        memset(&timeout, 0, sizeof(timeout));
        memset(&packets[REPLY], 0, sizeof(packets[REPLY]));
        // Define the ICMP header
        initPacket(&packets[REQUEST]);
        defineRequestPacket(&packets[REQUEST], addrs[REQUEST].sin_addr.s_addr, addrs[REPLY].sin_addr.s_addr, ++sequenceNumber);
        // Timestamp the start time
        gettimeofday(&start, NULL);
        // Send the ICMP request
        triggerError(sendRequest(sockfd, &addrs[REPLY], &packets[REQUEST]) < 0, "sendto failed", sockfd);
        ++rtt.pkg_sent;
        // Set the timeout for the select function
        timeout.tv_sec = (rtt.pkg_received == 0) ? 1 : (timeout.tv_usec = JITTER + rtt.mean, 0);
        while(g_exit == FALSE)
        {
            // If timeout is reached, breaks the loop and sends the next request
            if (socketIsReady(sockfd, &readfds, &timeout) == FAILURE)
                break;
            // Receives data from the socket, stores in buffer
            triggerError(receiveResponse((void *)packets[REPLY].buffer, sockfd, sizeof(packets[REPLY].buffer)) < 0, "recvfrom failed", sockfd);
            // Timestamp the end time
            gettimeofday(&end, NULL);
            // If no valid packet is received, continue to next iteration
            if (getValidPacket(&packets[REPLY], &packets[REQUEST]) == FAILURE)
                continue;
            // Calculate the round-trip time
            rtt_microseconds = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
            // Print the reply information
            ptrPrintReply(packets[REPLY].ip_hdr, packets[REPLY].icmp_hdr, args.domain, rtt_microseconds);
            ++rtt.pkg_received;
            // Update the RTT statistics
            rttUpdate(&rtt, rtt_microseconds);
            break;
        }
        sleep(1);
    }
    finalizeMeanDeviation(&rtt);
    printStatistics(&rtt, args.domain);
    close(sockfd);
    return 0;
}