# Compiler
CC = gcc

# Compiler flags
CFLAGS = -g -Wall

# Source and Include directories
SRC_DIR = src
INCLUDE_DIR = src

# Source files
SRCS = $(SRC_DIR)/BitBoard.c $(SRC_DIR)/MoveGen.c

# Object files
OBJS = $(SRCS:.c=.o)

# Output executable
TARGET = chess_engine

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Compile
%.o: %.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Run program
run: $(TARGET)
	./$(TARGET)

# Clean build files
clean:
	rm -f $(TARGET) $(OBJS)