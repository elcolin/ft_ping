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
    // args
    t_args              args = {};
    // socket
    int                 sockfd;
    fd_set              readfds;
    // rtt
    struct timeval      start, end, timeout;
    long                rtt_microseconds = 0;
    t_rtt               rtt = {};
    //packet
    u_int16_t           sequenceNumber = 0;
    t_packet            packets[PACKET_NUMBER] = {0};
    struct sockaddr_in  addrs[PACKET_NUMBER] = {0};
    int                 error;

    if(signal(SIGINT, handlesigint) == SIG_ERR)
        exit(EXIT_FAILURE);

    // Option check
    checkArguments(argc, argv, &args);
    if (args.activatedOptions[PACKET_SIZE] == FALSE)
        args.optionsValue[PACKET_SIZE] = DEFAULT_PADDING;

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

    printBeginning(&args, sockfd, &addrs[REPLY]);
    FD_ZERO(&readfds);
    
    // Loop until the maximum sequence number is reached or CTRL-C is pressed
    while(sequenceNumber < UINT16_MAX && g_exit == FALSE)
    {
        error = 0;
        memset(&timeout, 0, sizeof(timeout));
        memset(&packets[REPLY], 0, sizeof(packets[REPLY]));
        initPacket(&packets[REQUEST]);
        handlePacketOptions(&packets[REQUEST], &args);
        // Define the header
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
            if (getValidPacket(&packets[REPLY], &packets[REQUEST], &error) == FAILURE)
            {
                printPacketError(&packets[REPLY], error, sequenceNumber);
                break;
            }
            // Calculate the round-trip time
            rtt_microseconds = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
            // Print the reply information
            printReplyInfo(&packets[REPLY], rtt_microseconds, &args, args.domain);
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