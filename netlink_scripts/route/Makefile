
CFLAGS += -std=c99
CXXFLAGS +=
LDFLAGS +=

BUILD_DIR = build
SRC = main.c
OBJ = $(SRC:%=$(BUILD_DIR)/%.o)
TARGET = a.out

$(BUILD_DIR)/$(TARGET): $(OBJ)
	@echo ld $@
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $@ $(LDFLAGS) -lstdc++

$(BUILD_DIR)/%.c.o: %.c
	@echo cc $@
	@mkdir -p ./build
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cc.o: %.cc
	@echo g++ $@
	@mkdir -p ./build
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run:
	./build/a.out
