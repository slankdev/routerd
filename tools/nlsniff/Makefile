
CXX = g++
CXXFLAGS += -Wall -Werror -std=c++11 -I../../
CXXFLAGS += -fsanitize=address
LDFLAGS +=

SRC = main.cc
OBJ = $(SRC:.cc=.o)
TARGET = nlsniff

all: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET).out $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET).out $(TARGET)

install:
	cp $(TARGET).out /usr/local/bin/$(TARGET)

re: clean all

run:
	sudo ./nlsniff.out -g ipv4route,ipv6route
