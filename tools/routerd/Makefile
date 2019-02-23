
CXX = g++
CXXFLAGS += -Wall -Werror -std=c++11
CXXFLAGS += -I../../ -I../../libslankdev
CXXFLAGS += -fsanitize=address
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

install:
	mkdir -p /usr/local/bin
	cp $(TARGET) /usr/local/bin/routerd

