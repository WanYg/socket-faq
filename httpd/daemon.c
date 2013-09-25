/*
 *  Copyright (c) 2012-2013, Yang Wang. All rights reserved.
 *
 *  This file is provided for use with the unix-socket-faq. 
 *
 *  If you have found a bug, please pass it on to me at the above address
 *  acknowledging that there will be no copyright on your work.
 *
 */

#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define HTTP_PORT		80
#define RECV_MAX_BUFFER		1*1024
#define MAX_EPOLL_FDS		5000
#define EPOLL_MAX_EVENTS	1024
#define EPOLL_TIMEOUT		10

#define BUFFER "HTTP/1.1 200 OK\r\nContent-Length: 5\r\nConnection: close\r\nContent-Type: text/html\r\n\r\nHello"

typedef union {
	struct sockaddr sa;
	struct sockaddr_in sa_in;
#ifdef __CONFIG_IPV6__
	struct sockaddr_in6 sa_in6;
#endif //__CONFIG_IPV6__
} usockaddr;

/* Const vars */
const int int_1 = 1;

static int init_listen_socket(usockaddr * usaP)
{
	int listen_fd;

	if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
	{
		fprintf(stderr, "socket failed\n");
		return -1;
	}
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (const void*)&int_1, sizeof(int_1));

	memset(usaP, 0, sizeof(usockaddr));
#ifdef __CONFIG_IPV6__
	int n_version = usaP->sa.sa_family;
	if(n_version == AF_INET6) {
		usaP->sa.sa_family = AF_INET6;
		usaP->sa_in6.sin6_addr = in6addr_an6y;
		usaP->sa_in6.sin6_port = htons(HTTP_PORT);
	} else
#endif //__CONFIG_IPV6__
	{
		usaP->sa.sa_family = AF_INET;
		usaP->sa_in.sin_addr.s_addr = htonl(INADDR_ANY);
		usaP->sa_in.sin_port = htons(HTTP_PORT);
	}

	if (bind(listen_fd, (struct sockaddr *)&usaP->sa, sizeof(usockaddr)) != 0)
	{
		fprintf(stderr, "bind failed\n");
		return -1;
	}

	return listen_fd;
}

void setnonblocking(int fd)
{
	int opts;
	opts = fcntl(fd, F_GETFL);
	if (opts < 0)
	{
		fprintf(stderr, "fcntl failed\n");
		return;
	}
	opts = opts | O_NONBLOCK;
	if(fcntl(fd, F_SETFL, opts) < 0)
	{
		fprintf(stderr, "fcntl failed\n");
		return;
	}
	return;
}

void work_process_cycle(int listenfd, usockaddr* usaP)
{
	int nLoop, ret, connfd;
	char szbuffer[RECV_MAX_BUFFER];
	socklen_t sz_usa = sizeof(usaP);
	int epfd = epoll_create(MAX_EPOLL_FDS); //init epoll

	//...
	
	//add listenfd to epoll
	struct epoll_event ep_ev;
	ep_ev.data.fd = listenfd;
	ep_ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ep_ev);
	
	while(1)
	{
		//epoll events
		struct epoll_event events[EPOLL_MAX_EVENTS];
		int nfds = epoll_wait(epfd, events, EPOLL_MAX_EVENTS, EPOLL_TIMEOUT);
		
		for(nLoop = 0; nLoop < nfds; nLoop++) 
		{
			//accept connection
			if(events[nLoop].data.fd == listenfd)
			{
				int connfd = accept(listenfd, (struct sockaddr *) &usaP->sa, &sz_usa);

				setnonblocking(connfd);

				//add read event
				struct epoll_event ep_ev;
				ep_ev.data.fd = connfd;
				ep_ev.events = EPOLLIN | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ep_ev);
			}
			else if(events[nLoop].events & EPOLLIN)
			{
				//deal with read events
				connfd = events[nLoop].data.fd;
				ret = recv(connfd, szbuffer, sizeof(szbuffer),0);
				//printf("read ret(%d)=%s\n", ret, szbuffer);
				shutdown(connfd, 0);

				ep_ev.data.fd = connfd;
				ep_ev.events = EPOLLOUT | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_MOD, connfd, &ep_ev);
			}
			else if(events[nLoop].events & EPOLLOUT)
			{
				//deal with write events
				connfd = events[nLoop].data.fd;
				ret = send(connfd, BUFFER, strlen(BUFFER), 0);
				//printf("send ret...........= %d\n", ret);

				ep_ev.data.fd = connfd;
				epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, &ep_ev);

				fflush(connfd);
				//shutdown(connfd, 2);
				close(connfd);
			}
		}
		
		//other events
		//process events, timed events, etc.
	}
}

int main(int argc, char *argv[])
{
	int listen_fd;
	usockaddr usa;

	if((listen_fd = init_listen_socket(&usa)) < 0) {
		perror("Can't bind to any address\n");
		exit(errno);
	}
	if (listen(listen_fd, 32) != 0)
	{
		perror("listen failed\n");
		exit(errno);
	}

	if (daemon(1, 1) == -1) {
		perror("daemon");
		exit(errno);
        }

	work_process_cycle(listen_fd, &usa);

	if (listen_fd > 0)
	      close(listen_fd);

	return 0;
}
