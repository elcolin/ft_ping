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
    t_args              args = {};
    int                 sockfd;
    fd_set              readfds;
    struct timeval      start, end, timeout;
    long                rtt_microseconds = 0;
    t_rtt               rtt = {};
    u_int16_t           sequenceNumber = 0;
    t_packet            packets[PACKET_NUMBER] = {0};
    struct sockaddr_in  addrs[PACKET_NUMBER] = {0};

    if(signal(SIGINT, handlesigint) == SIG_ERR)
        exit(EXIT_FAILURE);

    checkArguments(argc, argv, &args);
    if (args.activatedOptions[PACKET_SIZE] == FALSE)
        args.optionsValue[PACKET_SIZE] = DEFAULT_PADDING;

    setDestinationAddress(&addrs[REPLY], args.domain);
    setSourceAddress(&addrs[REQUEST], &addrs[REPLY]);

    sockfd = initSocketFd();
    printBeginning(&args, sockfd, &addrs[REPLY]);
    FD_ZERO(&readfds);
    
    while(sequenceNumber < UINT16_MAX && g_exit == FALSE)
    {
        memset(&timeout, 0, sizeof(timeout));
        memset(&packets[REPLY], 0, sizeof(packets[REPLY]));
        initPacket(&packets[REQUEST]);
        handlePacketOptions(&packets[REQUEST], &args);
        defineRequestPacket(&packets[REQUEST], addrs[REQUEST].sin_addr.s_addr, addrs[REPLY].sin_addr.s_addr, ++sequenceNumber);
        gettimeofday(&start, NULL);
        triggerError(sendRequest(sockfd, &addrs[REPLY], &packets[REQUEST]) < 0, "sendto failed", sockfd);
        ++rtt.pkg_sent;
        timeout.tv_sec = (rtt.pkg_received == 0) ? 1 : (timeout.tv_usec = JITTER + rtt.mean, 0);
        while(g_exit == FALSE)
        {
            if (socketIsReady(sockfd, &readfds, &timeout) == FAILURE)
                break;
            triggerError(receiveResponse((void *)packets[REPLY].buffer, sockfd, sizeof(packets[REPLY].buffer)) < 0, "recvfrom failed", sockfd);
            gettimeofday(&end, NULL);
            if (getValidPacket(&packets[REPLY], &packets[REQUEST]) == FAILURE)
            {
                printPacketError(&packets[REPLY], sequenceNumber);
                break;
            }
            rtt_microseconds = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
            printReplyInfo(&packets[REPLY], rtt_microseconds, &args, args.domain);
            ++rtt.pkg_received;
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