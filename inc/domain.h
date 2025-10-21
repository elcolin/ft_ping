#ifndef DOMAIN_H
#define DOMAIN_H

#include "utils.h"
#include <sys/socket.h>
// EXIT_FAILURE
#include <stdlib.h>
// getaddrinfo
#include <netdb.h>
// stderr
#include <stdio.h>
// inet_pton
#include <arpa/inet.h>
//memset
#include <string.h>

status resolveFQDN(char *fqdn, struct sockaddr_in *addr);
void setDestinationAddress(struct sockaddr_in *destAddress, char *ip_address);

#endif