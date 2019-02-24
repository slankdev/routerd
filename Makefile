
CXX = g++
PREFIX = .
CXXFLAGS += -I$(PREFIX)/include -I$(PREFIX)/libslankdev
CXXFLAGS += -Wall -Werror -Wextra
CXXFLAGS += -std=c++11 -g -O0 -static
CXXFLAGS += $(shell pkg-config json-c --cflags)
LDFLAGS += $(shell pkg-config json-c --libs)

SRC = main.cc link.cc addr.cc route.cc neigh.cc log.cc
OBJ = $(SRC:.cc=.o)
TARGET = routerd.out

all: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

re: clean all

install: all
	mkdir -p /usr/local/bin
	mkdir -p /etc/routerd
	mkdir -p /etc/systemd/system
	cp $(TARGET) /usr/local/bin/routerd
	cp -f root/etc/routerd/config.json /etc/routerd/config.json
	cp -f root/etc/systemd/system/routerd.service /etc/systemd/system/routerd.service
	systemctl daemon-reload

run: all
	sudo ./$(TARGET)

clean_debian:
	rm -f  ../routerd-dbgsym_*.ddeb
	rm -f  ../routerd_*.dsc
	rm -f  ../routerd_*.tar.gz
	rm -f  ../routerd_*.build
	rm -f  ../routerd_*.buildinfo
	rm -f  ../routerd_*.changes
	rm -f  ../routerd_*.deb
	rm -f  ../routerd/debian/routerd.substvars
	rm -f  ../routerd/debian/debhelper-build-stamp
	rm -rf ../routerd/debian/routerd

install_tools:
	make -C tools/nldump install
	make -C tools/nlsniff install

install_tools_docker:
	make -C tools/nldump install_docker
	make -C tools/nlsniff install_docker

debian_package:
	debuild -uc -us

