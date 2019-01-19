
CXX = g++
CXXFLAGS += -Wall -Werror -std=c++11 -I../../
CXXFLAGS += -fsanitize=address
LDFLAGS +=

SRC = main.cc
OBJ = $(SRC:.cc=.o)
TARGET = nlsniff.out

all: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

re: clean all

run:
	sudo ./a.out
