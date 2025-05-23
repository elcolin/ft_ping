#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <sys/select.h>
// getprotobyname
#include <netdb.h>
// EXIT_FAILURE
#include <stdlib.h>
// perror
#include <stdio.h>
// close
#include <unistd.h>
#include "booleans.h"

int initSocketFd();
int sendRequest(int sockfd, struct sockaddr_in *destAddress, struct icmphdr *icmpHeader);
int receiveResponse(char *buffer, int sockfd, u_int16_t buffer_size);
status socketIsReady(int sockfd, fd_set *readfds, struct timeval *timeout);

#endif
