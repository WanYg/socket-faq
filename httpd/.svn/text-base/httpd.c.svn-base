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
#include <string.h>
#include <pthread.h>
#include <errno.h>

#define HTTP_PORT 8888
#define MAXFDS 5000
#define EVENTSIZE 1024

#define BUFFER "HTTP/1.1 200 OK\r\nContent-Length: 5\r\nConnection: close\r\nContent-Type: text/html\r\n\r\nHello"

typedef union {
	struct sockaddr sa;
	struct sockaddr_in sa_in;
#ifdef __CONFIG_IPV6__
	struct sockaddr_in6 sa_in6;
#endif //__CONFIG_IPV6__
} usockaddr;

int epfd;
void *serv_epoll(void *p);
const int opt=1;

static int init_listen_socket(usockaddr * usaP)
{
	int listen_fd;

	if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
	{
		fprintf(stderr, "socket failed\n");
		return -1;
	}
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (const void*)&opt, sizeof(opt));

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
	opts=fcntl(fd, F_GETFL);
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

int main(int argc, char *argv[])
{
	int fd, cfd;
	usockaddr usa;
	socklen_t sz_usa = sizeof(usa);
	struct epoll_event ev;
	pthread_t tid;
	pthread_attr_t attr;

	epfd = epoll_create(MAXFDS);

	if((fd = init_listen_socket(&usa)) < 0) {
		fprintf(stderr, "Can't bind to any address\n");
		return -1;
	}
	if (listen(fd, 32) != 0)
	{
		fprintf(stderr, "listen failed\n");
		return -1;
	}

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	if (pthread_create(&tid, &attr, serv_epoll, NULL) != 0)
	{
		fprintf(stderr, "pthread_create failed\n");
		return -1;
	}

	while ((cfd = accept(fd, (struct sockaddr *)&usa.sa, &sz_usa)) > 0)
	{
		setnonblocking(cfd);
		ev.data.fd = cfd;
		ev.events = EPOLLIN | EPOLLET;
		epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
		//printf("connect from %s ",inet_ntoa(usa.sa.sin_addr));
		//printf("cfd=%d\n",cfd);
	}

	if (fd > 0)
	      close(fd);
	return 0;
}

void *serv_epoll(void *p)
{
	int i, ret, cfd, nfds;;
	struct epoll_event ev,events[EVENTSIZE];
	char buffer[512];

	while (1)
	{
		nfds = epoll_wait(epfd, events, EVENTSIZE , -1);
		//printf("nfds ........... %d\n",nfds);
		for (i=0; i<nfds; i++)
		{
			if(events[i].events & EPOLLIN)
			{
				cfd = events[i].data.fd;
				ret = recv(cfd, buffer, sizeof(buffer),0);
				//printf("read ret..........= %d\n",ret);

				ev.data.fd = cfd;
				ev.events = EPOLLOUT | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_MOD, cfd, &ev);
			}
			else if(events[i].events & EPOLLOUT)
			{
				cfd = events[i].data.fd;
				ret = send(cfd, BUFFER, strlen(BUFFER), 0);
				//printf("send ret...........= %d\n", ret);

				ev.data.fd = cfd;
				epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, &ev);
				//shutdown(cfd, 1);
				close(cfd);
			}
		}
	}
	return NULL;
}
