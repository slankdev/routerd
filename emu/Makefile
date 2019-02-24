
CXX = g++
PREFIX = ../
CXXFLAGS += -I$(PREFIX)/include -I$(PREFIX)/libslankdev
CXXFLAGS += -Wall -Werror -Wextra
CXXFLAGS += -std=c++11 -g -O0 -static
# CXXFLAGS += $(shell pkg-config json-c --cflags)
# LDFLAGS += $(shell pkg-config json-c --libs)
LDFLAGS += -lpthread


SRC = main.cc
OBJ = $(SRC:.cc=.o)
TARGET = trio.out

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

re: clean $(TARGET)
run: $(TARGET)
	sudo ./$(TARGET)

netns:
	# sudo ip netns add ns0
	sudo ip link set peer-et-0-0-0 netns ns0
	sudo ip link set peer-et-0-0-1 netns ns0
	sudo ip netns exec ns0 ip link set lo up
	sudo ip netns exec ns0 ip link set peer-et-0-0-0 up
	sudo ip netns exec ns0 ip link set peer-et-0-0-1 up
	sudo ip netns exec ns0 ip addr add 10.0.0.2/24 dev peer-et-0-0-0
	sudo ip netns exec ns0 ip addr add 10.1.0.2/24 dev peer-et-0-0-1
	sudo ip addr add 10.0.0.1/24 dev et-0-0-0
	sudo ip addr add 10.1.0.1/24 dev et-0-0-1

