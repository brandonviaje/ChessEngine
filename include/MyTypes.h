#ifndef MYTYPES_H
#define MYTYPES_H

// Libraries
#include <errno.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

// FEN Notation
#define starting_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

// Color & Move Types
#define MAX_MOVES 256
#define MAX_PLY 64
#define FLAG_NONE 0
#define FLAG_ENPASSANT 1
#define FLAG_CASTLE_KINGSIDE 2
#define FLAG_CASTLE_QUEENSIDE 3
#define FLAG_PROMOTION 4
#define FLAG_DOUBLE_PUSH 5
#define WHITE 0
#define BLACK 1
#define WHITE_CASTLE_K 1
#define WHITE_CASTLE_Q 2
#define BLACK_CASTLE_K 4
#define BLACK_CASTLE_Q 8

// Macros
#define SetBit(board, square) ((board) |= (1ULL << (square)))
#define GetBit(board, square) ((board) & (1ULL << (square)))
#define PopBit(board, square) ((board) &= ~(1ULL << (square)))
#define PopCount(bb) __builtin_popcountll(bb)
#define lsb(bb) __builtin_ctzll(bb)
#define pop_lsb(ptr) (*(ptr) &= (*(ptr) - 1))
#define MIRROR(sq) ((sq) ^ 56)

// Typedef
typedef unsigned long long U64;

// Move Structure
typedef struct
{
    int piece;     // piece type of bitboard
    int from;      // square index the piece moves from
    int to;        // square index the piece moves to
    int promotion; // piece type of promotted piece
    int captured;  // piece type of captured piece
    int flags;     // handles special moves
    int prevEnpassant;
    int prevCastle;
    int prevHalfMove;
} Move;

// MoveList Structure
typedef struct
{
    Move moves[MAX_MOVES];
    int count;
} MoveList;

// Pieces Enum
typedef enum
{
    P,
    N,
    B,
    R,
    Q,
    K, // white
    p,
    n,
    b,
    r,
    q,
    k // black
} Pieces;

// Game State
extern int side;
extern int enpassant;
extern int castlePerm;
extern unsigned char castle;
extern int halfmove;
extern int fullmove;

// Bitboards
extern U64 bitboards[12];
extern U64 whitePieces;
extern U64 blackPieces;
extern U64 occupied;

#endif