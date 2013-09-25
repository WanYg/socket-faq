/*
 *  Copyright (c) 2012-2013, Yang Wang. All rights reserved.
 *
 *  Non blocking server demo for the unix-socket-faq
 *
*/

#include "sockhelp.h"
#include <ctype.h>
#include <sys/time.h>
#include <fcntl.h>

int sock;            /* The socket file descriptor for our "listening"
                        socket */
int connectlist[5];  /* Array of connected sockets so we know who
                        we are talking to */
fd_set socks;        /* Socket file descriptors we want to wake
                        up for, using select() */
int highsock;        /* Highest #'d file descriptor, needed for select() */

void setnonblocking(sock)
int sock;
{
        int opts;

        opts = fcntl(sock,F_GETFL);
        if (opts < 0) {
                perror("fcntl(F_GETFL)");
                exit(EXIT_FAILURE);
        }
        opts = (opts | O_NONBLOCK);
        if (fcntl(sock,F_SETFL,opts) < 0) {
                perror("fcntl(F_SETFL)");
                exit(EXIT_FAILURE);
        }
        return;
}

void build_select_list() {
        int listnum;         /* Current item in connectlist for for loops */

        /* First put together fd_set for select(), which will
           consist of the sock veriable in case a new connection
           is coming in, plus all the sockets we have already
           accepted. */
        
        
        /* FD_ZERO() clears out the fd_set called socks, so that
                it doesn't contain any file descriptors. */
        
        FD_ZERO(&socks);
        
        /* FD_SET() adds the file descriptor "sock" to the fd_set,
                so that select() will return if a connection comes in
                on that socket (which means you have to do accept(), etc. */
        
        FD_SET(sock,&socks);
        
        /* Loops through all the possible connections and adds
                those sockets to the fd_set */
        
        for (listnum = 0; listnum < 5; listnum++) {
                if (connectlist[listnum] != 0) {
                        FD_SET(connectlist[listnum],&socks);
                        if (connectlist[listnum] > highsock)
                                highsock = connectlist[listnum];
                }
        }
}

void handle_new_connection() {
        int listnum;         /* Current item in connectlist for for loops */
        int connection; /* Socket file descriptor for incoming connections */

        /* We have a new connection coming in!  We'll
        try to find a spot for it in connectlist. */
        connection = accept(sock, NULL, NULL);
        if (connection < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
        }
        setnonblocking(connection);
        for (listnum = 0; (listnum < 5) && (connection != -1); listnum ++)
                if (connectlist[listnum] == 0) {
                        printf("\nConnection accepted:   FD=%d; Slot=%d\n",
                                connection,listnum);
                        connectlist[listnum] = connection;
                        connection = -1;
                }
        if (connection != -1) {
                /* No room left in the queue! */
                printf("\nNo room left for new client.\n");
                sock_puts(connection,"Sorry, this server is too busy.  \
                                        Try again later!\r\n");
                close(connection);
        }
}

void deal_with_data(
        int listnum                     /* Current item in connectlist for for loops */
        ) {
        char buffer[80];     /* Buffer for socket reads */
        char *cur_char;      /* Used in processing buffer */

        if (sock_gets(connectlist[listnum],buffer,80) < 0) {
                /* Connection closed, close this end
                   and free up entry in connectlist */
                printf("\nConnection lost: FD=%d;  Slot=%d\n",
                        connectlist[listnum],listnum);
                close(connectlist[listnum]);
                connectlist[listnum] = 0;
        } else {
                /* We got some data, so upper case it
                   and send it back. */
                printf("\nReceived: %s; ",buffer);
                cur_char = buffer;
                while (cur_char[0] != 0) {
                        cur_char[0] = toupper(cur_char[0]);
                        cur_char++;
                }
                sock_puts(connectlist[listnum],buffer);
                sock_puts(connectlist[listnum],"\n");
                printf("responded: %s\n",buffer);
        }
}

