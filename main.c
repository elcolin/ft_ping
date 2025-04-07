#include "ft_ping.h"

struct sockaddr_in destAddress;
struct sockaddr_in srcAddress;

void triggerError(int condition, char *msg)
{
    if (condition)
    {
        perror(msg);
        exit(1);
    }
}

int initSocketFd()
{
    struct protoent *proto = getprotobyname("icmp");
    triggerError(!proto, "getprotobyname() failed");

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    triggerError(sockfd < 0, "socket() failed");
    printf("socket created\n");
    return sockfd;
}

uint16_t computeChecksum(uint8_t *addr, int count)
{
/* Compute Internet Checksum for "count" bytes
* beginning at location "addr".
*/
    uint16_t checksum;
    register uint32_t sum = 0;
    while( count > 1 ) {
    /* This is the inner loop */
    sum += *(addr++);
    count -= 2;
    }
    /* Add left-over byte, if any */
    if( count > 0 )
    sum += * (uint8_t *) addr;
    /* Fold 32-bit sum to 16 bits */
    while (sum>>16)
    sum = (sum & 0xffff) + (sum >> 16);
    checksum = ~sum;
    return checksum;
}

int defineICMPHeader()
{
    struct icmp icmpHeader;
    icmpHeader.icmp_type = ICMP_ECHO;
    icmpHeader.icmp_code = ICMP_CODE;
    icmpHeader.icmp_id = getpid() & 0xFFFF;
    icmpHeader.icmp_seq = ICMP_SEQUENCE_NUMBER;
    icmpHeader.icmp_cksum = computeChecksum((uint8_t *) &icmpHeader, sizeof(icmpHeader));
    return 0;
}

int main(int argc, char **argv)
{   
    struct timeval tv;
    (void) tv;
    triggerError(argc != 2, "Usage: ft_ping <destAddress>\n");
    
    printf("pinging %s\n", argv[1]);
    destAddress.sin_family = AF_INET;
    triggerError(inet_pton(PF_INET, argv[1], &destAddress) != 1, "malloc() failed\n");

    int sockfd = initSocketFd();

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