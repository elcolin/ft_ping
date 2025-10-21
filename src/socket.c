#include "socket.h"

int initSocketFd()
{
    struct protoent *proto = getprotobyname("icmp");
    if (proto == NULL)
    {
        perror("getprotobyname");
        exit(EXIT_FAILURE);
    }
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

int sendRequest(int sockfd, struct sockaddr_in *destAddress, t_packet *request)
{
    return sendto(sockfd, (void *)request->buffer, sizeof(struct iphdr) + sizeof(struct icmphdr), 0, (struct sockaddr *)destAddress, sizeof(struct sockaddr_in));
}


int receiveResponse(void *buffer, int sockfd, u_int16_t buffer_size)
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    return recvfrom(sockfd, buffer, buffer_size, 0, (struct sockaddr *)&addr, &addr_len);
}

status socketIsReady(int sockfd, fd_set *readfds, struct timeval *timeout)
{
    FD_SET(sockfd, readfds);
    if (select(sockfd + 1, readfds, NULL, NULL, timeout) < 0)
    {
        perror("select failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    if (timeout->tv_sec == 0 && timeout->tv_usec == 0)
        return FAILURE;
    FD_CLR(sockfd, readfds);
    return SUCCESS;
}