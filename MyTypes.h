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
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1"


// Typedef
typedef unsigned long long U64;

typedef enum {
    P, N, B, R, Q, K,   // white
    p, n, b, r, q, k    // black
} Pieces;

typedef struct {
    int from;    // square index the piece moves from
    int to;      // square index the piece moves to
    int flags;   // bitmask for promotions, en passant, castling, etc.
} Move;

#endif