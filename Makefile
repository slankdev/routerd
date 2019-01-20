
CXX = g++
CXXFLAGS += -Wall -Werror -std=c++11
CXXFLAGS += -fsanitize=address
LDFLAGS +=

SRC = main.cc
OBJ = $(SRC:.cc=.o)
TARGET = routerd.out

all: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

re: clean all

run:
	sudo ./$(TARGET)
