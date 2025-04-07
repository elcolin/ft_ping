#include "ft_ping.h"

struct sockaddr_in destAddress;
struct sockaddr_in srcAddress;



int main(int argc, char **argv)
{   
    struct timeval tv;
    (void) tv;
    triggerError(argc != 2, "Usage: ft_ping <destAddress>\n");
    
    printf("pinging %s\n", argv[1]);
    destAddress.sin_family = AF_INET;
    triggerError(inet_pton(PF_INET, argv[1], &destAddress) != 1, "malloc() failed\n");

    int sockfd = initSocketFd();
    (void) sockfd;

    return 0;
}
// int defineIPHeader()
// {
//     struct ip ipHeader;
//     ipHeader.ip_v = IP_ICMP_VERSION;
//     ipHeader.ip_hl = IP_ICMP_HEADER_LENGTH;
//     ipHeader.ip_tos = IP_ICMP_TOS;
//     ipHeader.ip_len = IP_ICMP_TOTAL_LENGTH;
//     ipHeader.ip_id = IP_ICMP_ID;
//     ipHeader.ip_off = IP_ICMP_FLAGS;
//     ipHeader.ip_ttl = IP_ICMP_TTL;
//     ipHeader.ip_p = IP_ICMP_PROTOCOL;
//     ipHeader.ip_dst = destAddress.sin_addr;
//     ipHeader.ip_src = srcAddress.sin_addr;
//     ipHeader.ip_sum = computeChecksum( (uint8_t *) &ipHeader, IP_ICMP_HEADER_LENGTH * 4);
//     return 0;
// }
    // struct ifaddrs *ifap;
    // triggerError(getifaddrs(&ifap) == -1, "getifaddrs() failed\n");

    // struct ifaddrs *idx = ifap;
    // while (idx)
    // {
    //     //printf("interface name: %s\n", idx->ifa_name);
    //     if (idx->ifa_addr->sa_family == AF_INET && strcmp(idx->ifa_name, "en0") == 0)
    //     {
    //         srcAddress = *(struct sockaddr_in *)idx->ifa_addr;
    //         //printf("ip address: %s\n", inet_ntoa(addr->sin_addr));
    //         break;
    //     }
    //     idx = idx->ifa_next;
    // }
    // defineIPHeader();
    // freeifaddrs(ifap);