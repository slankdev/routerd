
CC = gcc
CFLAGS += -I/usr/include/vpp_plugins -g -O0
LDFLAGS += \
        -lvlibmemoryclient \
        -lsvm \
        -lvatplugin \
        -lvppinfra  \
        -lvlib \
        -lpthread -lm -lrt -ldl -lcrypto


SRC = main.c
OBJ = $(SRC:.c=.o)
TARGET = a.out

all: $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

