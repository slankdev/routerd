
CFLAGS += -I./lib -I. -D_GNU_SOURCE -g -O0
SRC += lib/vty.c \
			 lib/printf/vfprintf.c \
			 lib/printf/glue.c \
			 lib/printf/printf-pos.c \
			 lib/prefix.c \
			 lib/memory.c \
			 lib/hash.c \
			 lib/linklist.c \
			 lib/thread.c \
			 lib/network.c \
			 lib/pqueue.c \
			 lib/sigevent.c \
			 lib/frrstr.c \
			 lib/vector.c \
			 lib/strlcpy.c \
			 lib/strlcat.c \
			 lib/sockunion.c \
			 lib/privs.c \
			 lib/buffer.c \
			 lib/qobj.c \
			 lib/typesafe.c \
			 lib/command.c \
			 lib/command_graph.c \
			 lib/command_match.c \
			 lib/command_parse.c \
			 lib/command_lex.c \
			 lib/graph.c \
			 lib/sockopt.c \
			 # end
LDFLAGS += -lpthread -lm -lcrypt

SRC += main.c
OBJ = $(SRC:.c=.o)
TARGET = a.out

all: $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

