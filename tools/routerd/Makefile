
CXX = g++
CXXFLAGS += -Wall -Werror -std=c++11 -g -O0
CXXFLAGS += -I../../ -I../../libslankdev -static
CXXFLAGS += $(shell pkg-config json-c --cflags)
LDFLAGS += $(shell pkg-config json-c --libs)
LDFLAGS +=

SRC = main.cc link.cc addr.cc route.cc neigh.cc
OBJ = $(SRC:.cc=.o)
TARGET = routerd.out

all: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

re: clean all

install: all
	mkdir -p /usr/local/bin
	cp $(TARGET) /usr/local/bin/routerd

run: all
	sudo ./$(TARGET)

