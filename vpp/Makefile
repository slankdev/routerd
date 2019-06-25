
CFLAGS += -Wall -Werror -Wextra
CFLAGS += -Wno-unused-parameter \
					-Wno-unused-variable \
					-Wno-return-type \
					-Wno-sign-compare \
					-Wno-type-limits \
					-Wno-unused-function
CFLAGS += -g -O0
CXXFLAGS += $(CFLAGS)

# for libvui
VUI = ../libvui/src
CFLAGS += -I$(VUI) -D_GNU_SOURCE
LDFLAGS += -L$(VUI) -lvui -lpthread -lm -lcrypt

# for yalin
YALIN = ../libyalin
CXXFLAGS += -I$(YALIN) -std=c++11
LDFLAGS += -L$(YALIN) -lyalin

# for vlib
CFLAGS += -I/usr/include/vpp_plugins
LDFLAGS += \
  -lvlibmemoryclient -lvatplugin \
  -lvppinfra -lvlib -lsvm -lrt -ldl \
  -lpthread -lm -lcrypt -lcrypto

# for config.h
CXXFLAGS += -I../libslankdev
CXXFLAGS += -std=c++11 -g -O0
CXXFLAGS += $(shell pkg-config json-c --cflags)
LDFLAGS += $(shell pkg-config json-c --libs)

BUILD_DIR = build
SRC = main.cc netlink_cli.c vpp.c tap.c netlink.cc log.cc
OBJ = $(SRC:%=$(BUILD_DIR)/%.o)
TARGET = a.out

$(BUILD_DIR)/$(TARGET): $(OBJ)
	@echo ld $@
	@$(CC) $(OBJ) -o $@ $(LDFLAGS) -lstdc++

$(BUILD_DIR)/%.c.o: %.c
	@echo cc $@
	@mkdir -p ./build
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cc.o: %.cc
	@echo g++ $@
	@mkdir -p ./build
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run:
	sudo build/a.out
