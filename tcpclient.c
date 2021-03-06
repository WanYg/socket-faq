/*
 *  Copyright (c) 2012-2013, Yang Wang. All rights reserved.
 *
 *  This file is provided for use with the unix-socket-faq.
 *
 *  If you have found a bug, please pass it on to me at the above address
 *  acknowledging that there will be no copyright on your work.
 *
 */

#include "sockhelp.h"
#include <stdio.h>
#include <unistd.h>

int main(argc, argv)
int argc;
char *argv[];
{
  int sock;
  char buffer[1024];
  int connected = 1;

  if (argc != 3) {
    fprintf(stderr,"Usage:  tcpclient host port\n");
    fprintf(stderr,"where host is the machine which is running the\n");
    fprintf(stderr,"tcpserver program, and port is the port it is\n");
    fprintf(stderr,"listening on.\n");
    exit(EXIT_FAILURE);
  }
  ignore_pipe();
  sock = make_connection(argv[2], SOCK_STREAM, argv[1]);
  if (sock == -1) {
    fprintf(stderr,"make_connection failed.\n");
    return -1;
  }
  sock_gets(sock,buffer,sizeof(buffer)-1); /* -1 added thanks to
    David Duchene <dave@ltd.com> for pointing out the possible
    buffer overflow resulting from the linefeed added below. */
  printf("Server: %s\n",buffer);
  while (connected) {
    printf("Client: ");
    if ((fgets(buffer,1024,stdin) == NULL) || (buffer[0] == 0))
      connected = 0;
    else {
      strcat(buffer,"\n"); /* Add line-feed */
      sock_puts(sock,buffer);
      if (sock_gets(sock,buffer,sizeof(buffer)) == -1) {
        connected = 0;
      }
      printf("Server: %s\n",buffer);
    }
  }
  printf("<CLOSED>\n");
  close(sock);
  return 0;
}
