CFLAGS = -Os -Werror -Wmissing-declarations
CFLAGS += -Wall
CFLAGS += --std=gnu99 
CPPFLAGS += -I. 

TARGET_LIB = libcrypt.a
TARGET = $(TARGET_LIB)
LIB_OBJS = url.o md5.o md4.o aes128.o crc32c.o sha1.o sha256.o

all: $(TARGET)

$(TARGET_LIB): $(LIB_OBJS)
	$(AR) rcs $@ $^

clean:
	$(RM) *.o $(TARGET)
