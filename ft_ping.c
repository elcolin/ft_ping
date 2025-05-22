#include "ft_ping.h"


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