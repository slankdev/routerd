
CC = clang
CXX = clang++
VUI = ../src
CFLAGS += -I$(VUI) -D_GNU_SOURCE -g -O0
LDFLAGS += -L$(VUI) -lvui -lpthread -lm -lcrypt
CXXFLAGS += $(CFLAGS) -std=c++17

TARGET = a.out
BUILD_DIR = build
SRC = main.c netlink.c
OBJ = $(SRC:%=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@ $(LDFLAGS) -lstdc++

$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p ./build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cc.o: %.cc
	@mkdir -p ./build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
