
FRR_ROOT = ./
CFLAGS += -I$(FRR_ROOT)/lib -I$(FRR_ROOT) -D_GNU_SOURCE $(shell pkg-config --cflags glib-2.0)
# LDFLAGS += -lfrr
LDFLAGS += lib/vty.o \
					 lib/northbound.o \
					 lib/northbound_cli.o \
					 lib/northbound_db.o \
					 lib/yang.o \
					 lib/yang_translator.o \
					 lib/prefix.o \
					 lib/memory.o \
					 lib/command.o \
					 lib/hash.o \
					 lib/jhash.o \
					 lib/termtable.o \
					 lib/linklist.o \
					 lib/libfrr.o \
					 lib/thread.o \
					 lib/network.o \
					 lib/pqueue.o \
					 lib/yang_wrappers.o \
					 lib/sigevent.o \
					 lib/frrstr.o \
					 lib/vector.o \
					 lib/log.o \
					 lib/printf/vfprintf.o \
					 lib/printf/glue.o \
					 lib/printf/printf-pos.o \
					 lib/memory_vty.o \
					 lib/openbsd-tree.o \
					 lib/strlcpy.o \
					 lib/strlcat.o \
					 lib/module.o \
					 lib/debug.o \
					 lib/sockunion.o \
					 lib/privs.o \
					 lib/buffer.o \
					 lib/qobj.o \
					 lib/vrf.o \
					 lib/netns_linux.o \
					 lib/netns_other.o \
					 lib/filter.o \
					 lib/routemap.o \
					 lib/typesafe.o \
					 lib/table.o \
					 lib/command_graph.o \
					 lib/command_match.o \
					 lib/command_parse.o \
					 lib/command_lex.o \
					 lib/workqueue.o \
					 lib/ferr.o \
					 lib/hook.o \
					 lib/pid_output.o \
					 lib/graph.o \
					 lib/if.o \
					 lib/nexthop.o \
					 lib/nexthop_group.o \
					 lib/json.o \
					 lib/zclient.o \
					 lib/stream.o \
					 lib/lib_errors.o \
					 lib/sockopt.o \
					 # end
LDFLAGS += -lpthread -ljson-c -lyang -lm $(shell pkg-config --libs glib-2.0) -ldl -lcrypt -lunwind

SRC = main.c test_cli.c
OBJ = $(SRC:.c=.o)
TARGET = a.out

all: $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

