
YALIN = ../../../libyalin
CXXFLAGS += -Wall -Werror -Wextra -std=c++11 -fsanitize=address -g -O0
CXXFLAGS += -I$(YALIN) -I../../../libslankdev
LDFLAGS += -L$(YALIN) -lyalin

SRC = main.cc
OBJ = $(SRC:.cc=.o)
TARGET = nlsniff

all: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET).out $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET).out

install:
	mkdir -p /usr/local/bin
	cp $(TARGET).out /usr/local/bin/$(TARGET)

install_docker:
	for c in `docker ps --format "{{.Names}}"`; do\
		docker cp $(TARGET).out $$c:/usr/bin/$(TARGET) ; done

