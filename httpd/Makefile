C_ARGS ?= -Wall -pipe #-O2 -g
LD_ARGS ?= -lpthread

TARGET = httpd daemon

all:	$(TARGET)

httpd: httpd.o
	$(CC) -o $@ $^ $(LD_ARGS)
daemon: daemon.o
	$(CC) -o $@ $^ 

%.o: %.c
	$(CC) $(C_ARGS) -c $<

clean:
	rm -f *.o $(TARGET)
