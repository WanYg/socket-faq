/*
 *  Copyright (c) 2012-2013, Yang Wang. All rights reserved.
 *
 *  This file is provided for use with the unix-socket-faq. 
 *
 *  If you have found a bug, please pass it on to me at the above address
 *  acknowledging that there will be no copyright on your work.
 *
 */

#ifndef _SOCKHELP_H_
#define _SOCKHELP_H_

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <limits.h>
#include <netdb.h>
#include <arpa/inet.h>

int atoport(/* char *service, char *proto */);
struct in_addr *atoaddr(/* char *address */);
int get_connection(/* int socket_type, u_short port, int *listener */);
int make_connection(/* char *service, int type, char *netaddress */);
int sock_read(/* int sockfd, char *buf, size_t count */);
int sock_write(/* int sockfd, const char *buf, size_t count */);
int sock_gets(/* int sockfd, char *str, size_t count */);
int sock_puts(/* int sockfd, const char *str */);
void ignore_pipe(/*void*/);

#endif
