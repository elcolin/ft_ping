#include "ft_ping.h"

volatile sig_atomic_t g_exit = FALSE;

void handlesigint(int sig)
{
    g_exit = TRUE;
    (void)sig;
}

int main(int argc, char **argv)
{
    int                 sockfd;
    fd_set              readfds;
    struct timeval      beginExec = {}, endExec = {};
    struct timeval      start, end, timeout;
    long                rtt_microseconds = 0;
    t_rtt               rtt = {};
    t_args              args = {};
    u_int16_t           sequenceNumber = 0;
    t_packet            packets[PACKET_NUMBER] = {0};
    struct sockaddr_in  addrs[PACKET_NUMBER] = {0};
    size_t              error_cnt = 0;

    triggerErrorNoFreeingIf(signal(SIGINT, handlesigint) == SIG_ERR, "Signal Failed\n", "");

    checkArguments(argc, argv, &args);
    initDefaultOptions(&args);

    setDestinationAddress(&addrs[REPLY], args.domain);
    setSourceAddress(&addrs[REQUEST], &addrs[REPLY]);

    sockfd = initSocketFd();
    printBeginning(&args, sockfd, &addrs[REPLY]);
    FD_ZERO(&readfds);
    gettimeofday(&beginExec, NULL);
    
    while(sequenceNumber < UINT16_MAX && g_exit == FALSE)
    {
        memset(&timeout, 0, sizeof(timeout));
        memset(&packets[REPLY], 0, sizeof(packets[REPLY]));
        initPacket(&packets[REQUEST]);
        handlePacketOptions(&packets[REQUEST], &args);
        defineRequestPacket(&packets[REQUEST], addrs[REQUEST].sin_addr.s_addr, addrs[REPLY].sin_addr.s_addr, ++sequenceNumber);
        timeout.tv_sec = (rtt.pkg_received == 0) ? 1 : (timeout.tv_usec = JITTER + rtt.mean, 0);
        gettimeofday(&start, NULL);
        triggerErrorIf(sendRequest(sockfd, &addrs[REPLY], &packets[REQUEST]) < 0, "sendto failed", sockfd);
        ++rtt.pkg_sent;
        while(g_exit == FALSE)
        {
            if (socketIsReady(sockfd, &readfds, &timeout) == FAILURE)
                break;
            triggerErrorIf(receiveResponse((void *)packets[REPLY].buffer, sockfd, sizeof(packets[REPLY].buffer)) < 0, "recvfrom failed", sockfd);
            gettimeofday(&end, NULL);
            if (getValidPacket(&packets[REPLY], &packets[REQUEST]) == FAILURE)
            {
                if (printPacketError(&packets[REPLY], sequenceNumber) == TRUE)
                {
                    error_cnt++;
                    break;
                }
                continue;
            }
            rtt_microseconds = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
            printReplyInfo(&packets[REPLY], rtt_microseconds, &args, args.domain);
            ++rtt.pkg_received;
            rttUpdate(&rtt, rtt_microseconds);
            break;
        }
        sleep(1);
    }
    gettimeofday(&endExec, NULL);
    finalizeMeanDeviation(&rtt);
    printStatistics(&rtt, args.domain, error_cnt, get_elapsed_ms(beginExec, endExec));
    close(sockfd);
    return 0;
}