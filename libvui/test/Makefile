
VUI = ../src
CFLAGS += -I$(VUI) -L$(VUI)

CFLAGS += -D_GNU_SOURCE -g -O0
LDFLAGS += -lvui -lpthread -lm -lcrypt
SRC += main.c netlink.c
OBJ = $(SRC:.c=.o)
TARGET = a.out

all: $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

