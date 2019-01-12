
CFLAGS += -I../
LDFLAGS += -L../

.c.out:
	@echo "  [CC]   $@"
	@$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

SRC = $(wildcard *.c)
OUT = $(SRC:.c=.out)

all: $(OUT)
	@echo All Compilation is successed.

clean:
	rm -f $(OUT) a.out
