
EXTRA_CFLAGS=
EXTRA_CXXFLAGS=

CFLAGS += -Wall -Werror -Wextra
CFLAGS += -Wno-unused-parameter \
					-Wno-unused-variable \
					-Wno-return-type \
					-Wno-sign-compare \
					-Wno-type-limits \
					-Wno-unused-function \
					-Wno-error
CFLAGS += -g -O0 $(EXTRA_CFLAGS)
CXXFLAGS += $(CFLAGS) $(EXTRA_CXXFLAGS)

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
CFLAGS += -I$(HOME)/git/vpp/build-root/install-vpp_debug-native/vpp/include/
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
  routerd.cc debug.cc
OBJ = $(SRC:%=$(BUILD_DIR)/%.o)
TARGET = routerd.out

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
	rm -f  debian/routerd.substvars
	rm -f  debian/debhelper-build-stamp
	rm -rf debian/routerd
	rm -rf debian/output

run:
	sudo build/routerd.out --conf ./routerd.conf --interactive

debug:
	sudo gdb -ex 'run --conf ./routerd.conf --interactive' build/routerd.out

pkg-deb:
	debuild -uc -us
	mkdir -p build
	mv ../routerd-dbgsym_*.ddeb build
	mv ../routerd_*.dsc build
	mv ../routerd_*.tar.gz build
	mv ../routerd_*.build build
	mv ../routerd_*.buildinfo build
	mv ../routerd_*.changes build
	mv ../routerd_*.deb build