void read_socks() {
        int listnum;         /* Current item in connectlist for for loops */

        /* OK, now socks will be set with whatever socket(s)
           are ready for reading.  Lets first check our
           "listening" socket, and then check the sockets
           in connectlist. */
        
        /* If a client is trying to connect() to our listening
                socket, select() will consider that as the socket
                being 'readable'. Thus, if the listening socket is
                part of the fd_set, we need to accept a new connection. */
        
        if (FD_ISSET(sock,&socks))
                handle_new_connection();
        /* Now check connectlist for available data */
        
        /* Run through our sockets and check to see if anything
                happened with them, if so 'service' them. */
        
        for (listnum = 0; listnum < 5; listnum++) {
                if (FD_ISSET(connectlist[listnum],&socks))
                        deal_with_data(listnum);
        } /* for (all entries in queue) */
}

int main (argc, argv)
int argc;
char *argv[];
{
        char *ascport;  /* ASCII version of the server port */
        int port;       /* The port number after conversion from ascport */
        struct sockaddr_in server_address; /* bind info structure */
        int reuse_addr = 1;  /* Used so we can re-bind to our port
                                while a previous connection is still
                                in TIME_WAIT state. */
        struct timeval timeout;  /* Timeout for select */
        int readsocks;       /* Number of sockets ready for reading */

        /* Make sure we got a port number as a parameter */
        if (argc < 2) {
                printf("Usage: %s PORT\r\n",argv[0]);
                exit(EXIT_FAILURE);
        }

        /* Obtain a file descriptor for our "listening" socket */
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
                perror("socket");
                exit(EXIT_FAILURE);
        }
        /* So that we can re-bind to it without TIME_WAIT problems */
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse_addr,
                sizeof(reuse_addr));

        /* Set socket to non-blocking with our setnonblocking routine */
        setnonblocking(sock);

        /* Get the address information, and bind it to the socket */
        ascport = argv[1]; /* Read what the user gave us */
/*ERR.CYGWIN?:printf("\n");fflush(stdout);*/
        port = atoport(ascport,NULL); /* Use function from sockhelp to
                                    convert to an int */
        memset((char *) &server_address, 0, sizeof(server_address));
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = htonl(INADDR_ANY);
        server_address.sin_port = port;
        if (bind(sock, (struct sockaddr *) &server_address,
          sizeof(server_address)) < 0 ) {
                perror("bind");
                close(sock);
                exit(EXIT_FAILURE);
        }

        /* Set up queue for incoming connections. */
        listen(sock,5);

        /* Since we start with only one socket, the listening socket,
           it is the highest socket so far. */
        highsock = sock;
        memset((char *) &connectlist, 0, sizeof(connectlist));

        while (1) { /* Main server loop - forever */
                build_select_list();
                timeout.tv_sec = 1;
                timeout.tv_usec = 0;
                
                /* The first argument to select is the highest file
                        descriptor value plus 1. In most cases, you can
                        just pass FD_SETSIZE and you'll be fine. */

                /* The second argument to select() is the address of
                        the fd_set that contains sockets we're waiting
                        to be readable (including the listening socket). */

                /* The third parameter is an fd_set that you want to
                        know if you can write on -- this example doesn't
                        use it, so it passes 0, or NULL. The fourth parameter
                        is sockets you're waiting for out-of-band data for,
                        which usually, you're not. */

                /* The last parameter to select() is a time-out of how
                        long select() should block. If you want to wait forever
                        until something happens on a socket, you'll probably
                        want to pass NULL. */
                
                readsocks = select(highsock+1, &socks, (fd_set *) 0,
                  (fd_set *) 0, &timeout);
                
                /* select() returns the number of sockets that had
                        things going on with them -- i.e. they're readable. */

                /* Once select() returns, the original fd_set has been
                        modified so it now reflects the state of why select()
                        woke up. i.e. If file descriptor 4 was originally in
                        the fd_set, and then it became readable, the fd_set
                        contains file descriptor 4 in it. */
                
                if (readsocks < 0) {
                        perror("select");
                        exit(EXIT_FAILURE);
                }
                if (readsocks == 0) {
                        /* Nothing ready to read, just show that
                           we're alive */
                        printf(".");
                        fflush(stdout);
                } else
                        read_socks();
        } /* while(1) */
} /* main */
