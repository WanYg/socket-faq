# Uncomment the following line for Solaris
# export C_LINK := -lsocket -lnsl

# Uncomment this for SCO.  (Note, this has only been reported to work with
# Revision 3.2.4 with the "SCO TCP/IP Development System" package installed.
# Please let me know if you have any other SCO success stories.
# export C_LINK := -lsocket

# Comment the following line if you are not using the gnu c compiler
export C_ARGS := -Wall -pipe #-O2 -g

# You might have to change this if your c compiler is not cc
export CC := cc

# You shouldn't need to make any more changes below this line.
TOP := $(shell pwd -P)
TARGET := tcpclient tcpserver udpclient udpserver nbserver
TARGET += smtp_vrfy
SMTP_VRFY := $(TOP)/stdio

all:	$(TARGET)

tcpclient:	tcpclient.o sockhelp.o
	$(CC) -o $@ $^ $(C_LINK)
tcpserver:	tcpserver.o sockhelp.o
	$(CC) -o $@ $^ $(C_LINK)
udpserver:	udpserver.o sockhelp.o
	$(CC) -o $@ $^ $(C_LINK)
udpclient:	udpclient.o sockhelp.o
	$(CC) -o $@ $^ $(C_LINK)
nbserver:	nbserver.o sockhelp.o
	$(CC) -o $@ $^ $(C_LINK)
smtp_vrfy:
	[ ! -d $(SMTP_VRFY) ] || $(MAKE) -C $(SMTP_VRFY)

%.o:	%.c
	$(CC) $(C_ARGS) -c $^

clean:
	rm -f *.o
%-clean:
	rm -f $* $*.o
distclean:	clean
	[ ! -d $(SMTP_VRFY) ] || $(MAKE) -C $(SMTP_VRFY) $@
	rm -f $(TARGET)
