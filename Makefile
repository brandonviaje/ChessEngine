# Compiler
CC = gcc

# Compiler flags
CFLAGS = -g -Wall

# Directories
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build

# Source files
SRCS = $(SRC_DIR)/BitBoard.c \
       $(SRC_DIR)/MoveGen.c \
       $(SRC_DIR)/Attack.c \
       $(SRC_DIR)/Magic.c \
       $(TEST_DIR)/Perft.c

# Object files placed under build
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)

# Output executable
TARGET = chess_engine

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Compile *.c → build/*.o
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure build exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)