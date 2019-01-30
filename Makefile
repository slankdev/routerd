
CXX = g++
CXXFLAGS += -Wall -Werror -std=c++11 -I./libslankdev
CXXFLAGS += -fsanitize=address
LDFLAGS +=

SRC = main.cc
OBJ = $(SRC:.cc=.o)
TARGET = routerd.out

all: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

tools_install:
	make -C tools/nldump
	make -C tools/nldump install
	make -C tools/nlsniff
	make -C tools/nlsniff install

re: clean all

run:
	sudo ./$(TARGET)

