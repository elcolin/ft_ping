#include "domain.h"

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

void setAddress(struct sockaddr_in *address, char *ip_address)
{
    memset(address, 0, sizeof(*address));
    address->sin_family = AF_INET;
    if(inet_pton(PF_INET, ip_address, &address->sin_addr) != 1 && resolveFQDN(ip_address, address) == FAILURE)
    {
        fprintf(stderr, "Invalid address: %s\n", ip_address);
        exit(EXIT_FAILURE);
    }
}

void setDestinationAddress(struct sockaddr_in *destAddress, char *ip_address)
{
    setAddress(destAddress, ip_address);
}

void setSourceAddress(struct sockaddr_in *srcAddress, char *ip_address)
{
    setAddress(srcAddress, ip_address);
}