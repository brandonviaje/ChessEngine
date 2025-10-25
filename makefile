# Makefile for chess engine tests

CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = test_bitboard
SRC = test_bitboard/test_bitboard.c test_bitboard/unity.c test_bitboard/test_bitboard.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
