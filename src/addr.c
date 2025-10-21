#include "addr.h"

status resolveFQDN(char *fqdn, struct sockaddr_in *addr)
{
    struct addrinfo hints;
    struct addrinfo *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    if (getaddrinfo(fqdn, NULL, &hints, &result) != 0)
    {
       perror("getaddrinfo failed");
       return FAILURE;
    }
    memcpy(addr, result->ai_addr, sizeof(struct sockaddr_in));
    freeaddrinfo(result);
    return SUCCESS;
}

void setDestinationAddress(struct sockaddr_in *destAddress, char *ip_address)
{
    memset(destAddress, 0, sizeof(*destAddress));
    destAddress->sin_family = AF_INET;
    if(inet_pton(PF_INET, ip_address, &destAddress->sin_addr) != 1 && resolveFQDN(ip_address, destAddress) == FAILURE)
    {
        fprintf(stderr, "Invalid address: %s\n", ip_address);
        exit(EXIT_FAILURE);
    }
}

void setSourceAddress(struct sockaddr_in *srcAddress, const struct sockaddr_in *destAddress)
{
    memset(srcAddress, 0, sizeof(*srcAddress));
    struct ifaddrs *networkInterfaces = NULL;
    
    getifaddrs(&networkInterfaces);
    struct ifaddrs *idx = networkInterfaces;
    while (idx)
    {
        if ((idx->ifa_addr->sa_family == destAddress->sin_family) 
            && !(idx->ifa_flags & IFF_LOOPBACK) 
            && (idx->ifa_flags & IFF_BROADCAST) 
            && (idx->ifa_flags & IFF_UP))
        {
            memcpy(srcAddress, idx->ifa_addr, sizeof(struct sockaddr_in));
            freeifaddrs(networkInterfaces);
            return;
        }
        idx = idx->ifa_next;
    }
    fprintf(stderr, "Coulnd't get host address");
    exit(EXIT_FAILURE);
}