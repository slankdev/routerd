
CXX = g++
CXXFLAGS += -Wall -Werror -std=c++11 -I./libslankdev -g -O0
# CXXFLAGS += -fsanitize=address
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

docker_tools_install:
	# make -C tools/nldump
	# make -C tools/nldump install_docker
	make -C tools/nlsniff clean
	make -C tools/nlsniff
	make -C tools/nlsniff install_docker

re: clean all

run:
	sudo ./$(TARGET)

gdb:
	sudo gdb routerd.out

e:
	sudo ip route add fc00:20::1 encap seg6local action End dev dum0
	sudo ip route add fc00:20::2 encap seg6local action End.X nh6 2001:12::2 dev dum0
	sudo ip route add fc00:20::3 encap seg6local action End.DX2 oif dum0 dev dum0
	sudo ip route add fc00:20::4 encap seg6local action End.T table 100 dev dum0

d:
	sudo ip route del fc00:20::1 encap seg6local action End dev dum0
	sudo ip route del fc00:20::2 encap seg6local action End.X nh6 2001:12::2 dev dum0
	sudo ip route del fc00:20::3 encap seg6local action End.DX2 oif dum0 dev dum0
	sudo ip route del fc00:20::4 encap seg6local action End.T table 100 dev dum0

