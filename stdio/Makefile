# Uncomment the following line for Solaris
# C_LINK = -lsocket -lnsl

# Uncomment this for SCO.  (Note, this has only been reported to work with
# Revision 3.2.4 with the "SCO TCP/IP Development System" package installed.
# Please let me know if you have any other SCO success stories.
# C_LINK = -lsocket

# Comment the following line if you are not using the gnu c compiler
C_ARGS ?= -Wall -pipe #-O2 -g

# You might have to change this if your c compiler is not cc
CC ?= cc

# You shouldn't need to make any more changes below this line.
TARGET = smtp_vrfy
C_ARGS += -I. -I..

all:	$(TARGET)

smtp_vrfy:	smtp_vrfy.o sockutil.o
	$(CC) -o $@ $^ $(C_LINK)

%.o:	%.c
	$(CC) $(C_ARGS) -c $^

clean:
	rm -f *.o
%-clean:
	rm -f $* $*.o
distclean:	clean
	rm -f $(TARGET)
