
CXX = g++
CXXFLAGS += -Wall -Werror -Wextra
CXXFLAGS += -std=c++11 -g -O0
CXXFLAGS += -I../../ -I../../libslankdev -static
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

uninstall:
	rm -f /usr/local/bin/routerd
	rm -f /etc/routerd/config.json
	rm -f /etc/systemd/system/routerd.service
	systemctl daemon-reload

run: all
	sudo ./$(TARGET)

