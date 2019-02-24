
CXX = g++
PREFIX = ../../
CXXFLAGS += -I$(PREFIX)/include -I$(PREFIX)/libslankdev
CXXFLAGS += -Wall -Werror -std=c++11
CXXFLAGS += -fsanitize=address
LDFLAGS +=

SRC = main.cc
OBJ = $(SRC:.cc=.o)
TARGET = nldump.out

all: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

re: clean all

install:
	mkdir -p /usr/local/bin
	cp $(TARGET) /usr/local/bin/nldump

