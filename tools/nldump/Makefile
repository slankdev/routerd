
CXX = g++
CXXFLAGS += -Wall -Werror -std=c++11 -I../../ -I.
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

