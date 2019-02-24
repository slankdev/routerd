
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

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

re: clean $(TARGET)

install: $(TARGET) tools
	mkdir -p /usr/local/bin
	mkdir -p /etc/routerd
	mkdir -p /etc/systemd/system
	cp $(TARGET) /usr/local/bin/routerd
	cp -f root/etc/routerd/config.json /etc/routerd/config.json
	cp -f root/etc/systemd/system/routerd.service /etc/systemd/system/routerd.service
	systemctl daemon-reload

clean_debian:
	rm -f  debian/routerd.substvars
	rm -f  debian/debhelper-build-stamp
	rm -rf debian/routerd
	rm -rf debian/output

debian_package:
	debuild -uc -us
	mkdir -p debian/output
	mv ../routerd-dbgsym_*.ddeb debian/output/
	mv ../routerd_*.dsc debian/output/
	mv ../routerd_*.tar.gz debian/output/
	mv ../routerd_*.build debian/output/
	mv ../routerd_*.buildinfo debian/output/
	mv ../routerd_*.changes debian/output/
	mv ../routerd_*.deb debian/output/

