
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
CFLAGS += -I$(HOME)/git/vpp/build-root/install-vpp_debug-native/vpp/include/vpp_plugins
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
SRC = main.cc log.cc tap.c \
  netlink_cli.cc netlink.cc \
  vpp.c vpp_cli.c \
  routerd.cc
OBJ = $(SRC:%=$(BUILD_DIR)/%.o)
TARGET = a.out

$(BUILD_DIR)/$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p ./build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cc.o: %.cc
	@mkdir -p ./build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run:
	sudo build/a.out

debug:
	sudo gdb -ex run build/a.out

create_tap:
	ip tuntap add vpp0 mode tap
	ip tuntap add vpp1 mode tap
	ip tuntap add vpp2 mode tap
	ip tuntap add vpp3 mode tap

delete_tap:
	ip link del vpp0
	ip link del vpp1
	ip link del vpp2
	ip link del vpp3

