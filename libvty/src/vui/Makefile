
CFLAGS += -I. -I./lib
CFLAGS += -D_GNU_SOURCE -g -O0
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
			 lib/command_node.c \
			 lib/command_match.c \
			 lib/command_parse.c \
			 lib/command_lex.c \
			 lib/graph.c \
			 lib/sockopt.c \
			 vui.c
			 # end
OBJ = $(SRC:.c=.o)
TARGET = libvui.a

all: pre_build $(OBJ)
	$(AR) rcs $(TARGET) $(OBJ)
	mv $(TARGET) ../

pre_build: lib/command_lex.c lib/command_parse.c

lib/command_lex.c: lib/command_lex.l
	flex lib/command_lex.l

lib/command_parse.c: lib/command_parse.y
	bison -d -Dapi.prefix={cmd_yy} \
		-Dparse.error=verbose -Wno-yacc \
		lib/command_parse.y

clean:
	rm -f $(OBJ) $(TARGET)
	rm -f lib/command_lex.c \
		lib/command_lex.h \
		lib/command_parse.c \
		lib/command_parse.h

