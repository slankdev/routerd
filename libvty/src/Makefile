
CFLAGS += -I./lib -I. -D_GNU_SOURCE $(shell pkg-config --cflags glib-2.0)
CFLAGS += -DSYSCONFDIR=\"/etc/\" -DHAVE_CONFIG_H
SRC += lib/vty.c \
					 lib/northbound.c \
					 lib/northbound_cli.c \
					 lib/northbound_db.c \
					 lib/yang.c \
					 lib/yang_translator.c \
					 lib/yang_wrappers.c \
					 lib/printf/vfprintf.c \
					 lib/printf/glue.c \
					 lib/printf/printf-pos.c \
					 lib/prefix.c \
					 lib/memory.c \
					 lib/command.c \
					 lib/hash.c \
					 lib/jhash.c \
					 lib/termtable.c \
					 lib/linklist.c \
					 lib/libfrr.c \
					 lib/thread.c \
					 lib/network.c \
					 lib/pqueue.c \
					 lib/sigevent.c \
					 lib/frrstr.c \
					 lib/vector.c \
					 lib/log.c \
					 lib/memory_vty.c \
					 lib/openbsd-tree.c \
					 lib/strlcpy.c \
					 lib/strlcat.c \
					 lib/module.c \
					 lib/debug.c \
					 lib/sockunion.c \
					 lib/privs.c \
					 lib/buffer.c \
					 lib/qobj.c \
					 lib/vrf.c \
					 lib/netns_linux.c \
					 lib/netns_other.c \
					 lib/typesafe.c \
					 lib/table.c \
					 lib/command_graph.c \
					 lib/command_match.c \
					 lib/command_parse.c \
					 lib/command_lex.c \
					 lib/workqueue.c \
					 lib/ferr.c \
					 lib/hook.c \
					 lib/pid_output.c \
					 lib/graph.c \
					 lib/if.c \
					 lib/nexthop.c \
					 lib/nexthop_group.c \
					 lib/json.c \
					 lib/zclient.c \
					 lib/stream.c \
					 lib/lib_errors.c \
					 lib/sockopt.c \
					 # end
LDFLAGS += -lpthread -ljson-c -lyang -lm $(shell pkg-config --libs glib-2.0) -ldl -lcrypt -lunwind

SRC += main.c
OBJ = $(SRC:.c=.o)
TARGET = a.out

all: $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

