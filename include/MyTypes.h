#ifndef MYTYPES_H
#define MYTYPES_H

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

// FEN Notation
#define starting_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define kiwipete "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"

// Color & Move Types
#define MAX_MOVES 256
#define FLAG_NONE 0
#define FLAG_ENPASSANT 1
#define FLAG_CASTLE_KINGSIDE 2
#define FLAG_CASTLE_QUEENSIDE 3
#define FLAG_PROMOTION 4
#define FLAG_DOUBLE_PUSH 5
#define WHITE 0
#define BLACK 1

// Macros
#define SetBit(board, square) ((board) |= (1ULL << (square)))
#define GetBit(board, square) ((board) & (1ULL << (square)))
#define PopBit(board, square) ((board) &= ~(1ULL << (square)))

// Typedef
typedef unsigned long long U64;

typedef struct
{
    int piece;     // piece type of bitboard
    int from;      // square index the piece moves from
    int to;        // square index the piece moves to
    int promotion; // piece type of promotted piece
    int captured;  // piece type of captured piece
    int flags;     // handles special moves
} Move;

typedef enum
{
    P,N,B,R,Q,K, // white
    p,n,b,r,q,k // black
} Pieces;

#endif
