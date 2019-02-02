
CXX = g++
CXXFLAGS += -Wall -Werror -std=c++11 -I../../ -I../../libslankdev
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
	mkdir -p /usr/local/bin
	cp $(TARGET).out /usr/local/bin/$(TARGET)

install_docker:
	for c in `docker ps --format "{{.Names}}"`; do\
		docker cp nlsniff.out $$c:/usr/bin/nlsniff ; done

run: clean all install_docker
