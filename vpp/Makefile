
VUI = ../libvui/src
CFLAGS += -I$(VUI) -L$(VUI)
CFLAGS += -D_GNU_SOURCE
CFLAGS += -g -O0
CFLAGS += -I/usr/include/vpp_plugins

LDFLAGS += \
  -lvlibmemoryclient -lvatplugin \
  -lvppinfra -lvlib -lsvm -lrt -ldl \
  -lvui -lpthread -lm -lcrypt -lcrypto

SRC = main.c netlink.c vpp.c
OBJ = $(SRC:.c=.o)
TARGET = a.out

all: $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

