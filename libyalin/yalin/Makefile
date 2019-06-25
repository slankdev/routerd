
CXX = g++
LIBYALIN = ..
CXXFLAGS += -I$(LIBYALIN) -I../..//libslankdev
CXXFLAGS += -Wall -Werror -Wextra
CXXFLAGS += -std=c++11 -g -O0 -static
CXXFLAGS += $(shell pkg-config json-c --cflags)
LDFLAGS += $(shell pkg-config json-c --libs)

SRC = netlink_cache.cc yalin.cc netlink_socket.cc
OBJ = $(SRC:.cc=.o)
TARGET = libyalin.a

$(TARGET): $(OBJ)
	ar rcs $(TARGET) $(OBJ)
	mv $(TARGET) ../

clean:
	rm -f $(OBJ) $(TARGET)

re: clean $(TARGET)

