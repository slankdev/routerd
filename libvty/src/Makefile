
FRR_ROOT = $(HOME)/git/frr
CFLAGS += -I$(FRR_ROOT)/lib -I$(FRR_ROOT)
LDFLAGS +=  -lfrr -lpthread -ljson-c -lyang

SRC = main.c test_cli.c
OBJ = $(SRC:.c=.o)
TARGET = a.out

all: $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

