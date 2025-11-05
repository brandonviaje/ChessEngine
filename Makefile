# Makefile for Chess Engine

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -g -Wall

# Source files
SRCS = MoveGen.c BitBoard.c

# Output executable
TARGET = target

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

# Clean build files
clean:
	rm -f $(TARGET) *.o
