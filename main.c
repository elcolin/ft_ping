#include "ft_ping.h"

void triggerError(int condition, char *msg)
{
    if (condition)
    {
        perror(msg);
        exit(1);
    }
}

int main(int argc, char **argv)
{
    triggerError(argc != 2, "Usage: ft_ping <ipAdress>\n");
    
    printf("pinging %s\n", argv[1]);
    struct sockaddr_in ipAdress;
    triggerError(inet_pton(PF_INET, argv[1], &ipAdress) != 1, "malloc() failed\n");

    struct protoent *proto = getprotobyname("icmp");
    triggerError(!proto, "getprotobyname() failed");

    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);

    // char buffer[1024];
    // socklen_t len = sizeof(buffer);
    // getsockopt(sockfd, IPPROTO_ICMP, &buffer, len);
    triggerError(sockfd < 0, "socket() failed");
    printf("socket created\n");



}